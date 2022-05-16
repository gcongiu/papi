/*
 * Copyright 2015-2016 NVIDIA Corporation. All rights reserved.
 *
 * Sample to demonstrate use of NVlink CUPTI APIs
 *
 * This version is significantly changed to use PAPI and the CUDA component to
 * handle access and reporting. As of 10/05/2018, I have deleted all CUPTI_ONLY
 * references, for clarity. The file nvlink_bandwidth_cupti_only.cu contains
 * the cupti-only code.  I also deleted the #if PAPI; there is no option
 * without PAPI.  Also, before my changes, the makefile did not even have a
 * build option that set CUPTI_ONLY for this file.
 *
 * -TonyC.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda.h>
#include "papi.h"
#include "papi_test.h"

#define MAX_DEVICES    (32)
#define BLOCK_SIZE     (1024)
#define GRID_SIZE      (512)
#define BUF_SIZE       (32 * 1024)
#define ALIGN_SIZE     (8)
#define SUCCESS        (0)
#define NUM_METRIC     (18)
#define NUM_EVENTS     (2)
#define MAX_SIZE       (64*1024*1024)   // 64 MB

typedef union
{
    long long ll;
    unsigned long long ull;
    double    d;
    void *vp;
    unsigned char ch[8];
} convert_64_t;

typedef struct {
    char name[128];
    long long value;
} eventStore_t;

int eventsFoundCount = 0;               // occupants of the array.
int eventsFoundMax;                     // Size of the array.
int eventsFoundAdd = 32;                // Blocksize for increasing the array.
eventStore_t *eventsFound = NULL;       // The array.

int Streams;                            // Gets asyncEngineCount (number of physical copy engines).
int cpuToGpu = 0;
int gpuToGpu = 0;
size_t bufferSize = 0;

int         *deviceEvents = NULL;
CUdeviceptr *pDevBuffer0  = NULL;
CUdeviceptr *pDevBuffer1  = NULL;
float       **pHostBuffer = NULL;
cudaStream_t *cudaStreams = NULL;

extern "C" __global__ void test_nvlink_bandwidth(float *source, float *dest)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    dest[idx] = source[idx] * 2.0f;
}

#define DIM(x) (sizeof(x)/sizeof(*(x))) /* compute elements in an array */

void FreeGlobals(void)
{
    int i;
    cudaError_t cuda_errno = cudaSuccess;
    free(deviceEvents);

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaSetDevice(0);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
        }

        cuda_errno = cudaFree((void **) &pDevBuffer0[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaFree failed.", 0);
        }

        free(pHostBuffer[i]);
    }

    free(pDevBuffer0);
    free(pHostBuffer);
    free(pDevBuffer1);

    for (i = 0; i < Streams; i++) {
        if (cudaStreams[i] != NULL) {
            cuda_errno = cudaStreamDestroy(cudaStreams[i]);
            if (cuda_errno != cudaSuccess) {
                test_fail(__FILE__, __LINE__, "cudaStreamDestroy failed.", 0);
            }
        }
    }

    free(cudaStreams);
}

void calculateSize(char *result, uint64_t size)
{
    int i;

    const char *sizes[] = { "TB", "GB", "MB", "KB", "B" };
    uint64_t exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL;

    uint64_t multiplier = exbibytes;

    for (i = 0; (unsigned) i < DIM(sizes); i++, multiplier /= (uint64_t) 1024) {
        if (size < multiplier)
            continue;
        sprintf(result, "%.1f %s", (float) size / multiplier, sizes[i]);
        return;
    }
    strcpy(result, "0");
    return;
}

void testCpuToGpu(CUdeviceptr * pDevBuffer, float **pHostBuffer, size_t bufferSize,
                  cudaStream_t * cudaStreams)
{
    int i;
    cudaError_t cuda_errno = cudaSuccess;

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaMemcpyAsync((void *) pDevBuffer[i], pHostBuffer[i],
                                     bufferSize, cudaMemcpyHostToDevice,
                                     cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpyHostToDevice failed", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaMemcpyAsync(pHostBuffer[i], (void *) pDevBuffer[i],
                                     bufferSize, cudaMemcpyDeviceToHost,
                                     cudaStreams[i]);
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaMemcpyAsync failed.", 0);
    }

    for (i = 0; i < Streams; i += 2) {
        cuda_errno = cudaMemcpyAsync((void *) pDevBuffer[i], pHostBuffer[i],
                                     bufferSize, cudaMemcpyHostToDevice,
                                     cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpyAsync failed.", 0);
        }

        cuda_errno = cudaMemcpyAsync(pHostBuffer[i + 1], (void *) pDevBuffer[i + 1],
                                     bufferSize, cudaMemcpyDeviceToHost,
                                     cudaStreams[i + 1]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpyAsync failed.", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }
}

void testGpuToGpu_part1(CUdeviceptr * pDevBuffer0, CUdeviceptr * pDevBuffer1,
                        float **pHostBuffer, size_t bufferSize,
                        cudaStream_t * cudaStreams)
{
    int i;
    cudaError_t cuda_errno = cudaSuccess;

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaMemcpyAsync((void *) pDevBuffer0[i], pHostBuffer[i],
                                     bufferSize, cudaMemcpyHostToDevice,
                                     cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpyAsync failed.", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaMemcpyAsync((void *) pDevBuffer1[i], pHostBuffer[i],
                                     bufferSize, cudaMemcpyHostToDevice,
                                     cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpuAsync failed.", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }
}

void testGpuToGpu_part2(CUdeviceptr * pDevBuffer0, CUdeviceptr * pDevBuffer1,
                        float **pHostBuffer, size_t bufferSize,
                        cudaStream_t * cudaStreams)
{
    int i;
    cudaError_t cuda_errno = cudaSuccess;

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaMemcpyAsync((void *) pDevBuffer0[i], (void *) pDevBuffer1[i],
                                     bufferSize, cudaMemcpyDeviceToDevice,
                                     cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpyAsync failed.", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }

    for(i = 0; i < Streams; i++) {
        cuda_errno = cudaMemcpyAsync((void *) pDevBuffer1[i], (void *) pDevBuffer0[i],
                                     bufferSize, cudaMemcpyDeviceToDevice,
                                     cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMemcpyAsync failed.", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }

    for(i = 0; i < Streams; i++) {
        test_nvlink_bandwidth<<<GRID_SIZE, BLOCK_SIZE>>>((float *) pDevBuffer1[i],
                                                         (float *) pDevBuffer0[i]);
    }
}

void conductCpuToGpu(int EventSet, int device, long long *values)
{
    int i;
    int papi_errno = PAPI_OK;
    cudaError_t cuda_errno = cudaSuccess;

    if (device == 0) {
        papi_errno = PAPI_start(EventSet);
        if (papi_errno != PAPI_OK) {
            test_fail(__FILE__, __LINE__, "PAPI_start failed.", papi_errno);
        }

        testCpuToGpu(pDevBuffer0, pHostBuffer, bufferSize, cudaStreams);
    } else {
        cuda_errno = cudaSetDevice(device);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
        }

        for(i = 0; i < Streams; i++) {
            cuda_errno = cudaMalloc((void **) &pDevBuffer1[i], bufferSize);
            if (cuda_errno != cudaSuccess) {
                test_fail(__FILE__, __LINE__, "cudaMalloc failed.", 0);
            }
        }

        papi_errno = PAPI_start(EventSet);
        if (papi_errno != PAPI_OK) {
            test_fail(__FILE__, __LINE__, "PAPI_start failed.", papi_errno);
        }

        testCpuToGpu(pDevBuffer1, pHostBuffer, bufferSize, cudaStreams);

        for (i = 0; i < Streams; i++) {
            cuda_errno = cudaFree((void **) pDevBuffer1[i]);
            if (cuda_errno != cudaSuccess) {
                test_fail(__FILE__, __LINE__, "cudaFree failed.", 0);
            }
        }
    }

    papi_errno = PAPI_stop(EventSet, values);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_stop failed.", papi_errno);
    }
}

void conductGpuToGpu(int EventSet, int device, long long *values) {
    int i;
    int partner = device;
    cudaError_t cuda_errno = cudaSuccess;
    int papi_errno = PAPI_OK;

    if (device == 0) {
        partner = 1;
    }

    cuda_errno = cudaSetDevice(0);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
    }

    cuda_errno = cudaDeviceEnablePeerAccess(partner, 0);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceEnablePeerAccess failed.", 0);
    }

    cuda_errno = cudaSetDevice(partner);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
    }

    cuda_errno = cudaDeviceEnablePeerAccess(0, 0);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceEnablePeerAccess failed.", 0);
    }

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaMalloc((void **) &pDevBuffer1[i], bufferSize);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMalloc failed.", 0);
        }
    }

    testGpuToGpu_part1(pDevBuffer0, pDevBuffer1, pHostBuffer, bufferSize,
                       cudaStreams);

    papi_errno = PAPI_start(EventSet);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_start failed.", papi_errno);
    }

    testGpuToGpu_part2(pDevBuffer0, pDevBuffer1, pHostBuffer, bufferSize,
                       cudaStreams);

    papi_errno = PAPI_stop(EventSet, values);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_stop failed.", papi_errno);
    }

    cuda_errno = cudaSetDevice(0);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
    }

    cuda_errno = cudaDeviceDisablePeerAccess(partner);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceDisablePeerAccess failed.", 0);
    }

    cuda_errno = cudaSetDevice(partner);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
    }

    cuda_errno = cudaDeviceDisablePeerAccess(0);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceDisablePeerAccess failed.", 0);
    }

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaFree((void **) pDevBuffer1[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaFree failed.", 0);
        }
    }

    cuda_errno = cudaSetDevice(0);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaSetDevice failed.", 0);
    }
}

static void printUsage()
{
    printf("usage: Demonstrate use of NVlink CUPTI APIs\n");
    printf("    -h, -help, --help: display this help message.\n");
    printf("    Otherwise, exactly one of these options:\n");
    printf("    --cpu-to-gpu: Show results for data transfer between CPU and GPU.\n");
    printf("    --gpu-to-gpu: Show results for data transfer between two GPUs.\n");
}

void parseCommandLineArgs(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Invalid number of options\n");
        printUsage();
        exit(EXIT_SUCCESS);
    }

    if (strcmp(argv[1], "--cpu-to-gpu") == 0) {
        cpuToGpu = 1;
    } else if (strcmp(argv[1], "--gpu-to-gpu") == 0) {
        gpuToGpu = 1;
    } else if ((strcmp(argv[1], "--help") == 0) ||
               (strcmp(argv[1], "-help") == 0)  ||
               (strcmp(argv[1], "-h") == 0)) {
        printUsage();
        exit(EXIT_SUCCESS);
    } else {
        printf("Failed to understand argument '%s'.\n", argv[1]);
        printUsage();
        exit(EXIT_FAILURE);
    }
}

void addEventsFound(char *eventName, long long value)
{
    strncpy(eventsFound[eventsFoundCount].name, eventName, 127);
    eventsFound[eventsFoundCount].value = value;

    if (++eventsFoundCount >= eventsFoundMax) {
        eventsFoundMax += eventsFoundAdd;
        eventsFound =
            (eventStore_t*) realloc(eventsFound,
                                    eventsFoundMax * sizeof(eventStore_t));
        memset(eventsFound+(eventsFoundMax - eventsFoundAdd), 0,
               eventsFoundAdd * sizeof(eventStore_t));
    }
}

int main(int argc, char *argv[])
{
    int device, deviceCount = 0, i = 0;
    size_t freeMemory = 0, totalMemory = 0;
    char str[64];
    int papi_errno, cu_errno;
    cudaError_t cuda_errno;

    eventsFoundMax = eventsFoundAdd;
    eventsFound = (eventStore_t*) calloc(eventsFoundMax, sizeof(eventStore_t));

    cudaDeviceProp prop[MAX_DEVICES];

    // Parse command line arguments
    parseCommandLineArgs(argc, argv);

    cu_errno = cuInit(0);
    if (cu_errno != CUDA_SUCCESS) {
        test_fail(__FILE__, __LINE__, "cuInit failed.", 0);
    }

    cuda_errno = cudaGetDeviceCount(&deviceCount);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaGetDeviceCount failed.", 0);
    }

    printf("There are %d devices.\n", deviceCount);

    if(deviceCount == 0) {
        printf("There is no device supporting CUDA.\n");
        exit(EXIT_FAILURE);
    }

    Streams = 1;
    for (device = 0; device < deviceCount; device++) {
        cuda_errno = cudaGetDeviceProperties(&prop[device], device);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaGetDeviceProperties failed.", 0);
        }

        printf("CUDA Device %d Name: %s", i, prop[i].name);
        printf(", AsyncEngineCount=%i", prop[i].asyncEngineCount);
        printf(", MultiProcessors=%i", prop[i].multiProcessorCount);
        printf(", MaxThreadsPerMP=%i", prop[i].maxThreadsPerMultiProcessor);
        printf("\n");

        if (prop[i].asyncEngineCount > Streams) {
            Streams = prop[i].asyncEngineCount;
        }
    }

    printf("Streams to use: %i (= max Copy Engines).\n", Streams);

    deviceEvents= (int*)          calloc(deviceCount, sizeof(int));
    pDevBuffer0 = (CUdeviceptr*)  calloc(Streams, sizeof(CUdeviceptr));
    pDevBuffer1 = (CUdeviceptr*)  calloc(Streams, sizeof(CUdeviceptr));
    pHostBuffer = (float **)      calloc(Streams, sizeof(float*));
    cudaStreams = (cudaStream_t*) calloc(Streams, sizeof(cudaStream_t));

    cuda_errno = cudaMemGetInfo(&freeMemory, &totalMemory);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaMemGetInfo failed.", 0);
    }

    printf("Total Device Memory available : ");
    calculateSize(str, (uint64_t) totalMemory);
    printf("%s\n", str);

    bufferSize = MAX_SIZE < (freeMemory / 4) ? MAX_SIZE : (freeMemory / 4);
    bufferSize = bufferSize/2;
    printf("Memcpy size is set to %llu B (%llu MB)\n",
           (unsigned long long) bufferSize,
           (unsigned long long) bufferSize / (1024 * 1024));

    for (i = 0; i < Streams; i++) {
        cuda_errno = cudaStreamCreate(&cudaStreams[i]);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaStreamCreate failed.", 0);
        }
    }

    cuda_errno = cudaDeviceSynchronize();
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaDeviceSynchronize failed.", 0);
    }

    int EventSet = PAPI_NULL;
    int eventCount;
    int retval;
    int k, m, cid = -1;

    /* PAPI Initialization */
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if(retval != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI_library_init failed, ret=%i [%s]\n",
                retval, PAPI_strerror(retval));
        FreeGlobals();
        exit(EXIT_FAILURE);
    }

    printf("PAPI version: %d.%d.%d\n", PAPI_VERSION_MAJOR(PAPI_VERSION),
                                       PAPI_VERSION_MINOR(PAPI_VERSION),
                                       PAPI_VERSION_REVISION(PAPI_VERSION));

    k = PAPI_num_components();
    for (i = 0; i < k && cid < 0; i++) {
        PAPI_component_info_t *aComponent = 
            (PAPI_component_info_t*) PAPI_get_component_info(i);
        if (aComponent == NULL) {
            fprintf(stderr, "PAPI_get_component_info(%i) failed, "
                    "returned NULL. %i components reported.\n", i, k);
            FreeGlobals();
            exit(EXIT_FAILURE);
        }

       if (strcmp("cuda", aComponent->name) == 0) cid = i;
    }

    if (cid < 0) {
        fprintf(stderr, "Failed to find cuda component among %i "
               "reported components.\n", k);
        FreeGlobals();
        PAPI_shutdown();
        exit(EXIT_FAILURE);
    }

    printf("Found CUDA Component at id %d\n", cid);

    eventCount = 0;
    int eventsRead = 0;

    for(i = 0; i < Streams; i++) {
        cuda_errno = cudaMalloc((void **) &pDevBuffer0[i], bufferSize);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMalloc failer.", 0);
        }

        pHostBuffer[i] = (float *) malloc(bufferSize);
        if (pHostBuffer[i] == NULL) {
            test_fail(__FILE__, __LINE__, "malloc failed.", 0);
        }
    }

    if (cpuToGpu) {
       printf("Experiment timing memory copy from host to GPU.\n");
    }

    if (gpuToGpu) {
        printf("Experiment timing memory copy between GPU 0 and each other GPU.\n");
    }

   printf("Events with numeric values were read; if they are zero, they may not  \n"
          "be operational, or the exercises performed by this code do not affect \n"
          "them. We report all 'nvlink' events presented by the cuda component.  \n"
          "\n"
          "---------------------------Event Name---------------------------:---Value---\n");

    PAPI_event_info_t info;
    m = PAPI_NATIVE_MASK;
    papi_errno = PAPI_enum_cmp_event(&m, PAPI_ENUM_FIRST, cid);
    do {
        memset(&info,0,sizeof(PAPI_event_info_t));
        k = m;

        do {
            papi_errno = PAPI_get_event_info(k, &info);

            if (strstr(info.symbol, "nvlink") == NULL)
                continue;
            char *devstr = strstr(info.symbol, "device=");
            if (devstr == NULL)
                continue;
            device = atoi(devstr + 7);
            if (device < 0 || device >= deviceCount)
                continue;
            deviceEvents[device]++;

            papi_errno = PAPI_create_eventset(&EventSet);
            if (papi_errno != PAPI_OK) {
                test_fail(__FILE__, __LINE__, "PAPI_create_eventset failed.",
                          papi_errno);
            }

            papi_errno = PAPI_assign_eventset_component(EventSet, cid);
            if (papi_errno != PAPI_OK) {
                test_fail(__FILE__, __LINE__, "PAPI_assign_eventset_component failed.",
                          papi_errno);
            }

            papi_errno = PAPI_add_named_event(EventSet, info.symbol);
            if(papi_errno == PAPI_OK) {
                eventCount++;
            } else {
                papi_errno = PAPI_cleanup_eventset(EventSet);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.",
                              papi_errno);
                }

                papi_errno = PAPI_destroy_eventset(&EventSet);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.",
                              papi_errno);
                }
                continue;
            }

            long long value = -1;

            if(cpuToGpu) {
                conductCpuToGpu(EventSet, device, &value);
            } else if(gpuToGpu) {
                conductGpuToGpu(EventSet, device, &value);
            }

            addEventsFound(info.symbol, value);

            papi_errno = PAPI_cleanup_eventset(EventSet);
            if (papi_errno != PAPI_OK) {
                test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.", 0);
            }

            papi_errno = PAPI_destroy_eventset(&EventSet);
            if (papi_errno != PAPI_OK) {
                test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.", 0);
            }

            if (value >= 0) {
                eventsRead++;
                calculateSize(str, value);
                if (value == 0) {
                    printf("%-64s: %9s (not exercised by current test code.)\n",
                           info.symbol, str);
                } else {
                    printf("%-64s: %9s\n", info.symbol, str);
                }
            } else {
                printf("%-64s: Failed to read.\n", info.symbol);
            }
        } while (PAPI_enum_cmp_event(&k, PAPI_NTV_ENUM_UMASKS, cid) == PAPI_OK);
    } while (PAPI_enum_cmp_event(&m, PAPI_ENUM_EVENTS, cid) == PAPI_OK);

    if (eventCount < 1) {
        fprintf(stderr, "Unable to add any NVLINK events; they are not present in the component.\n");
        fprintf(stderr, "Unable to proceed with this test.\n");
        FreeGlobals();
        PAPI_shutdown();
        exit(EXIT_FAILURE);
    }

    if (eventsRead < 1) {
        printf("\nFailed to read any nvlink events.\n");
        fprintf(stderr, "Unable to proceed with this test.\n");
        FreeGlobals();
        PAPI_shutdown();
        exit(EXIT_FAILURE);
    }

    printf("\nTotal nvlink events identified: %i.\n\n", eventsFoundCount);
    if (eventsFoundCount < 2) {
        printf("Insufficient events are exercised by the current test code to perform pair testing.\n");
        FreeGlobals();
        PAPI_shutdown();
        exit(EXIT_SUCCESS);
    }

    for (i = 0; i < deviceCount; i++) {
        printf("Device %i has %i events. %i potential pairings per device.\n",
               i, deviceEvents[i], deviceEvents[i] * (deviceEvents[i]-1) / 2);
    }

    // Begin pair testing. We consider every possible pairing of events
    // that, tested alone, returned a value greater than zero.

    int mainEvent, pairEvent, mainDevice, pairDevice;
    long long saveValues[2];
    long long readValues[2];
    int  goodOnSame = 0, failOnDiff = 0, badSameCombo = 0, pairProblems = 0;
    int type;
    for (type = 0; type < 2; type++) {
        if (type == 0) {
            printf("List of Pairings on SAME device:\n");
            printf("* means value changed by more than 10%% when paired (vs measured singly, above).\n");
            printf("^ means a pair was rejected as an invalid combo.\n");
        } else {
            printf("List of Failed Pairings on DIFFERENT devices:\n");
        }

        for (mainEvent = 0; mainEvent < eventsFoundCount - 1; mainEvent++) {
             char *devstr = strstr(eventsFound[mainEvent].name, "device=");
             mainDevice = atoi(devstr + 7);

            for (pairEvent = mainEvent + 1; pairEvent < eventsFoundCount; pairEvent++) {
                devstr = strstr(eventsFound[pairEvent].name, "device=");
                pairDevice=atoi(devstr + 7);

                if (type == 0 && mainDevice != pairDevice)
                    continue;
                if (type == 1 && mainDevice == pairDevice)
                    continue;

                papi_errno = PAPI_create_eventset(&EventSet);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_create_eventset failed.",
                              papi_errno);
                }

                papi_errno = PAPI_assign_eventset_component(EventSet, cid);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_assign_eventset_component failed.",
                              papi_errno);
                }

                papi_errno = PAPI_add_named_event(EventSet, eventsFound[mainEvent].name);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_add_named_event failed.",
                              papi_errno);
                }

                papi_errno = PAPI_add_named_event(EventSet, eventsFound[pairEvent].name);
                if (type == 0 && papi_errno == PAPI_ECOMBO) {
                    printf("%c %64s + %-64s [Invalid Combo]\n", '^',
                        eventsFound[mainEvent].name, eventsFound[pairEvent].name);
                    badSameCombo++;
                    papi_errno = PAPI_cleanup_eventset(EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.",
                                  papi_errno);
                    }

                    papi_errno = PAPI_destroy_eventset(&EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.",
                                  papi_errno);
                    }
                    continue;
                }

                if (type == 1 && papi_errno == PAPI_ECOMBO) {
                    printf("%64s + %-64s BAD COMBINATION ACROSS DEVICES.\n",
                        eventsFound[mainEvent].name, eventsFound[pairEvent].name);
                    failOnDiff++;
                    papi_errno = PAPI_cleanup_eventset(EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.",
                                  papi_errno);
                    }

                    papi_errno = PAPI_destroy_eventset(&EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.",
                                  papi_errno);
                    }
                    continue;
                }

                if (papi_errno != PAPI_OK) {
                    fprintf(stderr, "%s:%d Attempt to add event '%s' to set "
                            "with event '%s' produced an unexpected error: "
                            "[%s]. Ignoring this pair.\n",
                            __FILE__, __LINE__, eventsFound[pairEvent],
                            eventsFound[mainEvent], PAPI_strerror(papi_errno));
                    papi_errno = PAPI_cleanup_eventset(EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.",
                                  papi_errno);
                    }

                    papi_errno = PAPI_destroy_eventset(&EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.",
                                  papi_errno);
                    }
                    continue;
                }

                // We were able to add the pair. In type 1, we just skip it,
                // because we presume a single event on a device isn't changed
                // by any event on another device.
                if (type == 1) {
                    papi_errno = PAPI_cleanup_eventset(EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.",
                                  papi_errno);
                    }

                    papi_errno = PAPI_destroy_eventset(&EventSet);
                    if (papi_errno != PAPI_OK) {
                        test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.",
                                  papi_errno);
                    }
                    continue;
                }

                // We were able to add the pair, in type 0, get a measurement.
                readValues[0] = -1; readValues[1] = -1;

                if(cpuToGpu) {
                    conductCpuToGpu(EventSet, mainDevice, readValues);
                    saveValues[0] = readValues[0];
                    saveValues[1] = readValues[1];
                } else if(gpuToGpu) {
                    conductGpuToGpu(EventSet, mainDevice, readValues);
                    saveValues[0] = readValues[0];
                    saveValues[1] = readValues[1];
                }

                goodOnSame++;

                // For the checks, we add 2 (so -1 becomes +1) to avoid any
                // divide by zeros. It won't make a significant difference
                // in the ratios. (none if readings are the same).
                double mainSingle = (2.0 + eventsFound[mainEvent].value);
                double pairSingle = (2.0 + eventsFound[pairEvent].value);
                double mainCheck  = mainSingle/(2.0 + saveValues[0]);
                double pairCheck  = pairSingle/(2.0 + saveValues[1]);

                char flag=' ', flag1=' ', flag2=' ';
                if (mainCheck < 0.90 || mainCheck > 1.10) flag1='*';
                if (pairCheck < 0.90 || pairCheck > 1.10) flag2='*';
                if (flag1 == '*' || flag2 == '*') {
                    pairProblems++;
                    flag = '*';
                }

                printf("%c %64s + %-64s [", flag, eventsFound[mainEvent].name, eventsFound[pairEvent].name);
                calculateSize(str, saveValues[0]);
                printf("%c%9s,", flag1, str);
                calculateSize(str, saveValues[1]);
                printf("%c%9s]\n", flag2, str);

                papi_errno = PAPI_cleanup_eventset(EventSet);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.",
                              papi_errno);
                }

                papi_errno = PAPI_destroy_eventset(&EventSet);
                if (papi_errno != PAPI_OK) {
                    test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.",
                              papi_errno);
                }
            }
        }

        if (type == 0) {
            if (goodOnSame == 0) {
                printf("NO valid pairings of above events if both on the SAME device.\n");
            } else {
                printf("%i valid pairings of above events if both on the SAME device.\n", goodOnSame);
            }

            printf("%i unique pairings on SAME device were rejected as bad combinations.\n", badSameCombo);

            if (pairProblems > 0) {
                printf("%i pairings resulted in a change of one or both event values > 10%%.\n", pairProblems);
            } else {
                printf("No significant change in event values read for any pairings.\n");
            }
        } else {
            if (failOnDiff == 0) {
                printf("NO failed pairings of above events if each on a DIFFERENT device.\n");
            } else {
                printf("%i failed pairings of above events with each on a DIFFERENT device.\n", failOnDiff);
            }
        }
    }

    PAPI_shutdown();
    return(0);
}

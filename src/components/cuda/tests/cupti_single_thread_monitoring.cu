/*
 * This software contains source code provided by NVIDIA Corporation
 *
 * According to the Nvidia EULA (compute 5.5 version)
 * http://developer.download.nvidia.com/compute/cuda/5_5/rel/docs/EULA.pdf
 *
 * Chapter 2. NVIDIA CORPORATION CUDA SAMPLES END USER LICENSE AGREEMENT
 * 2.1.1. Source Code
 * Developer shall have the right to modify and create derivative works with the Source
 * Code. Developer shall own any derivative works ("Derivatives") it creates to the Source
 * Code, provided that Developer uses the Materials in accordance with the terms and
 * conditions of this Agreement. Developer may distribute the Derivatives, provided that
 * all NVIDIA copyright notices and trademarks are propagated and used properly and
 * the Derivatives include the following statement: “This software contains source code
 * provided by NVIDIA Corporation.”
 */

/*
 * This application demonstrates how to use the CUDA API to use multiple GPUs,
 * with an emphasis on simple illustration of the techniques (not on performance).
 *
 * Note that in order to detect multiple GPUs in your system you have to disable
 * SLI in the nvidia control panel. Otherwise only one GPU is visible to the
 * application. On the other side, you can still extend your desktop to screens
 * attached to both GPUs.
 */

#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <cupti.h>

#include "papi.h"
#include "papi_test.h"

typedef struct {
    int dataN;
    float *h_Data;
    float *h_Sum;
    float *d_Data,*d_Sum;
    float *h_Sum_from_device;
    cudaStream_t stream;
} TGPUplan;

const int MAX_GPU_COUNT = 32;
const int DATA_N = 48576 * 32;
const int MAX_NUM_EVENTS = 32;

__global__ static void reduceKernel(float *d_Result, float *d_Input, int N)
{
    const int tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int threadN = gridDim.x * blockDim.x;
    float sum = 0;

    for (int pos = tid; pos < N; pos += threadN) {
        sum += d_Input[pos];
    }

    d_Result[tid] = sum;
}

int main(int argc, char **argv)
{
    TGPUplan plan[MAX_GPU_COUNT];
    float h_SumGPU[MAX_GPU_COUNT];
    float sumGPU;
    double sumCPU, diff;
    int i, j, gpuBase, GPU_N;

    const int BLOCK_N = 32;
    const int THREAD_N = 256;
    const int ACCUM_N = BLOCK_N * THREAD_N;

    int quiet = tests_quiet(argc, argv);

    CUcontext ctx[MAX_GPU_COUNT];
    CUcontext poppedCtx;
    CUresult cu_errno = CUDA_SUCCESS;

    printf("Starting cupti_single_thread_monitoring\n");

    int papi_errno = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_errno != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI_library_init failed\n");
        test_fail(__FILE__, __LINE__, "PAPI_library_init failed.", 0);
    }

    printf("PAPI version: %d.%d.%d\n", PAPI_VERSION_MAJOR(PAPI_VERSION),
                                       PAPI_VERSION_MINOR(PAPI_VERSION),
                                       PAPI_VERSION_REVISION(PAPI_VERSION));

    int cc_major = 0, cc_minor = 0;
    int runtimeVersion = 0, driverVersion = 0;
    char deviceName[64];
    CUdevice device[MAX_GPU_COUNT];
    cudaError_t cuda_errno = cudaSuccess;

    cuda_errno = cudaGetDeviceCount(&GPU_N);
    if (cuda_errno != cudaSuccess) {
        test_fail(__FILE__, __LINE__, "cudaGetDeviceCount failed.", 0);
    }

    if (GPU_N > MAX_GPU_COUNT) {
        GPU_N = MAX_GPU_COUNT;
    }

    printf("CUDA-capable device count: %i\n", GPU_N);
    for (i = 0; i < GPU_N; i++) {
        cu_errno = cuDeviceGet(&device[i], i);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuDeviceGet failed.", 0);
        }

        cu_errno = cuDeviceGetName(deviceName, 64, device[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuDeviceGetName failed.", 0);
        }

        cu_errno = cuDeviceGetAttribute(&cc_major,
                                        CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR,
                                        device[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuDeviceGetAttribute failed.", 0);
        }

        cu_errno = cuDeviceGetAttribute(&cc_minor,
                                        CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR,
                                        device[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuDeviceGetAttribute failed.", 0);
        }

        cudaRuntimeGetVersion(&runtimeVersion);
        cudaDriverGetVersion(&driverVersion);

        printf("CUDA Device %d: %s : computeCapability %d.%d runtimeVersion %d.%d driverVersion %d.%d\n",
                i, deviceName, cc_major, cc_minor, runtimeVersion/1000, (runtimeVersion % 100) / 10,
                driverVersion / 1000, (driverVersion % 100) / 10);

        if (cc_major < 2) {
            printf("CUDA Device %d compute capability is too low... will not add any more GPUs\n", i);
            GPU_N = i;
            break;
        }
    }

    uint32_t cupti_linked_version;
    cuptiGetVersion(&cupti_linked_version);
    printf("CUPTI version: Compiled against version %d; Linked against version %d\n",
           CUPTI_API_VERSION, cupti_linked_version );

    for (i = 0; i < GPU_N; i++) {
        cu_errno = cuCtxCreate(&ctx[i], 0, device[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxCreate failed.", 0);
        }

        cu_errno = cuCtxPopCurrent(&poppedCtx);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPopCurrent failed.", 0);
        }
    }

    printf("Generating input data...\n");

    for (i = 0; i < GPU_N; i++)
        plan[i].dataN = DATA_N / GPU_N;

    for (i = 0; i < DATA_N % GPU_N; i++)
        plan[i].dataN++;

    gpuBase = 0;
    for (i = 0; i < GPU_N; i++) {
        plan[i].h_Sum = h_SumGPU + i;
        gpuBase += plan[i].dataN;
    }

    for (i = 0; i < GPU_N; i++) {
        cu_errno = cuCtxPushCurrent(ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPushCurrent failed.", 0);
        }

        cuda_errno = cudaStreamCreate(&plan[i].stream);
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaStreamCreate failed.", 0);
        }

        cuda_errno = cudaMalloc((void **) &plan[i].d_Data, plan[i].dataN * sizeof(float));
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMalloc failed." , 0);
        }

        cuda_errno = cudaMalloc((void **) &plan[i].d_Sum, ACCUM_N * sizeof(float));
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMalloc failed.", 0);
        }

        cuda_errno = cudaMallocHost((void **) &plan[i].h_Sum_from_device, ACCUM_N * sizeof(float));
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMallocHost failed.", 0);
        }

        cuda_errno = cudaMallocHost((void **) &plan[i].h_Data, plan[i].dataN * sizeof(float));
        if (cuda_errno != cudaSuccess) {
            test_fail(__FILE__, __LINE__, "cudaMallocHost failed.", 0);
        }

        for (j = 0; j < plan[i].dataN; j++) {
            plan[i].h_Data[j] = (float) rand() / (float) RAND_MAX;
        }

        cu_errno = cuCtxPopCurrent(&poppedCtx);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPopCurrent failed.", 0);
        }
    }

    if (!quiet) {
        printf("Setup PAPI counters internally (PAPI)\n");
    }

    int EventSet = PAPI_NULL;
    int NUM_EVENTS = MAX_GPU_COUNT * MAX_NUM_EVENTS;
    long long values[NUM_EVENTS];
    int eventCount;
    int cid = -1;
    int ee;

    int k = PAPI_num_components();
    for (i = 0; i < k && cid < 0; i++) {
        PAPI_component_info_t *aComponent =
            (PAPI_component_info_t*) PAPI_get_component_info(i);
        if (aComponent == NULL) {
            fprintf(stderr,  "PAPI_get_component_info(%i) failed, "
                    "returned NULL. %i components reported.\n", i, k);
            test_fail(__FILE__, __LINE__, "PAPI_get_component_info failed.", 0);
        }

        if (strcmp("cuda", aComponent->name) == 0) {
            cid = i;
        }
    }

    if (cid < 0) {
        fprintf(stderr, "Failed to find cuda component among %i "
                "reported components.\n", k);
        test_fail(__FILE__, __LINE__, "", 0);
    }

    if (!quiet) {
        printf("Found CUDA Component at id %d\n", cid);
    }

    papi_errno = PAPI_create_eventset(&EventSet);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_create_eventset failed.", papi_errno);
    }

    papi_errno = PAPI_assign_eventset_component(EventSet, cid);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_assign_eventset_component failed.",
                  papi_errno);
    }

    int numEventNames = 2;

    char const *EventNames[2];
    if (cc_major < 7 || (cc_major == 7 && cc_minor == 0)) {
        EventNames[0] = "cuda:::event:active_cycles_pm";
        EventNames[1] = "cuda:::event:active_warps_pm";
    } else {
        EventNames[0] = "cuda:::dram__bytes_read.sum";
        EventNames[1] = "cuda:::sm__warps_launched.sum";
    }

    // Add events at a GPU specific level ... eg cuda:::device:2:elapsed_cycles_sm
    // UNLIKE CUpti_11, we must change the contexts to the appropriate device to
    // add events to inform PAPI of the context that will run the kernels.

    // Save current context, will restore after adding events.
    CUcontext userContext;
    cu_errno = cuCtxGetCurrent(&userContext);
    if (cu_errno != CUDA_SUCCESS) {
        test_fail(__FILE__, __LINE__, "cuCtxGetCurrent failed.", 0);
    }

    char *EventName[NUM_EVENTS];
    char tmpEventName[64];
    eventCount = 0;
    for (i = 0; i < GPU_N; i++) {
        cu_errno = cuCtxSetCurrent(ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxSetCurrent failed.", 0);
        }

        for (ee = 0; ee < numEventNames; ee++) {
            snprintf(tmpEventName, 64, "%s:device=%d\0", EventNames[ee], i);
            papi_errno = PAPI_add_named_event(EventSet, tmpEventName);
            if (papi_errno == PAPI_OK) {
                if (!quiet) {
                    printf("Add event success: '%s' GPU %i\n", tmpEventName, i);
                }
                EventName[eventCount] = (char *)calloc(64, sizeof(char));
                snprintf(EventName[eventCount], 64, "%s", tmpEventName);
                eventCount++;
            } else {
                if (!quiet) {
                    printf("Add event failure: '%s' GPU %i error=%s\n",
                           tmpEventName, i, PAPI_strerror(papi_errno));
                }
            }
        }
    }

    cu_errno = cuCtxSetCurrent(userContext);
    if (cu_errno != CUDA_SUCCESS) {
        test_fail(__FILE__, __LINE__, "cuCtxSetCurrent failed.", 0);
    }

    papi_errno = PAPI_start(EventSet);
    if( papi_errno != PAPI_OK ) {
        test_fail(__FILE__, __LINE__, "PAPI_start failed.", papi_errno);
    }

    if (!quiet) {
        printf("Computing with %d GPUs...\n", GPU_N);
    }

    for (i = 0; i < GPU_N; i++) {
        cu_errno = cuCtxPushCurrent(ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPushCurrent failed.", 0);
        }

        cuda_errno = cudaMemcpyAsync(plan[i].d_Data, plan[i].h_Data,
                                     plan[i].dataN * sizeof( float ),
                                     cudaMemcpyHostToDevice, plan[i].stream );

        reduceKernel<<<BLOCK_N, THREAD_N, 0, plan[i].stream>>>(plan[i].d_Sum, plan[i].d_Data, plan[i].dataN);

        if (cudaGetLastError() != cudaSuccess) {
            printf( "reduceKernel() execution failed (GPU %d).\n", i );
            test_fail(__FILE__, __LINE__, "reduceKernel failed.", 0);
        }

        cuda_errno = cudaMemcpyAsync(plan[i].h_Sum_from_device, plan[i].d_Sum, ACCUM_N * sizeof( float ),
                                     cudaMemcpyDeviceToHost, plan[i].stream );

        cu_errno = cuCtxPopCurrent(&(ctx[i]));
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPopCurrent failed.", 0);
        }
    }

    if (!quiet) {
        printf("Process GPU results on %d GPUs...\n", GPU_N);
    }

    for (i = 0; i < GPU_N; i++) {
        float sum;
        cu_errno = cuCtxPushCurrent(ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPushCurrent failed.", 0);
        }

        cudaStreamSynchronize(plan[i].stream);

        sum = 0;
        for (j = 0; j < ACCUM_N; j++) {
            sum += plan[i].h_Sum_from_device[j];
        }

        *(plan[i].h_Sum) = (float) sum;

        cu_errno = cuCtxPopCurrent(&ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPopCurrent failed.", 0);
        }
    }

    for (i = 0; i < GPU_N; i++) {
        cu_errno = cuCtxPushCurrent(ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPushCurrnet failed.", 0);
        }

        cu_errno = cuCtxSynchronize();
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxSynchronize failed.", 0);
        }

        cu_errno = cuCtxPopCurrent(&ctx[i]);
        if (cu_errno != CUDA_SUCCESS) {
            test_fail(__FILE__, __LINE__, "cuCtxPopCurrent failed.", 0);
        }
    }

    papi_errno = PAPI_stop(EventSet, values);
    if (papi_errno != PAPI_OK ) {
        test_fail(__FILE__, __LINE__, "PAPI_stop failed.", papi_errno);
    }

    if (!quiet) {
        for (i = 0; i < eventCount; i++)
            printf("PAPI counterValue %12lld \t\t --> %s \n", values[i], EventName[i]);
    }

    int warp_size[GPU_N];
    int events_per_dev = eventCount / GPU_N;
    for (i = 0; i < GPU_N; i++) {
        cuDeviceGetAttribute(&warp_size[i], CU_DEVICE_ATTRIBUTE_WARP_SIZE, device[i]);
        if (values[(i * events_per_dev) + 1] != ((BLOCK_N * THREAD_N) / warp_size[i])) {
            test_fail(__FILE__, __LINE__, "", 0);
        }
    }

    papi_errno = PAPI_cleanup_eventset(EventSet);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_cleanup_eventset failed.", papi_errno);
    }

    papi_errno = PAPI_destroy_eventset(&EventSet);
    if (papi_errno != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_destroy_eventset failed.", papi_errno);
    }

    PAPI_shutdown();

    sumGPU = 0;
    for (i = 0; i < GPU_N; i++) {
        sumGPU += h_SumGPU[i];
    }

    if (!quiet) {
        printf("Computing the same result with Host CPU...\n");
    }

    sumCPU = 0;
    for (i = 0; i < GPU_N; i++) {
        for (j = 0; j < plan[i].dataN; j++) {
            sumCPU += plan[i].h_Data[j];
        }
    }

    if (!quiet) {
        printf("Comparing GPU and Host CPU results...\n");
    }

    diff = fabs(sumCPU - sumGPU) / fabs(sumCPU);

    if (!quiet) {
        printf("  GPU sum: %f\n  CPU sum: %f\n", sumGPU, sumCPU);
        printf("  Relative difference: %E \n", diff);
    }

    for (i = 0; i < GPU_N; i++) {
        cudaFreeHost(plan[i].h_Sum_from_device);
        cudaFreeHost(plan[i].h_Data);
        cudaFree(plan[i].d_Sum);
        cudaFree(plan[i].d_Data);
        cudaStreamDestroy(plan[i].stream);
    }

    if (diff < 1e-5) {
        test_pass(__FILE__);
    } else {
        test_fail(__FILE__, __LINE__, "", 0);
    }

    return 0;
}

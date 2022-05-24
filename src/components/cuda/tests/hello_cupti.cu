/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/**
 * @file    hello_cupti.cu
 * @author  Heike Jagode
 *          jagode@eecs.utk.edu
 * Mods:    Giuseppe Congiu
 *          gcongiu@icl.utk.edu
 * test case for Example component
 *
 *
 * @brief
 *  This file is a very simple HelloWorld C example which serves (together
 *  with its Makefile) as a guideline on how to add tests to components.
 *  The papi configure and papi Makefile will take care of the compilation
 *  of the component tests (if all tests are added to a directory named
 *  'tests' in the specific component dir).
 *  See components/README for more details.
 *
 *  The string "Hello Cupti!" is mangled and then restored.
 */

#include <stdio.h>
#include "papi.h"
#include "papi_test.h"
#include "common.h"

#define NUM_EVENTS 1

// Prototypes
__global__ void helloCupti(char*);

// Host function
int main(int argc, char** argv)
{
    int retval, i;
    int EventSet = PAPI_NULL;
    long long values[NUM_EVENTS];
    int events[NUM_EVENTS];
    int eventCount = 0;
    int quiet;

    quiet = tests_quiet(argc, argv);

    /* PAPI Initialization */
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        if (!quiet) {
            printf("PAPI init failed\n");
        }
        test_fail(__FILE__, __LINE__, "PAPI_library_init failed", 0);
    }

    if (!quiet) {
        printf("PAPI_VERSION     : %4d %6d %7d\n",
               PAPI_VERSION_MAJOR(PAPI_VERSION),
               PAPI_VERSION_MINOR(PAPI_VERSION),
               PAPI_VERSION_REVISION(PAPI_VERSION));
    }

    int cid = get_cuda_component_id();
    events[0] = 0 | PAPI_NATIVE_MASK;
    retval = PAPI_enum_cmp_event(&events[0], PAPI_ENUM_FIRST, cid);
    if (retval != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_enum_cmp_event failed.", retval);
    }

    PAPI_event_info_t event_info;
    retval = PAPI_get_event_info(events[0], &event_info);
    if (retval != PAPI_OK) {
        test_fail(__FILE__, __LINE__, "PAPI_get_event_info failed.", retval);
    }

    const char *EventName[NUM_EVENTS];
    EventName[0] = event_info.symbol;

#ifdef RUN_WITH_NON_PRIMARY_CONTEXT
    CUresult cuErr;
    CUcontext sessionCtx = NULL;

    cuErr = cuCtxCreate(&sessionCtx, 0, 0);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuCtxCreate cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__, __LINE__, "cuCtxCreate failed", 0);
    }
#endif

    for (i = 0; i < NUM_EVENTS; i++) {
        eventCount++;
        if (!quiet) {
            printf("Name %s --- Code: %#x\n", EventName[i], events[i]);
        }
    }

    if (eventCount == 0) {
        if (!quiet) {
            printf("Test FAILED: no valid events found.\n");
        }
        test_skip(__FILE__, __LINE__, "No events found", 0);
        return 1;
    }

    retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        if (!quiet) {
            printf("PAPI_create_eventset failed\n");
        }
        test_fail(__FILE__, __LINE__, "Cannot create eventset", retval);
    }

    /* Use the primary cuda context for the device */
    retval = PAPI_add_events(EventSet, events, eventCount);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI_add_events failed\n");
    }

    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI_start failed\n");
    }

    char str[] = "Hello Cupti!";

    int j;
    for (j = 0; j < 12; j++) {
        /* mangle contents of output */
        str[j] -= j;
    }

    char *d_str;
    size_t size = sizeof(str);
    cudaMalloc((void**)&d_str, size);
    cudaMemcpy(d_str, str, size, cudaMemcpyHostToDevice);

    dim3 dimGrid(2);  // one block per word
    dim3 dimBlock(6); // one thread per character

    helloCupti<<<dimGrid, dimBlock>>>(d_str);

    cudaMemcpy(str, d_str, size, cudaMemcpyDeviceToHost);
    cudaFree(d_str);

    if (!quiet) {
        printf("END: %s\n", str);
    }

    retval = PAPI_stop(EventSet, values);
    if (retval != PAPI_OK) {
        fprintf( stderr, "PAPI_stop failed\n" );
    }

    retval = PAPI_cleanup_eventset(EventSet);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI_cleanup_eventset failed\n");
    }

    retval = PAPI_destroy_eventset(&EventSet);
    if (retval != PAPI_OK) {
        fprintf(stderr, "PAPI_destroy_eventset failed\n");
    }

#ifdef RUN_WITH_NON_PRIMARY_CONTEXT
    CUcontext getCtx;
    cuErr = cuCtxPopCurrent(&getCtx);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuCtxPopCurrent cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__,__LINE__, "cuCtxPopCurrent failed", 0);
    }
    cuErr = cuCtxDestroy(sessionCtx);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuCtxDestroy cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__,__LINE__, "cuCtxDestroy failed", 0);
    }
#endif

    PAPI_shutdown();

    for (i = 0; i < eventCount; i++) {
        if (!quiet) {
            printf("%12lld \t\t --> %s \n", values[i], EventName[i]);
        }
    }

    test_pass(__FILE__);

    return 0;
}

__global__ void
helloCupti(char* str)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    str[idx] += idx;
}

#ifndef __COMMON_H__
#define __COMMON_H__

#include <cuda.h>

static int
get_cuda_component_id(void)
{
    int cid = -1;
    int cmp_count = PAPI_num_components();
    int i;

    for (i = 0; i < cmp_count; ++i) {
        const PAPI_component_info_t *cmp_info =
            PAPI_get_component_info(i);
        if (cmp_info == NULL) {
            test_fail(__FILE__, __LINE__, "PAPI_get_component_info failed.",
                      0);
        }

        if (strcmp("cuda", cmp_info->name) == 0) {
            cid = i;
            break;
        }
    }

    return cid;
}

static inline int
is_compute_capability_pre_cupti11()
{
    int cc_major, cc_minor;
    CUdevice device;
    CUresult cuErr;

    cuErr = cuInit(0);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuInit cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__, __LINE__, "cuInit failed", 0);
    }


    cuErr = cuDeviceGet(&device, 0);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuGetDevice cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__, __LINE__, "cuGetDevice failed", 0);
    }

    cuErr = cuDeviceGetAttribute(&cc_major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, device);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuGetAttribute cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__, __LINE__, "cuGetAttribute failed", 0);
    }

    cuErr = cuDeviceGetAttribute(&cc_minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, device);
    if (cuErr != CUDA_SUCCESS) {
        const char *errString = NULL;
        cuGetErrorString(cuErr, &errString);
        fprintf(stderr, "%s:%s:%i cuGetAttribute cudaError='%s'.\n",
                __FILE__, __func__, __LINE__, errString);
        test_fail(__FILE__, __LINE__, "cuGetAttribute failed", 0);
    }

    return (cc_major < 7 || (cc_major == 7 && cc_minor == 0));
}


#endif /* End of __COMMON_H__ */

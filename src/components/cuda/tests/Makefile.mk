##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

testlist       += src/components/cuda/tests/HelloWorld                  \
                  src/components/cuda/tests/simpleMultiGPU              \
                  src/components/cuda/tests/HelloWorld_CUPTI11          \
                  src/components/cuda/tests/HelloWorld_NP_Ctx           \
                  src/components/cuda/tests/nvlink_bandwidth            \
                  src/components/cuda/tests/cudaTest_cupti_only         \
                  src/components/cuda/tests/likeComp_cupti_only         \
                  src/components/cuda/tests/nvlink_all                  \
                  src/components/cuda/tests/nvlink_bandwidth_cupti_only

src_components_cuda_tests_HelloWorld_SOURCES =                          \
                  src/components/cuda/tests/HelloWorld.cu

src_components_cuda_tests_simpleMultiGPU_SOURCES =                      \
                  src/components/cuda/tests/simpleMultiGPU.cu

src_components_cuda_tests_HelloWorld_CUPTI11_SOURCES =                  \
                  src/components/cuda/tests/HelloWorld_CUPTI11.cu

src_components_cuda_tests_HelloWorld_NP_Ctx_SOURCES =                   \
                  src/components/cuda/tests/HelloWorld_NP_Ctx.cu

src_components_cuda_tests_nvlink_bandwidth_SOURCES =                    \
                  src/components/cuda/tests/nvlink_bandwidth.cu

src_components_cuda_tests_cudaTest_cupti_only_SOURCES =                 \
                  src/components/cuda/tests/cudaTest_cupti_only.cu

src_components_cuda_tests_likeComp_cupti_only_SOURCES =                 \
                  src/components/cuda/tests/likeComp_cupti_only.cu

src_components_cuda_tests_nvlink_all_SOURCES =                          \
                  src/components/cuda/tests/nvlink_all.cu

src_components_cuda_tests_nvlink_bandwidth_cupti_only_SOURCES =         \
                  src/components/cuda/tests/nvlink_bandwidth_cupti_only.cu

#TODO: add make recipes for libtool object using nvcc in the following
.cu.lo:

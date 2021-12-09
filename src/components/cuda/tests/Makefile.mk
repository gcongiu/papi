##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/cuda/tests/HelloWorld                  \
                  src/components/cuda/tests/simpleMultiGPU              \
                  src/components/cuda/tests/simpleMultiGPU_no_counters  \
                  src/components/cuda/tests/HelloWorld_CUPTI11          \
                  src/components/cuda/tests/HelloWorld_NP_Ctx           \
                  src/components/cuda/tests/nvlink_bandwidth            \
                  src/components/cuda/tests/nvlink_bw_plus              \
                  src/components/cuda/tests/cuda_event_only_test        \
                  src/components/cuda/tests/cudaTest_cupti_only         \
                  src/components/cuda/tests/likeComp_cupti_only         \
                  src/components/cuda/tests/nvlink_all                  \
                  src/components/cuda/tests/nvlink_bandwidth_cupti_only \
                  src/components/cuda/tests/nvidia_sample_bw_test       \
                  src/components/cuda/tests/p2pBandwidthLatencyTest

#TODO: add make recipes for libtool object using nvcc in the following
.cu.lo:

NAME=cuda
include ../../Makefile_comp_tests.target

PAPI_CUDA_ROOT ?= /opt/cuda
PAPI_CUPTI_ROOT ?= $(PAPI_CUDA_ROOT)/extras/CUPTI
CUDRV_DIR ?= $(PAPI_CUDA_ROOT)

TESTS = hello_cupti_primary_ctx        \
        hello_cupti_no_primary_ctx     \
        cupti_single_thread_monitoring \
        cupti_multi_kernel_launch_monitoring

cuda_tests: $(TESTS) 

NVCC     = $(PAPI_CUDA_ROOT)/bin/nvcc
NVCFLAGS = -g -ccbin='$(CC)'
INCLUDE += -I$(PAPI_CUDA_ROOT)/include -I$(PAPI_CUPTI_ROOT)/include
CUDALIBS = -L$(CUDRV_DIR)/lib64 -lcuda        \
           -L$(PAPI_CUDA_ROOT)/lib64 -lcudart \
           -L$(PAPI_CUDA_ROOT)/lib64/stubs    \
           -L$(PAPI_CUPTI_ROOT)/lib64 -lcupti \
           -Xlinker=-rpath="$(PAPI_CUDA_ROOT)/extras/CUPTI/lib64"
PAPILIB += -L../../../libpfm4/lib -lpfm       \
           -Xlinker=-rpath="../../../libpfm4/lib" \
           -Xlinker=-rpath="libpfm4/lib"

default: $(TESTS)

%.o:%.cu
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -c -o $@ $<

%.mac:%.cu
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -E -c -o $@ $<

cupti_multi_kernel_launch_monitoring: cupti_multi_kernel_launch_monitoring.o $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) -o cupti_multi_kernel_launch_monitoring cupti_multi_kernel_launch_monitoring.o $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

hello_cupti_primary_ctx: hello_cupti.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o $@ $^ $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

hello_cupti_no_primary_ctx: hello_cupti.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o $@ $^ $(PAPILIB) $(CUDALIBS) $(LDFLAGS) -DRUN_WITH_NON_PRIMARY_CONTEXT

cupti_single_thread_monitoring: cupti_single_thread_monitoring.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o $@ $^ $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

nvlink_bandwidth: nvlink_bandwidth.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

nvlink_all: nvlink_all.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

nvlink_bandwidth_cupti_only: nvlink_bandwidth_cupti_only.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o nvlink_bandwidth nvlink_all

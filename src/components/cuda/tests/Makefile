NAME=cuda
include ../../Makefile_comp_tests.target

PAPI_CUDA_ROOT ?= /opt/cuda
PAPI_CUPTI_ROOT ?= $(PAPI_CUDA_ROOT)/extras/CUPTI
CUDRV_DIR ?= $(PAPI_CUDA_ROOT)

TESTS = hello_cupti_primary_ctx        \
        hello_cupti_no_primary_ctx     \
        simpleMultiGPU                 \
        simpleMultiGPU_no_counters

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
           -Xlinker=-rpath="../../../libpfm4/lib"

default: $(TESTS)

%.o:%.cu
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -c -o $@ $<

%.mac:%.cu
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -E -c -o $@ $<

hello_cupti_primary_ctx: hello_cupti.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o $@ $^ $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

hello_cupti_no_primary_ctx: hello_cupti.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o $@ $^ $(PAPILIB) $(CUDALIBS) $(LDFLAGS) -DRUN_WITH_NON_PRIMARY_CONTEXT

simpleMultiGPU: simpleMultiGPU.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

simpleMultiGPU_CUPTI11: simpleMultiGPU_CUPTI11.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

simpleMultiGPU_no_counters: simpleMultiGPU.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DNO_COUNTERS -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

simpleMultiGPU_cupti_only: simpleMultiGPU.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DCUPTI_ONLY -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

nvlink_bandwidth: nvlink_bandwidth.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

cudaTest_cupti_only: cudaTest_cupti_only.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

likeComp_cupti_only: likeComp_cupti_only.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

nvlink_all: nvlink_all.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

nvlink_bandwidth_cupti_only: nvlink_bandwidth_cupti_only.cu $(UTILOBJS)
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -DPAPI -g -o $@ $+ $(UTILOBJS) $(PAPILIB) $(CUDALIBS) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o cuda_ld_preload_example.so nvlink_bandwidth nvlink_all


# Extra example to show LD_PRELOAD can be used to insert PAPI CUDA into a pre-existing binary
cuda_ld_preload_example.so: simpleMultiGPU_no_counters cuda_ld_preload_example.c
	gcc -Wall -fPIC -shared -o cuda_ld_preload_example.so $(LDFLAGS) $(INCLUDE) cuda_ld_preload_example.c ../../../libpapi.so

run: 
	( cd ~/icl/papi/papi/src/ && make )
	( make simpleMultiGPU )
	./simpleMultiGPU

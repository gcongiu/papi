# ***NOTE*** The Environment Variable PAPI_ROCM_ROOT must be defined for 
# programs to compile correctly. one typical location is /opt/rocm, but
# contact your sysadmin if you cannot find it.
NAME=rocm
HIP_PATH= ${PAPI_ROCM_ROOT}/hip
HIPCC=$(HIP_PATH)/bin/hipcc

include ../../Makefile_comp_tests.target
INCLUDE += -I$(PAPI_ROCM_ROOT)/hip/include
INCLUDE += -I$(PAPI_ROCM_ROOT)/hsa/include/hsa
INCLUDE += -I$(PAPI_ROCM_ROOT)/rocprofiler/include
INCLUDE += -I$(PAPI_ROCM_ROOT)/include
INCLUDE += -I$(PAPI_ROCM_ROOT)/rocblas/include
LDFLAGS = -ldl -g -L$(PAPI_ROCM_ROOT)/rocblas/lib -lrocblas

%.o:%.c
	@echo "INCLUDE=" $(INCLUDE)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

%.o:%.cpp
	@echo "INCLUDE=" $(INCLUDE)
	@echo "CFLAGS=" $(CFLAGS)
	@echo "CPPFLAGS" $(CPPFLAGS)
	g++ $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = 

rocm_tests: $(TESTS)

rocm_command_line: rocm_command_line.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o rocm_command_line rocm_command_line.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

rocm_standalone_report_all: rocm_standalone_report_all.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o rocm_standalone_report_all rocm_standalone_report_all.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

rocm_example.o: rocm_example.cpp $(UTILOBJS) $(PAPILIB)
	$(HIPCC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c $< -o $@

rocm_example: rocm_example.o $(UTILOBJS) $(PAPILIB)
	$(HIPCC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $< $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

rocm_all.o: rocm_all.cpp $(UTILOBJS) $(PAPILIB)
	$(HIPCC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c $< -o $@

rocm_all: rocm_all.o $(UTILOBJS) $(PAPILIB)
	$(HIPCC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $< $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o
	rm -f rocm_command_line rocm_standalone_report_all rocm_example rocm_all

checkpath: 
	echo PAPI_ROCM_ROOT = $(PAPI_ROCM_ROOT)
	echo HIP_PATH = $(HIP_PATH)
	echo HIPCC = $(HIPCC)
	echo INCLUDE = $(INCLUDE)
	echo LDFLAGS = $(LDFLAGS)
	echo CFLAGS = $(CFLAGS)


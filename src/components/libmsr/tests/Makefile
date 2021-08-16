NAME=libmsr
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = libmsr_basic

libmsr_tests: $(TESTS)

libmsr_basic: libmsr_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o libmsr_basic libmsr_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o

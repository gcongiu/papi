NAME=powercap
include ../../Makefile_comp_tests.target

TESTS = powercap_basic powercap_limit

powercap_tests: $(TESTS)

powercap_basic.o:	powercap_basic.c
	$(CC) $(CFLAGS) $(INCLUDE) -c powercap_basic.c -o powercap_basic.o -DBASIC_TEST=1

powercap_basic: powercap_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(INCLUDE) -o powercap_basic powercap_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)


powercap_limit.o:	powercap_limit.c
	$(CC) $(CFLAGS) $(INCLUDE) -c powercap_limit.c -o powercap_limit.o

powercap_limit: powercap_limit.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(INCLUDE) -o powercap_limit powercap_limit.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TESTS) *.o *~

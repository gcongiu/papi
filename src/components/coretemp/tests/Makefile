NAME=coretemp
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = coretemp_basic coretemp_pretty

coretemp_tests: $(TESTS)

coretemp_basic: coretemp_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o coretemp_basic coretemp_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

coretemp_pretty: coretemp_pretty.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o coretemp_pretty coretemp_pretty.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TESTS) *.o






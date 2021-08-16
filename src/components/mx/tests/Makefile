NAME=mx
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = mx_basic mx_elapsed

mx_tests: $(TESTS)

mx_basic: mx_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o mx_basic mx_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)


mx_elapsed: mx_elapsed.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o mx_elapsed mx_elapsed.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)


clean:
	rm -f $(TESTS) *.o






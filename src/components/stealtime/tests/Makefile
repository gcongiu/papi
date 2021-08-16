NAME=stealtime
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = stealtime_basic

stealtime_tests: $(TESTS)

stealtime_basic: stealtime_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(INCLUDE) -o stealtime_basic stealtime_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TESTS) *.o






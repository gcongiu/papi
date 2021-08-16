NAME=micpower
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = micpower_basic

micpower_tests: $(TESTS)

micpower_basic: micpower_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o micpower_basic micpower_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o






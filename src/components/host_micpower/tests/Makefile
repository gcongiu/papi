NAME=host_micpower
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OPTFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = host_micpower_basic
host_micpower_tests : $(TESTS)

micpower_tests: $(TESTS)

host_micpower_basic: host_micpower_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o host_micpower_basic host_micpower_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS)

clean:
	rm -f $(TESTS) *.o






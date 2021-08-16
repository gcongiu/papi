NAME=io
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = io_basic

io_tests: $(TESTS)

io_basic: io_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o io_basic io_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TESTS) *.o

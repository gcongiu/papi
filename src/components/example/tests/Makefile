NAME=example
include ../../Makefile_comp_tests.target

%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = example_basic example_multiple_components

example_tests: $(TESTS)

example_basic: example_basic.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o example_basic example_basic.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

example_multiple_components: example_multiple_components.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o example_multiple_components example_multiple_components.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TESTS) *.o

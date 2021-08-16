NAME=net
include ../../Makefile_comp_tests.target

TESTS = net_list_events net_values_by_code net_values_by_name

net_tests: $(TESTS)


%.o:%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c -o $@ $<

net_list_events: net_list_events.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o $@ net_list_events.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

net_values_by_code: net_values_by_code.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o $@ net_values_by_code.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

net_values_by_name: net_values_by_name.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -o $@ net_values_by_name.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TESTS) *.o


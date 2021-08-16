NAME=perf_event
include ../../Makefile_comp_tests.target

TESTS = broken_events nmi_watchdog perf_event_offcore_response perf_event_system_wide perf_event_user_kernel

DOLOOPS= $(testlibdir)/do_loops.o

perf_event_tests: $(TESTS)


event_name_lib.o:	event_name_lib.c event_name_lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c event_name_lib.c


broken_events.o:	broken_events.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c broken_events.c

broken_events:	broken_events.o event_name_lib.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(INCLUDE) -o broken_events broken_events.o event_name_lib.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)


nmi_watchdog.o:	nmi_watchdog.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c nmi_watchdog.c

nmi_watchdog:	nmi_watchdog.o event_name_lib.o $(UTILOBJS) $(PAPILIB)
	$(CC) $(INCLUDE) -o nmi_watchdog nmi_watchdog.o $(UTILOBJS) $(PAPILIB) $(LDFLAGS) $(LIBS)


perf_event_offcore_response.o:	perf_event_offcore_response.c event_name_lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c perf_event_offcore_response.c

perf_event_offcore_response:	perf_event_offcore_response.o event_name_lib.o $(UTILOBJS) $(DOLOOPS) $(PAPILIB)
	$(CC) $(INCLUDE) -o perf_event_offcore_response perf_event_offcore_response.o event_name_lib.o $(UTILOBJS) $(DOLOOPS) $(PAPILIB) $(LDFLAGS) $(LIBS)


perf_event_system_wide.o:	perf_event_system_wide.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c perf_event_system_wide.c

perf_event_system_wide:	perf_event_system_wide.o $(UTILOBJS) $(DOLOOPS) $(PAPILIB)
	$(CC) $(INCLUDE) -o perf_event_system_wide perf_event_system_wide.o $(UTILOBJS) $(DOLOOPS) $(PAPILIB) $(LDFLAGS) $(LIBS)


perf_event_user_kernel.o:	perf_event_user_kernel.c event_name_lib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE) -c perf_event_user_kernel.c

perf_event_user_kernel:	perf_event_user_kernel.o event_name_lib.o $(UTILOBJS) $(DOLOOPS) $(PAPILIB)
	$(CC) $(INCLUDE) -o perf_event_user_kernel perf_event_user_kernel.o event_name_lib.o $(UTILOBJS) $(DOLOOPS) $(PAPILIB) $(LDFLAGS) $(LIBS)


clean:
	rm -f $(TESTS) *.o *~


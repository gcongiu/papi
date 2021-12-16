##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

testlist       += src/components/perf_event/tests/broken_events                 \
                  src/components/perf_event/tests/nmi_watchdog                  \
                  src/components/perf_event/tests/perf_event_offcore_response   \
                  src/components/perf_event/tests/perf_event_system_wide        \
                  src/components/perf_event/tests/perf_event_user_kernel

src_components_perf_event_tests_broken_events_LDADD =                           \
                  src/components/perf_event/tests/event_name_lib.o              \
                  $(top_builddir)/src/testlib/libtestlib.la                     \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_tests_nmi_watchdog_LDADD  =                           \
                  src/components/perf_event/tests/event_name_lib.o              \
                  $(top_builddir)/src/testlib/libtestlib.la                     \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_tests_perf_event_offcore_response_LDADD =             \
                  src/components/perf_event/tests/event_name_lib.o              \
                  $(top_builddir)/src/testlib/libtestlib.la                     \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_tests_perf_event_system_wide =                        \
                  $(top_builddir)/src/testlib/libtestlib.la                     \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_tests_perf_event_user_kernel_LDADD =                  \
                  src/components/perf_event/tests/event_name_lib.o              \
                  $(top_builddir)/src/testlib/libtestlib.la                     \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

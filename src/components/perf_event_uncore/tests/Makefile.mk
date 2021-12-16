##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

testlist       += src/components/perf_event_uncore/tests/perf_event_uncore          \
                  src/components/perf_event_uncore/tests/perf_event_uncore_attach   \
                  src/components/perf_event_uncore/tests/perf_event_uncore_multiple \
                  src/components/perf_event_uncore/tests/perf_event_amd_northbridge \
                  src/components/perf_event_uncore/tests/perf_event_uncore_cbox

src_components_perf_event_uncore_tests_perf_event_uncore_LDADD =                    \
                  src/components/perf_event_uncore/tests/perf_event_uncore_lib.o    \
                  $(top_builddir)/src/testlib/libtestlib.la                         \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_uncore_tests_perf_event_uncore_attach_LDADD =             \
                  src/components/perf_event_uncore/tests/perf_event_uncore_lib.o    \
                  $(top_builddir)/src/testlib/libtestlib.la                         \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_uncore_tests_perf_event_uncore_multiple_LDADD =           \
                  src/components/perf_event_uncore/tests/perf_event_uncore_lib.o    \
                  $(top_builddir)/src/testlib/libtestlib.la                         \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_uncore_tests_perf_event_amd_northbridge_LDADD =           \
                  $(top_builddir)/src/testlib/libtestlib.la                         \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

src_components_perf_event_uncore_tests_perf_event_uncore_cbox_LDADD =               \
                  src/components/perf_event_uncore/tests/perf_event_uncore_lib.o    \
                  $(top_builddir)/src/testlib/libtestlib.la                         \
                  $(top_builddir)/lib/lib@PAPILIBNAME@.la

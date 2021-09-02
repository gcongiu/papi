##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_PERF_EVENT

noinst_HEADERS += src/components/perf_event/pe_libpfm4_events.h \
                  src/components/perf_event/perf_event_lib.h    \
                  src/components/perf_event/perf_helpers.h

papi_sources   += src/components/perf_event/pe_libpfm4_events.c \
                  src/components/perf_event/perf_event.c

endif

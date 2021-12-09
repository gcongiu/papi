##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_PERF_EVENT_UNCORE

papi_sources += src/components/perf_event_uncore/perf_event_uncore.c

include $(top_srcdir)/src/components/perf_event_uncore/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

AM_CPPFLAGS += -I$(top_srcdir)/src -I$(top_builddir)/src

include $(top_srcdir)/src/high-level/Makefile.mk
include $(top_srcdir)/src/components/Makefile.mk
include $(top_srcdir)/src/counter_analysis_toolkit/Makefile.mk
include $(top_srcdir)/src/ctests/Makefile.mk
include $(top_srcdir)/src/ftests/Makefile.mk
include $(top_srcdir)/src/utils/Makefile.mk
include $(top_srcdir)/src/testlib/Makefile.mk
include $(top_srcdir)/src/x86/Makefile.mk
include $(top_srcdir)/src/linux/Makefile.mk
include $(top_srcdir)/src/solaris/Makefile.mk
include $(top_srcdir)/src/freebsd/Makefile.mk
include $(top_srcdir)/src/darwin/Makefile.mk
include $(top_srcdir)/src/aix/Makefile.mk
include $(top_srcdir)/src/bgp/Makefile.mk
include $(top_srcdir)/src/bgq/Makefile.mk

include_HEADERS += src/papi.h

noinst_HEADERS += src/cpus.h                 \
                  src/components_config.h    \
                  src/extras.h               \
                  src/mb.h                   \
                  src/papi_bipartite.h       \
                  src/papi_common_strings.h  \
                  src/papi_debug.h           \
                  src/papi_internal.h        \
                  src/papi_libpfm4_events.h  \
                  src/papi_lock.h            \
                  src/papi_memory.h          \
                  src/papi_preset.h          \
                  src/papi_vector.h          \
                  src/papiStdEventDefs.h     \
                  src/papivi.h               \
                  src/sw_multiplex.h         \
                  src/threads.h

papi_sources   += src/cpus.c                 \
                  src/extras.c               \
                  src/papi.c                 \
                  src/papi_fwrappers_.c      \
                  src/papi_fwrappers__.c     \
                  src/upper_PAPI_FWRAPPERS.c \
                  src/papi_internal.c        \
                  src/papi_libpfm4_events.c  \
                  src/papi_memory.c          \
                  src/papi_vector.c          \
                  src/sw_multiplex.c         \
                  src/cpu_component.c        \
                  src/papi_preset.c          \
                  src/threads.c

# This is silly but whatever ...
src/papi_fwrappers_.c: src/papi_fwrappers.c
	$(CC) -E $(AM_CPPFLAGS) -DFORTRANUNDERSCORE $< -o $@

src/papi_fwrappers__.c: src/papi_fwrappers.c
	$(CC) -E $(AM_CPPFLAGS) -DFORTRANDOUBLEUNDERSCORE $< -o $@

src/upper_PAPI_FWRAPPERS.c: src/papi_fwrappers.c
	$(CC) -E $(AM_CPPFLAGS) -DFORTRANCAPS $< -o $@

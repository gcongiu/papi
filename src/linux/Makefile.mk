##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_LINUX

AM_CPPFLAGS += -I$(top_srcdir)/src/linux

noinst_HEADERS += src/linux/linux-common.h     \
                  src/linux/linux-context.h    \
                  src/linux/linux-generic.h    \
                  src/linux/linux-memory.h     \
                  src/linux/linux-timer.h

papi_sources   += src/linux/linux-common.c     \
                  src/linux/linux-generic.c    \
                  src/linux/linux-memory.c     \
                  src/linux/linux-timer.c

endif

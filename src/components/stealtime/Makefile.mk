##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_STEALTIME

papi_sources += linux-stealtime.c

include $(top_srcdir)/src/components/stealtime/tests/Makefile.mk

endif

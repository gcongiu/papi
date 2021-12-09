##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##


if BUILD_APPIO

noinst_HEADERS += src/components/appio/appio.h
papi_sources   += src/components/appio/appio.c

include $(top_srcdir)/src/components/appio/tests/Makefile.mk

endif

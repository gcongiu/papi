##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##


if BUILD_APPIO

noinst_HEADERS += appio.h
papi_sources   += appio.c

include $(top_srcdir)/src/components/appio/tests/Makefile.mk

endif

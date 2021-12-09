##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_CORETEMP

noinst_HEADERS += linux-coretemp.h
papi_sources   += linux-coretemp.c

include $(top_srcdir)/src/components/coretemp/tests/Makefile.mk

endif

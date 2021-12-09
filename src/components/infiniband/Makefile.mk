##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_INFINIBAND

noinst_HEADERS += pscanf.h
papi_sources   += linux-infiniband.c

include $(top_srcdir)/src/components/infiniband/tests/Makefile.mk

endif

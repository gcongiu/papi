##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_INFINIBAND

noinst_HEADERS += src/components/infiniband/pscanf.h
papi_sources   += src/components/infiniband/linux-infiniband.c

include $(top_srcdir)/src/components/infiniband/tests/Makefile.mk

endif

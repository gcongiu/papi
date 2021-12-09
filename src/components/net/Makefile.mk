##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_NET

noinst_HEADERS += src/components/net/linux-net.h
papi_sources   += src/components/net/linux-net.c

include $(top_srcdir)/src/components/net/tests/Makefile.mk

endif

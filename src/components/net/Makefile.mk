##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_NET

noinst_HEADERS += linux-net.h
papi_sources   += linux-net.c

include $(top_srcdir)/src/components/net/tests/Makefile.mk

endif

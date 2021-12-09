##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_EXAMPLE

noinst_HEADERS += src/components/example/example.h
papi_sources   += src/components/example/example.c

include $(top_srcdir)/src/components/example/tests/Makefile.mk

endif

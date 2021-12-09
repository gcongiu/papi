##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_RAPL

papi_sources += src/components/rapl/linux-rapl.c

include $(top_srcdir)/src/components/rapl/tests/Makefile.mk
endif

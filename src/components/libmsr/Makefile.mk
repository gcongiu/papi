##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_LIBMSR

papi_sources += linux-libmsr.c

include $(top_srcdir)/src/components/libmsr/tests/Makefile.mk

endif

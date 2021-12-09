##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_LIBMSR

papi_sources += src/components/libmsr/linux-libmsr.c

include $(top_srcdir)/src/components/libmsr/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_IO

papi_sources += linux-io.c

include $(top_srcdir)/src/components/io/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_POWERCAP

papi_sources += linux-powercap.c

include $(top_srcdir)/src/components/powercap/tests/Makefile.mk

endif

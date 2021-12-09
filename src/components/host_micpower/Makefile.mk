##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_HOST_MICPOWER

papi_sources += src/components/host_micpower/linux-host_micpower.c

include $(top_srcdir)/src/components/host_micpower/tests/Makefile.mk

endif

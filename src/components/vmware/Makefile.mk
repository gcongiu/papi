##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_VMWARE

papi_sources += vmware.c

include $(top_srcdir)/src/components/vmware/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_ROCM

papi_sources += linux-rocm.c

include $(top_srcdir)/src/components/rocm/tests/Makefile.mk

endif

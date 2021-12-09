##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_ROCM_SMI

papi_sources += linux-rocm-smi.c

include $(top_srcdir)/src/components/rocm_smi/tests/Makefile.mk

endif

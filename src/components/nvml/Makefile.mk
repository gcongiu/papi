##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_NVML

noinst_HEADERS += src/components/nvml/linux-nvml.h
papi_sources   += src/components/nvml/linux-nvml.c

include $(top_srcdir)/src/components/tests/Makefile.mk

endif

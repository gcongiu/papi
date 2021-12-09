##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_INTEL_GPU

AM_LDFLAGS += -lstdc++

noinst_HEADERS += linux_intel_gpu_metrics.h
papi_sources   += linux_intel_gpu_metrics.c

include $(top_srcdir)/src/components/intel_gpu/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##


if BUILD_CUDA

AM_CPPFLAGS += $(CUDA_CPPFLAGS)

papi_sources += src/components/cuda/linux-cuda.c

include $(top_srcdir)/src/components/cuda/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_MICPOWER

noinst_HEADERS += src/components/micpower/linux-micpower.h
papi_sources   += src/components/micpower/linux-micpower.c

include $(top_srcdir)/src/components/micpower/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_SENSORS_PPC

noinst_HEADERS += linux-sensors-ppc.h
papi_sources   += linux-sensors-ppc.c

include $(top_srcdir)/src/components/sensors_ppc/tests/Makefile.mk

endif

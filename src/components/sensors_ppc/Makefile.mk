##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_SENSORS_PPC

noinst_HEADERS += src/components/sensors_ppc/linux-sensors-ppc.h
papi_sources   += src/components/sensors_ppc/linux-sensors-ppc.c

include $(top_srcdir)/src/components/sensors_ppc/tests/Makefile.mk

endif

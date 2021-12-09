##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_POWERCAP_PPC

noinst_HEADERS += src/components/powercap_ppc/linux-powercap-ppc.h
papi_sources   += src/components/powercap_ppc/linux-powercap-ppc.c

include $(top_srcdir)/src/components/powercap_ppc/tests/Makefile.mk

endif

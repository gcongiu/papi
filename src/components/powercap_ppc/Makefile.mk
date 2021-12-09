##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_POWERCAP_PPC

noinst_HEADERS += linux-powercap-ppc.h
papi_sources   += linux-powercap-ppc.c

include $(top_srcdir)/src/components/powercap_ppc/tests/Makefile.mk

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_LUSTRE

papi_sources += src/components/lustre/linux-lustre.c

include $(top_srcdir)/src/components/lustre/tests/Makefile.mk

endif

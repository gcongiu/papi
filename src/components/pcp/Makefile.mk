##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_PCP

papi_sources += linux-pcp.c

include $(top_srcdir)/src/components/pcp/tests/Makefile.mk

endif

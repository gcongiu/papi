##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_BGPM

include $(top_srcdir)/src/components/bgpm/CNKunit/Makefile.mk
include $(top_srcdir)/src/components/bgpm/IOunit/Makefile.mk
include $(top_srcdir)/src/components/bgpm/L2unit/Makefile.mk
include $(top_srcdir)/src/components/bgpm/NWunit/Makefile.mk

endif

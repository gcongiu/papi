##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_X86

AM_CPPFLAGS += -I$(top_srcdir)/src/x86

noinst_HEADERS += src/x86/x86_cpuid_info.h
papi_sources   += src/x86/x86_cpuid_info.c

endif

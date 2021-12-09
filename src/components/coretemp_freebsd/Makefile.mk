##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_CORETEMP_FREEBSD

noinst_HEADERS += src/components/coretemp_freebsd/coretemp_freebsd.h
papi_sources   += src/components/coretemp_freebsd/coretemp_freebsd.c

endif

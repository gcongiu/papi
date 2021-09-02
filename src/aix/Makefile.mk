##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_AIX

noinst_HEADERS += aix-context.h \
                  aix-lock.h    \
                  aix.h

papi_sources   += aix-memory.c \
                  aix.c

endif

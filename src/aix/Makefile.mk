##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_AIX

noinst_HEADERS += src/aix/aix-context.h \
                  src/aix/aix-lock.h    \
                  src/aix/aix.h

papi_sources   += src/aix/aix-memory.c \
                  src/aix/aix.c

endif

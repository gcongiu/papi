##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_DARWIN

noinst_HEADERS += darwin-common.h  \
                  darwin-context.h \
                  darwin-lock.h    \
                  darwin-memory.h

papi_sources   += darwin-common.c  \
                  darwin-memory.c

endif

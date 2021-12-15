##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_DARWIN

noinst_HEADERS += src/darwin/darwin-common.h  \
                  src/darwin/darwin-context.h \
                  src/darwin/darwin-lock.h    \
                  src/darwin/darwin-memory.h

papi_sources   += src/darwin/darwin-common.c  \
                  src/darwin/darwin-memory.c

endif

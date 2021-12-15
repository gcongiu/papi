##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_SOLARIS

noinst_HEADERS += src/solaris/solaris-common.h   \
                  src/solaris/solaris-context.h  \
                  src/solaris/solaris-lock.h     \
                  src/solaris/solaris-memory.h   \
                  src/solaris/solaris-niagara2.h \
                  src/solaris/solaris-ultra.h

papi_sources   += src/solaris/solaris-common.c   \
                  src/solaris/solaris-memory.c   \
                  src/solaris/solaris-niagara2.c \
                  src/solaris/solaris-ultra.c

endif

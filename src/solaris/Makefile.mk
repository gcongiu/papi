##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_SOLARIS

noinst_HEADERS += solaris-common.h   \
                  solaris-context.h  \
                  solaris-lock.h     \
                  solaris-memory.h   \
                  solaris-niagara2.h \
                  solaris-ultra.h

papi_sources   += solaris-common.c   \
                  solaris-memory.c   \
                  solaris-niagara2.c \
                  solaris-ultra.c

endif

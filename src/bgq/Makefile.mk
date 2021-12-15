##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_BGQ

noinst_HEADERS += src/bgq/linux-bgq-common.h \
                  src/bgq/linux-bgq.h

papi_sources   += src/bgq/linux-bgq-common.c \
                  src/bgq/linux-bgq-memory.c \
                  src/bgq/linux-bgq.c

endif

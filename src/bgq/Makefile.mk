##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_BGQ

noinst_HEADERS += linux-bgq-common.h \
                  linux-bgq.h

papi_sources   += linux-bgq-common.c \
                  linux-bgq-memory.c \
                  linux-bgq.c

endif

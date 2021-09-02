##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_BGP

noinst_HEADERS += linux-bgp-native-events.h \
                  linux-bgp.h

papi_sources += linux-bgp-memory.c \
                linux-bgp.c

endif

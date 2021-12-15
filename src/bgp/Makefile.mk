##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_BGP

noinst_HEADERS += src/bgp/linux-bgp-native-events.h \
                  src/bgp/linux-bgp.h

papi_sources += src/bgp/linux-bgp-memory.c \
                src/bgp/linux-bgp.c

endif

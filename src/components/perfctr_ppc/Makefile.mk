##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_PERFCTR_PPC

noinst_HEADERS += linux-ppc64.h    \
                  perfctr-ppc64.h  \
                  ppc64_events.h

papi_sources   += perfctr-ppc64.c  \
                  ppc64_events.c

endif

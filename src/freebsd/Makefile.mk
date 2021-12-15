##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_FREEBSD

noinst_HEADERS += src/freebsd/freebsd-config.h       \
                  src/freebsd/freebsd-context.h      \
                  src/freebsd/freebsd-lock.h         \
                  src/freebsd/freebsd-memory.h       \
                  src/freebsd/map-atom.h             \
                  src/freebsd/map-core.h             \
                  src/freebsd/map-core2-extreme.h    \
                  src/freebsd/map-core2.h            \
                  src/freebsd/map-i7.h               \
                  src/freebsd/map-k7.h               \
                  src/freebsd/map-k8.h               \
                  src/freebsd/map-p4.h               \
                  src/freebsd/map-p6-2.h             \
                  src/freebsd/map-p6-3.h             \
                  src/freebsd/map-p6-c.h             \
                  src/freebsd/map-p6-m.h             \
                  src/freebsd/map-p6.h               \
                  src/freebsd/map-unknown.h          \
                  src/freebsd/map-westmere.h         \
                  src/freebsd/map.h

papi_sources   += src/freebsd/freebsd-memory.c       \
                  src/freebsd/freebsd.c              \
                  src/freebsd/map-atom.c             \
                  src/freebsd/map-core.c             \
                  src/freebsd/map-core2-extreme.c    \
                  src/freebsd/map-core2.c            \
                  src/freebsd/map-i7.c               \
                  src/freebsd/map-k7.c               \
                  src/freebsd/map-k8.c               \
                  src/freebsd/map-p4.c               \
                  src/freebsd/map-p6-2.c             \
                  src/freebsd/map-p6-3.c             \
                  src/freebsd/map-p6-c.c             \
                  src/freebsd/map-p6-m.c             \
                  src/freebsd/map-unknown.c          \
                  src/freebsd/map-westmere.c         \
                  src/freebsd/map.c

endif

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

if BUILD_FREEBSD

noinst_HEADERS += freebsd-config.h       \
                  freebsd-context.h      \
                  freebsd-lock.h         \
                  freebsd-memory.h       \
                  map-atom.h             \
                  map-core.h             \
                  map-core2-extreme.h    \
                  map-core2.h            \
                  map-i7.h               \
                  map-k7.h               \
                  map-k8.h               \
                  map-p4.h               \
                  map-p6-2.h             \
                  map-p6-3.h             \
                  map-p6-c.h             \
                  map-p6-m.h             \
                  map-p6.h               \
                  map-unknown.h          \
                  map-westmere.h         \
                  map.h

papi_sources   += freebsd-memory.c       \
                  freebsd.c              \
                  map-atom.c             \
                  map-core.c             \
                  map-core2-extreme.c    \
                  map-core2.c            \
                  map-i7.c               \
                  map-k7.c               \
                  map-k8.c               \
                  map-p4.c               \
                  map-p6-2.c             \
                  map-p6-3.c             \
                  map-p6-c.c             \
                  map-p6-m.c             \
                  map-unknown.c          \
                  map-westmere.c         \
                  map.c

endif

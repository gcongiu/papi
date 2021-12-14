##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += fortran_programs

fortran_programs =

if BUILD_FTESTS
fortran_programs += src/ftests/strtest           \
                    src/ftests/zero              \
                    src/ftests/zeronamed         \
                    src/ftests/first             \
                    src/ftests/second            \
                    src/ftests/tenth             \
                    src/ftests/description       \
                    src/ftests/fdmemtest         \
                    src/ftests/accum             \
                    src/ftests/cost              \
                    src/ftests/case1             \
                    src/ftests/case2             \
                    src/ftests/clockres          \
                    src/ftests/eventname         \
                    src/ftests/fmatrixlowpapi    \
                    src/ftests/fmultiplex1       \
                    src/ftests/johnmay2          \
                    src/ftests/fmultiplex2       \
                    src/ftests/avail             \
                    src/ftests/openmp            \
                    src/ftests/serial_hl
endif

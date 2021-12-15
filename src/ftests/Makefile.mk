##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += $(fortran_programs)

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

src_ftests_strtest_SOURCES =                     \
                    src/ftests/strtest.F
src_ftests_zero_SOURCES =                        \
                    src/ftests/zero.F
src_ftests_zeronamed_SOURCES =                   \
                    src/ftests/zeronamed.F
src_ftests_first_SOURCES =                       \
                    src/ftests/first.F
src_ftests_second_SOURCES =                      \
                    src/ftests/second.F
src_ftests_tenth_SOURCES =                       \
                    src/ftests/tenth.F
src_ftests_description_SOURCES =                 \
                    src/ftests/description.F
src_ftests_fdmemtest_SOURCES =                   \
                    src/ftests/fdmemtest.F
src_ftests_accum_SOURCES =                       \
                    src/ftests/accum.F
src_ftests_cost_SOURCES =                        \
                    src/ftests/cost.F
src_ftests_case1_SOURCES =                       \
                    src/ftests/case1.F
src_ftests_case2_SOURCES =                       \
                    src/ftests/case2.F
src_ftests_clockres_SOURCES =                    \
                    src/ftests/clockres.F
src_ftests_eventname_SOURCES =                   \
                    src/ftests/eventname.F
src_ftests_fmatrixlowpapi_SOURCES =              \
                    src/ftests/fmatrixlowpapi.F
src_ftests_fmultiplex1_SOURCES =                 \
                    src/ftests/fmultiplex1.F
src_ftests_johnmay2_SOURCES =                    \
                    src/ftests/johnmay2.F
src_ftests_fmultiplex2_SOURCES =                 \
                    src/ftests/fmultiplex2.F
src_ftests_avail_SOURCES =                       \
                    src/ftests/avail.F
src_ftests_openmp_SOURCES =                      \
                    src/ftests/openmp.F
src_ftests_serial_hl_SOURCES =                   \
                    src/ftests/serial_hl.F

endif

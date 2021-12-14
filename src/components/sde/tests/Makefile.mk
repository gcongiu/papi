##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/sde/tests/Minimal/Minimal_Test                 \
                  src/components/sde/tests/Simple/Simple_Test                   \
                  src/components/sde/tests/Simple2/Simple2_Test                 \
                  src/components/sde/tests/Simple2/Simple2_NoPAPI_Test          \
                  src/components/sde/tests/Recorder/Recorder_Test               \
                  src/components/sde/tests/Created_Counter/Created_Counter_Test \
                  src/components/sde/tests/Created_Counter/Overflow_Test        \
                  src/components/sde/tests/Advanced_C+Fortran/sde_test_f08

src_components_sde_tests_Simple_Simple_Test_SOURCES = src/components/sde/tests/Simple/Simple_Lib.c  \
                                                      src/components/sde/tests/Simple/Simple_Driver.c

src_components_sde_tests_Simple2_Simple2_Test_SOURCES = src/components/sde/tests/Simple2/Simple2_Lib.c  \
                                                        src/components/sde/tests/Simple2/Simple2_Driver.c

src_components_sde_tests_Simple2_Simple2_NoPAPI_Test_SOURCES = src/components/sde/tests/Simple2/Simple2_Lib.c   \
                                                               src/components/sde/tests/Simple2/Simple2_NoPAPI_Driver.c

src_components_sde_tests_Recorder_Recorder_Test_SOURCES = src/components/sde/tests/Recorder/Lib_With_Recorder.c     \
                                                          src/components/sde/tests/Recorder/Recorder_Driver.c

src_components_sde_tests_Created_Counter_Created_Counter_Test_SOURCES = src/components/sde/tests/Created_Counter/Lib_With_Created_Counter.c     \
                                                                        src/components/sde/tests/Created_Counter/Created_Counter_Driver.c

src_components_sde_tests_Created_Counter_Overflow_Test_SOURCES = src/components/sde/tests/Created_Counter/Lib_With_Created_Counter.c    \
                                                                 src/components/sde/tests/Created_Counter/Overflow_Driver.c

src_components_sde_tests_Advanced_C_Fortran_sde_test_f08_SOURCES = src/components/sde/tests/Advanced_C+Fortran/sde_test_f08.F90     \
                                                                   src/components/sde/tests/Advanced_C+Fortran/sde_symbols.c        \
                                                                   src/components/sde/tests/Advanced_C+Fortran/Xandria.F90          \
                                                                   src/components/sde/tests/Advanced_C+Fortran/Gamum.c              \
                                                                   src/components/sde/tests/Recorder/Lib_With_Recorder.c


##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/sde/tests/Minimal_Test             \
                  src/components/sde/tests/Simple_Test              \
                  src/components/sde/tests/Simple2_Test             \
                  src/components/sde/tests/Simple2_NoPAPI_Test      \
                  src/components/sde/tests/Recorder_Test            \
                  src/components/sde/tests/Created_Counter_Test     \
                  src/components/sde/tests/Overflow_Test            \
                  src/components/sde/tests/sde_test_f08

src_components_sde_tests_Simple_Test_SOURCES =                      \
                  src/components/sde/tests/Simple_Lib.c             \
                  src/components/sde/tests/Simple_Driver.c

src_components_sde_tests_Simple2_Test_SOURCES =                     \
                  src/components/sde/tests/Simple2_Lib.c            \
                  src/components/sde/tests/Simple2_Driver.c

src_components_sde_tests_Simple2_NoPAPI_Test_SOURCES =              \
                  src/components/sde/tests/Simple2_Lib.c            \
                  src/components/sde/tests/Simple2_NoPAPI_Driver.c

src_components_sde_tests_Recorder_Test_SOURCES =                    \
                  src/components/sde/tests/Lib_With_Recorder.c      \
                  src/components/sde/tests/Recorder_Driver.c

src_components_sde_tests_Created_Counter_Test_SOURCES =             \
                  src/components/sde/tests/Lib_With_Created_Counter.c   \
                  src/components/sde/tests/Created_Counter_Driver.c

src_components_sde_tests_Overflow_Test_SOURCES =                    \
                  src/components/sde/tests/Lib_With_Created_Counter.c   \
                  src/components/sde/tests/Overflow_Driver.c

src_components_sde_tests_sde_test_f08_SOURCES =                     \
                  src/components/sde/tests/Xandria.F90              \
                  src/components/sde/tests/sde_symbols.c            \
                  src/components/sde/tests/Gamum.c                  \
                  src/components/sde/tests/sde_test_f08.F90

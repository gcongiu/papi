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

src_components_sde_tests_Simple_Test_LDADD =                        \
                  src/components/sde/tests/Simple_Lib.o             \
                  src/components/sde/tests/Simple_Driver.o

src_components_sde_tests_Simple2_Test_LDADD =                       \
                  src/components/sde/tests/Simple2_Lib.o            \
                  src/components/sde/tests/Simple2_Driver.o

src_components_sde_tests_Simple2_NoPAPI_Test_LDADD =                \
                  src/components/sde/tests/Simple2_Lib.o            \
                  src/components/sde/tests/Simple2_NoPAPI_Driver.o

src_components_sde_tests_Recorder_Test_LDADD =                      \
                  src/components/sde/tests/Lib_With_Recorder.o      \
                  src/components/sde/tests/Recorder_Driver.o

src_components_sde_tests_Created_Counter_Test_LDADD =                   \
                  src/components/sde/tests/Lib_With_Created_Counter.o   \
                  src/components/sde/tests/Created_Counter_Driver.o

src_components_sde_tests_Overflow_Test_LDADD =                          \
                  src/components/sde/tests/Lib_With_Created_Counter.o   \
                  src/components/sde/tests/Overflow_Driver.o

src_components_sde_tests_sde_test_f08_LDADD =                       \
                  src/components/sde/tests/Xandria.o                \
                  src/components/sde/tests/sde_symbols.o            \
                  src/components/sde/tests/Gamum.o                  \
                  src/components/sde/tests/sde_test_f08.o

##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/rocm/tests/rocm_command_line   \
                  src/components/rocm/tests/rocm_standalone     \
                  src/components/rocm/tests/rocm_example        \
                  src/components/rocm/tests/rocm_all

src_components_rocm_tests_rocm_standalone_SOURCES =             \
                  src/components/rocm/tests/rocm_standalone.cpp

src_components_rocm_tests_rocm_example_SOURCES =                \
                  src/components/rocm/tests/rocm_example.cpp

src_components_rocm_tests_rocm_all_SOURCES =                    \
                  src/components/rocm/tests/rocm_all.cpp

.cpp.lo:

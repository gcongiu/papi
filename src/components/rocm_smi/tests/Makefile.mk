##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

testlist       += src/components/rocm_smi/tests/rocm_command_line       \
                  src/components/rocm_smi/tests/rocm_smi_all            \
                  src/components/rocm_smi/tests/rocm_smi_writeTests     \
                  src/components/rocm_smi/tests/power_monitor_rocm      \
                  src/components/rocm_smi/tests/rocmsmi_example

src_components_rocm_smi_tests_rocm_command_line_SOURCES =               \
                  src/components/rocm_smi/tests/rocm_command_line.cpp

src_components_rocm_smi_tests_rocm_smi_all_SOURCES =                    \
                  src/components/rocm_smi/tests/rocm_smi_all.cpp

src_components_rocm_smi_tests_rocm_smi_writeTests_SOURCES =             \
                  src/components/rocm_smi/tests/rocm_smi_writeTests.cpp

src_components_rocm_smi_tests_power_monitor_rocm_SOURCES =              \
                  src/components/rocm_smi/tests/power_monitor_rocm.cpp

src_components_rocm_smi_tests_rocmsmi_example_SOURCES =                 \
                  src/components/rocm_smi/tests/rocmsmi_example.cpp

.cpp.lo:

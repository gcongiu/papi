##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/nvml/tests/HelloWorld                  \
                  src/components/nvml/tests/nvml_power_limiting_test    \
                  src/components/nvml/tests/nvml_power_limit_read_test  \
                  src/components/nvml/tests/benchSANVML

src_components_nvml_tests_HelloWorld_SOURCES =                          \
                  src/components/nvml/tests/HelloWorld.cu

src_components_nvml_tests_nvml_power_limit_read_test_SOURCES =          \
                  src/components/nvml/tests/nvml_power_limit_read_test.cu

src_components_nvml_tests_nvml_power_limiting_test_SOURCES =            \
                  src/components/nvml/tests/nvml_power_limiting_test.cu

#TODO: add libtool recipes for NVCC
.cu.lo:

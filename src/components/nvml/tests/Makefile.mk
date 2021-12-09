##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/nvml/tests/HelloWorld                  \
                  src/components/nvml/tests/nvml_power_limiting_test    \
                  src/components/nvml/tests/nvml_power_limit_read_test  \
                  src/components/nvml/tests/benchSANVML

#TODO: add libtool recipes for NVCC
.cu.lo:

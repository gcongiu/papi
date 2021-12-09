##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/rocm_smi/tests/rocm_command_line       \
                  src/components/rocm_smi/tests/rocm_smi_all            \
                  src/components/rocm_smi/tests/rocm_smi_writeTests     \
                  src/components/rocm_smi/tests/power_monitor_rocm      \
                  src/components/rocm_smi/tests/rocmsmi_example         \
                  src/components/rocm_smi/tests/square

.cu.lo:

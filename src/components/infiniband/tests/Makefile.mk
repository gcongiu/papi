##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/components/infiniband/tests/infiniband_list_events    \
                  src/components/infiniband/tests/infiniband_values_by_code

if BUILD_MPITESTS
EXTRA_PROGRAMS += src/components/infiniband/tests/MPI_test_infiniband_events
endif

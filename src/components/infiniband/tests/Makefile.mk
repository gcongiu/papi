##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

testlist       += src/components/infiniband/tests/infiniband_list_events    \
                  src/components/infiniband/tests/infiniband_values_by_code

if BUILD_MPITESTS
testlist       += src/components/infiniband/tests/MPI_test_infiniband_events
endif

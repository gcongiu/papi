##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += src/validation_tests/fp_validation_hl     \
                  src/validation_tests/cycles_validation    \
                  src/validation_tests/flops_validation     \
                  src/validation_tests/memleak_check        \
                  src/validation_tests/papi_br_cn           \
                  src/validation_tests/papi_br_ins          \
                  src/validation_tests/papi_br_msp          \
                  src/validation_tests/papi_br_ntk          \
                  src/validation_tests/papi_br_prc          \
                  src/validation_tests/papi_br_tkn          \
                  src/validation_tests/papi_br_ucn          \
                  src/validation_tests/papi_dp_ops          \
                  src/validation_tests/papi_fp_ops          \
                  src/validation_tests/papi_sp_ops          \
                  src/validation_tests/papi_hw_int          \
                  src/validation_tests/papi_l1_dca          \
                  src/validation_tests/papi_l1_dcm          \
                  src/validation_tests/papi_l2_dca          \
                  src/validation_tests/papi_l2_dcm          \
                  src/validation_tests/papi_l2_dcr          \
                  src/validation_tests/papi_l2_dcw          \
                  src/validation_tests/papi_ld_ins          \
                  src/validation_tests/papi_sr_ins          \
                  src/validation_tests/papi_ref_cyc         \
                  src/validation_tests/papi_tot_cyc         \
                  src/validation_tests/papi_tot_ins

src_validation_tests_fp_validation_hl_LDADD = src/validation_tests/flops_testcode.o

src_validation_tests_cycles_validation_LDADD = src/validation_tests/instructions_testcode.o \
                                               src/validation_tests/display_error.o

src_validation_tests_flops_validation_LDADD = src/validation_tests/branches_testcode.o  \
                                              src/validation_tests/flops_testcode.o

src_validation_tests_memleak_check_LDADD = src/validation_tests/branches_testcode.o     \
                                           src/validation_tests/display_error.o

src_validation_tests_papi_br_cn_LDADD = src/validation_tests/display_error.o    \
                                        src/validation_tests/branches_testcode.o

src_validation_tests_papi_br_ins_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/branches_testcode.o

src_validation_tests_papi_br_msp_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/branches_testcode.o

src_validation_tests_papi_br_ntk_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/branches_testcode.o

src_validation_tests_papi_br_prc_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/branches_testcode.o

src_validation_tests_papi_br_tkn_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/branches_testcode.o

src_validation_tests_papi_br_ucn_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/branches_testcode.o

src_validation_tests_papi_dp_ops_LDADD = src/validation_tests/display_error.o       \
                                         src/validation_tests/branches_testcode.o   \
                                         src/validation_tests/flops_testcode.o

src_validation_tests_papi_fp_ops_LDADD = src/validation_tests/display_error.o       \
                                         src/validation_tests/branches_testcode.o   \
                                         src/validation_tests/flops_testcode.o

src_validation_tests_papi_ld_ins_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o

src_validation_tests_papi_l1_dca_LDADD = src/validation_tests/display_error.o       \
                                         src/validation_tests/matrix_multiply.o     \
                                         src/validation_tests/cache_testcode.o

src_validation_tests_papi_l1_dcm_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o \
                                         src/validation_tests/cache_testcode.o  \
                                         src/validation_tests/cache_helper.o

src_validation_tests_papi_l2_dca_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o \
                                         src/validation_tests/cache_testcode.o  \
                                         src/validation_tests/cache_helper.o

src_validation_tests_papi_l2_dcm_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o \
                                         src/validation_tests/cache_testcode.o  \
                                         src/validation_tests/cache_helper.o

src_validation_tests_papi_l2_dcr_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o \
                                         src/validation_tests/cache_testcode.o  \
                                         src/validation_tests/cache_helper.o

src_validation_tests_papi_l2_dcw_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o \
                                         src/validation_tests/cache_testcode.o  \
                                         src/validation_tests/cache_helper.o

src_validation_tests_papi_ref_cyc_LDADD = src/validation_tests/display_error.o  \
                                          src/validation_tests/flops_testcode.o

src_validation_tests_papi_sp_ops_LDADD = src/validation_tests/display_error.o       \
                                         src/validation_tests/branches_testcode.o   \
                                         src/validation_tests/flops_testcode.o

src_validation_tests_papi_sr_ins_LDADD = src/validation_tests/display_error.o   \
                                         src/validation_tests/matrix_multiply.o

src_validation_tests_papi_tot_cyc_LDADD = src/validation_tests/display_error.o  \
                                          src/validation_tests/matrix_multiply.o

src_validation_tests_papi_tot_ins_LDADD = src/validation_tests/display_error.o  \
                                          src/validation_tests/instructions_testcode.o

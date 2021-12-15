##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

EXTRA_PROGRAMS += $(serial_programs)    \
                  $(forkexec_programs)  \
                  $(overflow_programs)  \
                  $(profile_programs)   \
                  $(attach_programs)    \
                  $(broken_programs)    \
                  $(p4_test_programs)   \
                  $(ear_programs)       \
                  $(range_programs)     \
                  $(omp_programs)       \
                  $(smp_programs)       \
                  $(shmem_programs)     \
                  $(pthread_programs)   \
                  $(mpx_programs)       \
                  $(mpx_pthr_programs)  \
                  $(mpi_programs)

serial_programs =
forkexec_programs =
overflow_programs =
profile_programs =
attach_programs =
broken_programs =
p4_test_programs =
ear_programs =
range_programs =
omp_programs =
smp_programs =
shmem_programs =
pthread_programs =
mpx_programs =
mpx_pthr_programs =
mpi_programs =

if BUILD_CTESTS
serial_programs += src/ctests/serial_hl             \
                   src/ctests/serial_hl_ll_comb     \
                   src/ctests/all_events            \
                   src/ctests/all_native_events     \
                   src/ctests/branches              \
                   src/ctests/calibrate             \
                   src/ctests/case1                 \
                   src/ctests/case2                 \
                   src/ctests/cmpinfo               \
                   src/ctests/code2name             \
                   src/ctests/derived               \
                   src/ctests/describe              \
                   src/ctests/destroy               \
                   src/ctests/disable_component     \
                   src/ctests/dmem_info             \
                   src/ctests/eventname             \
                   src/ctests/exeinfo               \
                   src/ctests/failed_events         \
                   src/ctests/first                 \
                   src/ctests/get_event_component   \
                   src/ctests/inherit               \
                   src/ctests/hwinfo                \
                   src/ctests/johnmay2              \
                   src/ctests/low-level             \
                   src/ctests/memory                \
                   src/ctests/realtime              \
                   src/ctests/remove_events         \
                   src/ctests/reset                 \
                   src/ctests/second                \
                   src/ctests/tenth                 \
                   src/ctests/version               \
                   src/ctests/virttime              \
                   src/ctests/zero                  \
                   src/ctests/zero_flip             \
                   src/ctests/zero_named

forkexec_programs += src/ctests/fork                    \
                     src/ctests/fork2                   \
                     src/ctests/exec                    \
                     src/ctests/exec2                   \
                     src/ctests/forkexec                \
                     src/ctests/forkexec2               \
                     src/ctests/forkexec3               \
                     src/ctests/forkexec4               \
                     src/ctests/fork_overflow           \
                     src/ctests/exec_overflow           \
                     src/ctests/child_overflow          \
                     src/ctests/system_child_overflow   \
                     src/ctests/system_overflow         \
                     src/ctests/burn                    \
                     src/ctests/zero_fork

overflow_programs += src/ctests/overflow                \
                     src/ctests/overflow_force_software \
                     src/ctests/overflow_single_event   \
                     src/ctests/overflow_twoevents      \
                     src/ctests/timer_overflow          \
                     src/ctests/overflow2               \
                     src/ctests/overflow_index          \
                     src/ctests/overflow_one_and_read   \
                     src/ctests/overflow_allcounters

profile_programs += src/ctests/profile                  \
                    src/ctests/profile_force_software   \
                    src/ctests/sprofile                 \
                    src/ctests/profile_twoevents        \
                    src/ctests/byte_profile

src_ctests_profile_LDADD =                              \
                    src/ctests/profile_utils.o
src_ctests_profile_force_software_SOURCES =             \
                    src/ctests/profile.c
src_ctests_profile_force_software_CPPFLAGS =            \
                    -DSWPROFILE
src_ctests_profile_force_software_LDADD =               \
                    src/ctests/profile_utils.o
src_ctests_sprofile_LDADD =                             \
                    src/ctests/profile_utils.o
src_ctests_profile_twoevents_LDADD =                    \
                    src/ctests/profile_utils.o
src_ctests_byte_profile_LDADD =                         \
                    src/ctests/profile_utils.o

attach_programs += src/ctests/multiattach               \
                   src/ctests/multiattach2              \
                   src/ctests/zero_attach               \
                   src/ctests/attach3                   \
                   src/ctests/attach2                   \
                   src/ctests/attach_target             \
                   src/ctests/attach_cpu                \
                   src/ctests/attach_validate           \
                   src/ctests/attach_cpu_validate       \
                   src/ctests/attach_cpu_sys_validate

p4_test_programs += src/ctests/p4_lst_ins

ear_programs += src/ctests/earprofile

range_programs += src/ctests/data_range

broken_programs += src/ctests/pernode   \
                   src/ctests/val_omp

omp_programs += src/ctests/omp_hl    \
                src/ctests/zero_omp  \
                src/ctests/omptough

smp_programs += src/ctests/zero_smp

shmem_programs += src/ctests/zero_shmem

pthread_programs += src/ctests/pthread_hl           \
                    src/ctests/pthrtough            \
                    src/ctests/pthrtough2           \
                    src/ctests/thrspecific          \
                    src/ctests/profile_pthreads     \
                    src/ctests/overflow_pthreads    \
                    src/ctests/zero_pthreads        \
                    src/ctests/clockres_pthreads    \
                    src/ctests/overflow3_pthreads   \
                    src/ctests/locks_pthreads       \
                    src/ctests/krentel_pthreads

mpx_programs += src/ctests/max_multiplex     \
                src/ctests/multiplex1        \
                src/ctests/multiplex2        \
                src/ctests/mendes-alt        \
                src/ctests/sdsc2             \
                src/ctests/sdsc2-mpx         \
                src/ctests/sdsc2-mpx-noreset \
                src/ctests/sdsc-mpx          \
                src/ctests/sdsc4-mpx         \
                src/ctests/reset_multiplex

src_ctests_sdsc2_mpx_SOURCES =               \
                src/ctests/sdsc2.c
src_ctests_sdsc2_mpx_CPPFLAGS =              \
                -DMPX
src_ctests_sdsc2_mpx_noreset_SOURCES =       \
                src/ctests/sdsc2.c
src_ctests_sdsc2_mpx_noreset_CPPFLAGS =      \
                -DMPX -DSTARTSTOP
src_ctests_sdsc_mpx_CPPFLAGS =               \
                -DMPX
src_ctests_sdsc4_mpx_CPPFLAGS =              \
                -DMPX

mpx_pthr_programs += src/ctests/multiplex1_pthreads  \
                     src/ctests/multiplex3_pthreads  \
                     src/ctests/kufrin
endif

if BUILD_MPITESTS
mpi_programs += src/ctests/mpi_hl        \
                src/ctests/mpi_omp_hl    \
                src/ctests/mpifirst
endif

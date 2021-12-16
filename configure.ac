##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

m4_include([maint/version.m4])

AC_PREREQ([2.69])
AC_INIT([PAPI],
        PAPI_VERSION_m4,
        [ptools-perfapi@icl.utk.edu],
        [papi],
        [http://icl.utk.edu/papi])

AC_CONFIG_SRCDIR([src/papi.c])
AC_CONFIG_HEADER([src/config.h])

# During autoreconf.sh install additional autoconf scripts and
# automake tools in the confdb directory
AC_CONFIG_AUX_DIR([confdb])
AC_CONFIG_MACRO_DIR([confdb])

# For cross-compile support. Requires conf.guess and conf.sub.
# These are also installed in confdb at autoreconf.sh time.
AC_CANONICAL_TARGET
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_ARG_VAR([PAPILIBNAME],
           [can be used to override the name of the PAPI library (default : "papi")])
AC_ARG_VAR([PAPI_HOST_SYSTEM],
           [can be used in cross-compile mode to specify the target system (e.g. bgp, power5+)])
AC_ARG_VAR([PAPI_BUILD_SYSTEM],
           [can be used in cross-compile mode to specify the build system])
AC_ARG_VAR([PAPI_HOST_OS],
           [can be used in cross-compile mode to specify the host operating system (e.g. cle)])
AC_ARG_VAR([PAPI_BUILD_OS],
           [can be used in cross-compile mode to specify the build operating system])

PAPILIBNAME=${PAPILIBNAME:-"papi"}
AC_SUBST(PAPILIBNAME)

PAPI_HOST_SYSTEM=${PAPI_HOST_SYSTEM}
PAPI_HOST_OS=${PAPI_HOST_OS:-"$host_os"}
PAPI_BUILD_SYSTEM=${PAPI_BUILD_SYSTEM}
PAPI_BUILD_OS=${PAPI_HOST_OS:-"$build_os"}

AC_USE_SYSTEM_EXTENSIONS

# Enable automake with relaxed GNU coding standard rules
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR
AM_SILENT_RULES([yes])

PAPI_VERSION="PAPI_VERSION_m4"
AC_SUBST(PAPI_VERSION)

# Add libtool support
LT_INIT()
AM_CONDITIONAL([BUILD_SHARED_LIBS],[test "x$enable_shared" = xyes])

# FIXME: is this really used anywhere?
AC_MSG_CHECKING([for perf_event workaround level])
AC_ARG_WITH([assumed-kernel],
            [AS_HELP_STRING([--with-assumed-kernel=version],
                            [Assume kernel version for purposes of workarounds])],
            [],
            [with_assumed_kernel=default])

if test "$with_assumed_kernel" != "default" ; then
    assumed_kernel=$with_assumed_kernel
    AX_APPEND_FLAG([-DASSUME_KERNEL=$with_assumed_kernel],[CPPFLAGS])
else
    assumed_kernel="autodetect"
fi
AC_MSG_RESULT([$assumed_kernel])

AS_IF([test "$host_cpu" != "$build_cpu"],
      [with_cross_compile=yes],
      [with_cross_compile=no])

# Cross compile support: configure with --host=CPU-VENDOR-OS.
# For example, we can cross compile for BluegeneP using:
# `--host=bluegenep-ibm-cnk`. This results in the canonic form:
# powerpc-ibm-cnk. This is because Autoconf config.sub script
# does not differentiate among bluegene family. Since we need to
# retain the system "version", the variables PAPI_BUILD_SYSTEM
# and PAPI_HOST_SYSTEM allow this information to be provided by
# users. If the user does not provide such information we guess.
# Eventually, not recognized configurations will be reported and
# the configuration process aborted. Cross compilation only works
# for CPU code. Accelerators do not alter the cpu component, as
# their code, is component specific. Support for them is built in
# the corresponding component using the appropriate accelerator
# compiler tool.
AC_MSG_CHECKING([for build architecture])
if test "x`echo $build_os | grep 'linux'`" != "x" ; then
    cpuinfo=`cat /proc/cpuinfo | grep cpu | cut -d: -f2 | cut -d' ' -f2 | sed '2,$d'`
    case "$cpuinfo" in
        PPC970*)
            PAPI_BUILD_SYSTEM=ppc970
        ;;
        POWER5)
            PAPI_BUILD_SYSTEM=power5
            ;;
        POWER5+)
            PAPI_BUILD_SYSTEM=power5+
            ;;
        POWER6)
            PAPI_BUILD_SYSTEM=power6
            ;;
        POWER7)
            PAPI_BUILD_SYSTEM=power7
            ;;
        *)
            PAPI_BUILD_SYSTEM=$build_cpu
            ;;
    esac
elif test "x`echo $build_os | grep 'solaris'`" != "x" ; then
    machine=`uname -m`
    if test "`uname -p`" = "sparc" ; then
        if test "$machine" = "sun4u" ; then
            PAPI_BUILD_SYSTEM=ultra
        elif test "$machine" = "sun4v" ; then
            PAPI_BUILD_SYSTEM=niagara2
        fi
    fi
else
    PAPI_BUILD_SYSTEM=$build_cpu
fi
AC_MSG_RESULT([$PAPI_BUILD_SYSTEM])

AC_MSG_CHECKING([for build operating system])
AC_MSG_RESULT([$PAPI_BUILD_OS])

# Check if we are cross compiling and if the user supplied a system version
if test "$with_cross_compile" = "yes" ; then
    AC_MSG_CHECKING([for cross compile host system])
    if test "x$PAPI_HOST_SYSTEM" = "x" ; then
        AC_MSG_RESULT([undefined])
        AC_MSG_WARN([No host system defined for cross compilation! Going to guess.])
        PAPI_HOST_SYSTEM=$PAPI_BUILD_SYSTEM
        AC_MSG_NOTICE([Guessed cross compile host system: $PAPI_HOST_SYSTEM])
    else
        AC_MSG_RESULT([$PAPI_HOST_SYSTEM])
    fi
else
    AC_MSG_CHECKING([for host system])
    if test "x$PAPI_HOST_SYSTEM" = "x" ; then
        AC_MSG_RESULT([undefined])
        AC_MSG_NOTICE([No host system defined! Going to guess.])
        PAPI_HOST_SYSTEM=$PAPI_BUILD_SYSTEM
        AC_MSG_NOTICE([Guessed host system: $PAPI_HOST_SYSTEM])
    fi
fi

# Some configurations are not allowed and we can filter them out right away
if test "x`echo $host_cpu | grep 'powerpc'`" != "x" ; then
    case $PAPI_HOST_SYSTEM in
        power5|power5+|power6|power7|ppc970*|bgp|bgq)
            ;;
        powerpc*)
            AC_MSG_ERROR([Building for powerpc... PAPI_HOST_SYSTEM should be set to cpu model.])
            ;;
    esac
elif test "x`$host_cpu | grep 'sparc'`" != "x" ; then
    case $PAPI_HOST_SYSTEM in
        ultra | niagara2)
            ;;
        *)
            AC_MSG_ERROR([Building for sparc... PAPI_HOST_SYSTEM should be set to cpu model.])
            ;;
    esac
fi

with_perf_api=     # CPU performance API to use when building PAPI
with_perf_api_ver= # CPU performance API version to use when building PAPI
with_perf_lib=     # CPU performance Library to go along with the performance API
with_perf_lib_ver= # CPU performance Library version
with_perf_lib_emb= # CPU performance Library is embedded [y/n]

# Following we use the host system and os combination to
# guess the performance API and library to use if the user
# does not define them
case $PAPI_HOST_SYSTEM,$PAPI_HOST_OS in
    x86_64,linux* \
    | k1om,linux*)
        with_perf_api=perf_event
        with_perf_lib=pfm
        with_perf_lib_ver=4
        with_perf_lib_emb=y
        ;;
    bgp,cnk)
        with_perf_api=bgpm
        with_perf_lib=bgpm
        with_perf_lib_emb=n
        ;;
    bgq,cnk)
        with_perf_api=bgpm
        with_perf_lib=bgpm
        with_perf_lib_emb=n
        ;;
    ultra,solaris* \
    | niagara2,solaris*)
        with_perf_api=pcp
        with_perf_lib=pcp
        with_perf_lib_emb=n
        ;;
    *,freebsd*)
        with_perf_api=pmc
        with_perf_lib=pmc
        with_perf_lib_ver=
        with_perf_lib_emb=n
        ;;
    power5,aix    \
    | power5+,aix \
    | power6,aix  \
    | power7,aix  \
    | ppc970,aix)
        with_perf_api=pmapi
        with_perf_lib=pmapi
        with_perf_emb=n
        ;;
    *,*)
        AC_MSG_WARN([Cannot guess performance API and library for $PAPI_HOST_SYSTEM,$PAPI_HOST_OS.])
        ;;
esac

AX_APPEND_FLAG([-g],[CFLAGS])

#If not set, set FFLAGS to null to prevent AC_PROG_F77 from defaulting it to -g -O2
if test "x$FFLAGS" = "x"; then
    FFLAGS=""
fi

AC_PROG_CC([xlc icc gcc cc])
AC_PROG_CXX
AC_PROG_FC([xlf ifort gfortran f95 f90])
AC_PROG_F77([xlf ifort gfortran f77])
AC_CHECK_PROG([MPICC], [mpicc], [mpicc], [])

AC_PATH_PROG([PERL],[perl])

# Lets figure out what CC actually is...
# Used in later checks to set compiler specific options
with_mpi_tests=no
if test "$CC" = "icc" ; then
    if test "$MPICC" = "mpicc"; then
        # Check if mpiicc is available
        MPI_COMPILER_CHECK=`$MPICC -V 2>&1 | grep '^Intel(R) C'`
        if test "x$MPI_COMPILER_CHECK" = "x"; then
            AC_MSG_WARN([$MPICC is using a different compiler than $CC. MPI tests disabled.])
            with_mpi_tests=yes
        fi
    fi
elif "$CC" = "gcc" ; then
    if test "$MPICC" = "mpicc"; then
        MPI_COMPILER_CHECK=`$MPICC -v 2>&1 | grep 'gcc version'`
        if test "x$MPI_COMPILER_CHECK" = "x"; then
            AC_MSG_WARN([$MPICC is using a different compiler than $CC. MPI tests disabled.])
            with_mpi_tests=yes
        fi
    fi
elif "$CC" = "xlc" ; then
    if test "$MPICC" = "mpicc"; then
        MPI_COMPILER_CHECK=`$MPICC -qversion 2>&1 | grep 'IBM XL C'`
        if test "x$MPI_COMPILER_CHECK" = "x"; then
            AC_MSG_WARN([$MPICC is using a different compiler than $CC. MPI tests disabled.])
            with_mpi_tests=yes
        fi
    fi
fi

# prevent icc warnings about overriding optimization settings set by AC_PROG_CC
# remark #869: parameter was never referenced
# remark #271: trailing comma is nonstandard
if test "$CC" = "icc" ; then
    AX_APPEND_FLAG([-diag-disable 188,869,271],[CFLAGS])
    if test "$host_cpu" = "k1om"; then
        AX_APPEND_FLAG([-mmic],[CC])
    fi
fi

if test "$F77" = "ifort" -a "$host_cpu" = "k1om" ; then
    AX_APPEND_FLAG([-mmic],[F77])
fi

AC_PROG_AWK
AC_PROG_GREP
AC_PROG_EGREP
AC_PROG_CPP
AC_GNU_SOURCE
AC_HEADER_STDC
AC_C_INLINE
AC_HEADER_TIME

AC_CHECK_HEADERS([sys/time.h c_asm.h intrinsics.h mach/mach_time.h sched.h])
AC_CHECK_FUNCS([gethrtime read_real_time time_base_to_time clock_gettime mach_absolute_time sched_getcpu])

#
# Check if the system provides time_* symbols without -lrt, and if not,
# check for -lrt existance.
#

AC_MSG_CHECKING([for timer_create and timer_*ettime symbols in base system])
AC_TRY_LINK([
             #include <signal.h>
             #include <time.h>
            ],
            [timer_t timerid; timer_create(CLOCK_REALTIME, NULL, &timerid);],
            [rtsymbols_in_base="yes"],
            [rtsymbols_in_base="no"])

if test "${rtsymbols_in_base}" = "yes"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_RESULT([not found])
    AC_MSG_CHECKING([for timer_create and timer_*ettime symbols in -lrt])
    AX_VAR_PUSHVALUE([LIBS],[-lrt])
    AC_TRY_LINK([
                 #include <signal.h>
                 #include <time.h>
                ],
                [timer_t timerid; timer_create(CLOCK_REALTIME, NULL, &timerid);],
                [has_lrt="yes"],
                [has_lrt="no"])
    AX_VAR_POPVALUE([LIBS])
    if test "${has_lrt}" = "yes" ; then
        AC_MSG_RESULT([found])
        AX_APPEND_FLAG([-lrt],[LIBS])
    else
        AC_MSG_RESULT([not found])
        AC_MSG_CHECKING([for timer_create and timer_*ettime symbols in -lrt -lpthread])
        AX_VAR_PUSHVALUE([LIBS],[-lrt -lpthread])
        AC_TRY_LINK([
                     #include <signal.h>
                     #include <time.h>
                    ],
                    [timer_t timerid; timer_create(CLOCK_REALTIME, NULL, &timerid);],
                    [has_lrt_lpthd="yes"],
                    [has_lrt_lpthd="no"])
        AX_VAR_POPVALUE([LIBS])
        if test "${has_lrt_lpthd}" = "yes" ; then
            AC_MSG_RESULT([found])
            AX_APPEND_FLAG([-lrt],[LIBS])
            AX_APPEND_FLAG([-lpthread],[LIBS])
        else
            AC_MSG_ERROR([cannot find timer_create and timer_*ettime symbols neither in the base system libraries nor in -lrt, nor in -lrt -lpthread])
        fi
    fi
fi

#
# Check if the system provides dl* symbols without -ldl, and if not,
# check for -ldl existance.
#

AC_MSG_CHECKING([for dlopen and dlerror symbols in base system])
AC_TRY_LINK([#include <dlfcn.h>],
            [void *p = dlopen ("", 0); char *c = dlerror();],
            [dlsymbols_in_base="yes"],
            [dlsymbols_in_base="no"])

if test "${dlsymbols_in_base}" = "yes"; then
    AC_MSG_RESULT([found])
else
    AC_MSG_RESULT([not found])
    AC_MSG_CHECKING([for dlopen and dlerror symbols in -ldl])
    AX_VAR_PUSHVALUE([LIBS],[-ldl])
    AC_TRY_LINK([#include <dlfcn.h>],
                [void *p = dlopen ("", 0); char *c = dlerror();],
                [has_ldl="yes"],
                [has_ldl="no"])
    AX_VAR_POPVALUE([LIBS])
    if test "${has_ldl}" = "yes" ; then
        AC_MSG_RESULT([found])
        AX_APPEND_FLAG([-ldl],[LIBS])
    else
        AC_MSG_ERROR([cannot find dlopen and dlerror symbols neither in the base system libraries nor in -ldl])
    fi
fi

# set features for different combinations of host system and os
with_virtualtimer=
with_walltimer=
with_tls=
enable_ffsll=

case $PAPI_HOST_SYSTEM,$PAPI_HOST_OS in
    *,cle)
        with_virtualtimer=times
        with_walltimer=cycle
        with_tls=__thread
        enable_ffsll=yes
        ;;
    bgp,cnk)
        with_virtualtimer=perfctr
        with_walltimer=cycle
        with_tls=no
        enable_ffsll=yes
        ;;
    bgq,cnk)
        with_virtualtimer=no
        with_walltimer=no
        with_tls=no
        enable_ffsll=no
        ;;
    ultra,solaris* \
    | niagara2,solaris*)
        with_virtualtimer=no
        with_walltimer=no
        with_tls=no
        enable_ffsll=no
        ;;
    *,solaris*)
        AC_MSG_ERROR([Only SPARC processors are supported on Solaris])
        ;;
esac

AC_MSG_CHECKING([for tests])
AC_ARG_WITH([tests],
            [
   --with-tests=@<:@LIST@:>@ Specify space separated LIST of tests to run on install.
        ctests          - C tests
        ftests          - Fortran tests
        mpitests        - MPI tests
        no              - no tests

        If not given default: all available tests.
            ],,
            [with_tests="ctests ftests mpitests"])

if test "$with_tests" = "no" ; then
    AC_MSG_RESULT([no])
else
    tmp_tests=

    if test "x`echo $with_tests | grep 'ctests'`" != "x" ; then
        tmp_tests+="ctests "
        # mpi tests require ctests to build
        if test "x`echo $with_tests | grep 'mpitests'`" != "x" ; then
            if test "x$with_mpi_tests" = "xyes" -a "$enable_shared" = "yes"; then
                tmp_tests+="mpitests "
            fi
        fi
    fi

    if test "x`echo $with_tests | grep 'ftests'`" != "x" ; then
        tmp_tests+="ftests "
    fi

    with_tests=$tmp_tests
    if test "x$with_tests" = "x"; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([$with_tests])
    fi
fi

AM_CONDITIONAL([BUILD_CTESTS],[test "x`echo $with_tests | grep 'ctests'`" != "x"])
AM_CONDITIONAL([BUILD_FTESTS],[test "x`echo $with_tests | grep 'ftests'`" != "x"])
AM_CONDITIONAL([BUILD_MPITESTS],[test "x`echo $with_tests | grep 'mpitests'`" != "x"])


AC_MSG_CHECKING([for debug build])
AC_ARG_WITH([debug],
            [AS_HELP_STRING([--with-debug=value],
                            [Build PAPI with debug set to: yes, memory (for debug plus memory tracker), none;
                             default: no])],,
            [with_debug=no])

case "$with_debug" in
    yes)
        if test "CC" = "gcc" ; then
            AX_APPEND_FLAG([-g3],[CFLAGS])
        fi
        AX_APPEND_FLAG([-O0],[CFLAGS])
        AX_APPEND_FLAG([-DDEBUG],[CPPFLAGS])
        AX_APPEND_FLAG([-DPAPI_NO_MEMORY_MANAGEMENT],[CPPFLAGS])
        ;;
    memory)
        if test "$CC" = "gcc" ; then
            AX_APPEND_FLAG([-g3],[CFLAGS])
        fi
        AX_APPEND_FLAG([-DDEBUG],[CPPFLAGS])
        ;;
    no)
        AX_APPEND_FLAG([-DPAPI_NO_MEMORY_MANAGEMENT],[CPPFLAGS])
        ;;
esac
AC_MSG_RESULT([$with_debug])


if test "$CC" = "gcc" ; then
    gcc_version=`gcc -v 2>&1 | tail -n 1 | awk '{printf $3}'`
    major=`echo $gcc_version | sed 's/\([[^.]][[^.]]*\).*/\1/'`
    minor=`echo $gcc_version | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
    if (test "$major" -ge 4 || test "$major" = 3 -a "$minor" -ge 4) ; then
        AX_APPEND_FLAG([-Wextra],[CFLAGS])
    else
        AX_APPEND_FLAG([-W],[CFLAGS])
    fi
    # -Wextra => -Woverride-init on gcc >= 4.2
    # This issues a warning (error under -Werror) for some libpfm4 code.
    AC_MSG_CHECKING([for -Wno-override-init])
    AX_VAR_PUSHVALUE([CFLAGS],[-Wall -Wextra -Werror -Wno-override-init])
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([
                                       struct A {
                                           int x;
                                           int y;
                                       };
                                       int main(void) {
                                           struct A a = {.x = 0, .y = 0, .y = 5 };
                                           return a.x;
                                       }
                                      ])
                      ],
                      [have_no_override_init=yes],
                      [have_no_override_init=no])
    AX_VAR_POPVALUE([CFLAGS])
    if test "$have_no_override_init" = "yes" ; then
        AX_APPEND_FLAG([-Wno-override-init],[CFLAGS])
    fi
    AC_MSG_RESULT([$have_no_override_init])
fi

# First set pthread-mutexes based on arch
case $host_cpu in
    aarch64|arm*|parisc*)
        AX_APPEND_FLAG([-DUSE_PTHREAD_MUTEXES],[CPPFLAGS])
        echo "forcing use of pthread mutexes... " >&6
        ;;
esac

AC_ARG_ENABLE([pthread-mutex],
              [AS_HELP_STRING([--enable-pthread-mutex],
                              [Specify use of pthread mutex rather than custom PAPI locks])],
              [AX_APPEND_FLAG([-DUSE_PTHREAD_MUTEXES],[CPPFLAGS])])

AC_ARG_ENABLE([ffsll],
              [AS_HELP_STRING([--enable-ffsll],
                              [Enable use of the ffsll() function])])

if test "$enable_ffsll" = "no" ; then
    if test "$with_cross_compile" = "yes" ; then
        AC_MSG_ERROR([ffsll must be specified for cross compile])
    fi
    AC_CHECK_FUNC(ffsll,[ffsll=yes],[ffsll=no])

    if test "$enable_ffsll" = "yes" ; then
        AC_DEFINE([HAVE_FFSLL],[1],[This platform has the ffsll() function])
    fi
fi

AC_MSG_CHECKING([for working gettid])
AC_LINK_IFELSE([AC_LANG_SOURCE([
                                #include <sys/types.h>
                                main() { pid_t a = gettid(); }
                               ])],
               [papi_ac_cv_have_gettid=yes],
               [papi_ac_cv_have_gettid=no])

if test "$papi_ac_cv_have_gettid" = "yes" ; then
    AC_DEFINE([HAVE_GETTID],[1],[Full gettid function])
    AC_MSG_RESULT([yes])
else
    AC_MSG_RESULT([no])
    AC_MSG_CHECKING([for working syscall(SYS_gettid)])
    AC_LINK_IFELSE([AC_LANG_SOURCE([
                                    #include <sys/types.h>
                                    #include <sys/syscall.h>
                                    main() { pid_t a = syscall(SYS_gettid); }
                                   ])],
                   [papi_ac_cv_have_sys_gettid=yes],
                   [papi_ac_cv_have_sys_gettid=no])

    if test "$papi_ac_cv_have_sys_gettid" = "yes" ; then
        AC_DEFINE([HAVE_SYSCALL_GETTID],[1],[gettid syscall function])
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
    fi
fi


##################################################
#    select walltimer/virtualtimer/tls/ffsll
##################################################
AC_ARG_WITH([walltimer],
            [AS_HELP_STRING([--with-walltimer=realtimer],
                            [Specify real timer: gettimeofday, mmtimer, clock_realtime_hr, clock_realtime,cycle])])

if test "x$with_walltimer" = "x" ; then
    AC_MSG_CHECKING([for working CLOCK_REALTIME_HR POSIX 1b timer])
    AC_TRY_RUN([
                #include <unistd.h>
                #include <stdlib.h>
                #include <stdio.h>
                #include <time.h>
                #include <syscall.h>
                main() {
                    struct timespec t1, t2;
                    double seconds;
                    if (syscall(__NR_clock_gettime,CLOCK_REALTIME_HR,&t1) == -1) exit(1);
                        sleep(1);
                    if (syscall(__NR_clock_gettime,CLOCK_REALTIME_HR,&t2) == -1) exit(1);
                        seconds = ((double)t2.tv_sec + (double)t2.tv_nsec/1000000000.0) - ((double)t1.tv_sec + (double)t1.tv_nsec/1000000000.0);
                    if (seconds > 1.0)
                        exit(0);
                    else
                        exit(1);
                }
               ],
               [with_walltimer=clock_realtime_hr],
               [])
    if test "x$with_walltimer" = "xclock_realtime_hr" ; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
        AC_MSG_CHECKING([for working CLOCK_REALTIME POSIX 1b timer])
        AC_TRY_RUN([
                    #include <unistd.h>
                    #include <stdlib.h>
                    #include <stdio.h>
                    #include <time.h>
                    #include <syscall.h>
                    main() {
                        struct timespec t1, t2;
                        double seconds;
                        if (syscall(__NR_clock_gettime,CLOCK_REALTIME,&t1) == -1) exit(1);
                            sleep(1);
                        if (syscall(__NR_clock_gettime,CLOCK_REALTIME,&t2) == -1) exit(1);
                            seconds = ((double)t2.tv_sec + (double)t2.tv_nsec/1000000000.0) - ((double)t1.tv_sec + (double)t1.tv_nsec/1000000000.0);
                        if (seconds > 1.0)
                            exit(0);
                        else
                            exit(1);
                    }
                   ],
                   [
                    with_walltimer="clock_realtime"
                    AC_MSG_RESULT([yes])
                   ],
                   [
                    with_walltimer="cycle"
                    AC_MSG_RESULT([no])
                   ])
    fi
fi

AC_MSG_CHECKING([for which real time clock to use])
if test "$with_walltimer" = "gettimeofday"; then
    AC_DEFINE(HAVE_GETTIMEOFDAY, 1, [Normal gettimeofday timer])
elif test "$with_walltimer" = "clock_realtime_hr"; then
    AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [POSIX 1b clock])
    AC_DEFINE(HAVE_CLOCK_GETTIME_REALTIME_HR, 1, [POSIX 1b realtime HR clock])
elif test "$with_walltimer" = "clock_realtime"; then
    AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [POSIX 1b clock])
    AC_DEFINE(HAVE_CLOCK_GETTIME_REALTIME, 1, [POSIX 1b realtime clock])
elif test "$with_walltimer" = "cycle"; then
    AC_DEFINE(HAVE_CYCLE, 1, [Native access to a hardware cycle counter])
else
    AC_MSG_ERROR([Unknown value for walltimer])
fi
AC_MSG_RESULT([$with_walltimer])

AX_VAR_PUSHVALUE([LIBS],[])
AX_VAR_PUSHVALUE([LDFLAGS],[])
AX_VAR_PUSHVALUE([CFLAGS],[-pthread])

AC_ARG_WITH([tls],
            [AS_HELP_STRING([--with-tls=tls],
                            [This platform supports thread local storage (e.g. __thread)])])

if test "x$with_tls" = "x" ; then
    if test "$with_cross_compile" = "yes" ; then
        AC_MSG_ERROR([tls must be specified for cross compile])
    fi
    AC_MSG_CHECKING([for working __thread])
    AC_TRY_RUN([
                #include <pthread.h>
                #include <unistd.h>
                extern __thread int i;
                static int res1, res2;
                void thread_main (void *arg) {
                    i = (int)arg;
                    sleep (1);
                    if ((int)arg == 1)
                        res1 = (i == (int)arg);
                    else
                        res2 = (i == (int)arg);
                }
                __thread int i;
                int main () {
                    pthread_t t1, t2;
                    i = 5;
                    pthread_create (&t1, NULL, thread_main, (void *)1);
                    pthread_create (&t2, NULL, thread_main, (void *)2);
                    pthread_join (t1, NULL);
                    pthread_join (t2, NULL);
                    return !(res1 + res2 == 2);
                }
               ],
               [
                AC_MSG_RESULT([yes])
                with_tls="__thread"
               ],
               [
                AC_MSG_RESULT([no])
                with_tls="no"
               ])
fi

if test "x`echo $host_os | grep 'linux'`" != "x"; then
    if test "x$with_tls" = "x__thread"; then
        # On some linux distributions, TLS works in executables, but linking against
        # a shared library containing TLS fails with: undefined reference to '__tls_get_addr'
        AX_VAR_PUSHVALUE([CFLAGS],[-fPIC --shared])
        AC_TRY_LINK([],
                    [static __thread int foo; void main() { foo = 5; }],,
                    [
                     AC_MSG_WARN([Disabling usage of __thread.])
                     with_tls=no
                    ])
        AX_VAR_POPVALUE([CFLAGS])
    fi
fi

AC_MSG_CHECKING([for high performance thread local storage])
if test "$with_tls" = "no"; then
    AX_APPEND_FLAG([-DNO_TLS],[CPPFLAGS])
elif test "x$with_tls" != "x"; then
    if test "$with_tls" = "yes"; then
        with_tls="__thread"
    fi
    AX_APPEND_FLAG([-DUSE_COMPILER_TLS],[CPPFLAGS])
    AC_DEFINE_UNQUOTED([HAVE_THREAD_LOCAL_STORAGE],[$with_tls],[Keyword for per-thread variables])
fi
AC_MSG_RESULT([$with_tls])

AC_ARG_WITH([virtualtimer],
            [AS_HELP_STRING([--with-virtualtimer=virtualtimer],
                            [Specify per-thread virtual timer: clock_thread_cputime,times,proc,getrusage,perfctr])])

if test "x$with_virtualtimer" = "x" ; then
    # Check for cross compilation
    if test "$with_cross_compile" = "yes" ; then
        AC_MSG_ERROR([virtualtimer must be specified for cross compile])
    fi
    AC_MSG_CHECKING([for working CLOCK_THREAD_CPUTIME_ID POSIX 1b timer])
    AC_TRY_RUN([
                #include <pthread.h>
                #include <sys/signal.h>
                #include <sys/times.h>
                #include <assert.h>
                #include <stdio.h>
                #include <sys/types.h>
                #include <unistd.h>
                #include <sys/unistd.h>
                #include <syscall.h>
                #include <stdlib.h>
                #if !defined( SYS_gettid )
                #define SYS_gettid 1105
                #endif
                struct timespec threadone = { 0, 0 };
                struct timespec threadtwo = { 0, 0 };
                pthread_t threadOne, threadTwo;
                volatile int done = 0;

                int gettid() {
                    return syscall( SYS_gettid );
                }

                void *doThreadOne( void * v ) {
                    while (!done)
                        sleep(1);
                    if (syscall(__NR_clock_gettime,CLOCK_THREAD_CPUTIME_ID,&threadone) == -1) {
                        perror("clock_gettime(CLOCK_THREAD_CPUTIME_ID)");
                        exit(1);
                    }
                    return 0;
                }

                void *doThreadTwo( void * v ) {
                    long i, j = 0xdeadbeef;
                    for( i = 0; i < 0xFFFFFFF; ++i ) { j = j ^ i; }

                    if (syscall(__NR_clock_gettime,CLOCK_THREAD_CPUTIME_ID,&threadtwo) == -1) {
                        perror("clock_gettime(CLOCK_THREAD_CPUTIME_ID)");
                        exit(1);
                    }
                    done = 1;
                    return j;
                }

                int main( int argc, char ** argv ) {
                    int status = pthread_create( & threadOne, NULL, doThreadOne, NULL );
                    assert( status == 0 );
                    status = pthread_create( & threadTwo, NULL, doThreadTwo, NULL );
                    assert( status == 0 );
                    status = pthread_join( threadTwo, NULL );
                    assert( status == 0 );
                    status = pthread_join( threadOne, NULL );
                    assert( status == 0 );
                    if ((threadone.tv_sec != threadtwo.tv_sec) || (threadone.tv_nsec != threadtwo.tv_nsec))
                        exit(0);
                    else {
                        fprintf(stderr,"T1 %ld %ld T2 %ld %ld\n",threadone.tv_sec,threadone.tv_nsec,threadtwo.tv_sec,threadtwo.tv_nsec);
                        exit(1);
                    }
                }
               ],
               [with_virtualtimer=clock_thread_cputime_id],
               [])

    if test "x$with_virtualtimer" = "xclock_thread_cputime_id" ; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
        # *** Checks for working per thread timer***
        AC_MSG_CHECKING([for working per-thread times() timer])
        AC_TRY_RUN([
                    #include <pthread.h>
                    #include <sys/signal.h>
                    #include <sys/times.h>
                    #include <assert.h>
                    #include <stdio.h>
                    #include <sys/types.h>
                    #include <unistd.h>
                    #include <sys/unistd.h>
                    #include <syscall.h>
                    #include <stdlib.h>
                    #if !defined( SYS_gettid )
                    #define SYS_gettid 1105
                    #endif
                    long threadone = 0, threadtwo = 0;
                    pthread_t threadOne, threadTwo;
                    volatile int done = 0;

                    int gettid() {
                        return syscall( SYS_gettid );
                    }
                    int doThreadOne( void * v ) {
                        struct tms tm;
                        int status;
                        while (!done)
                            sleep(1);
                        status = times( & tm );
                        assert( status != -1 );
                        threadone = tm.tms_utime;
                        return 0;
                    }
                    int doThreadTwo( void * v ) {
                        struct tms tm;
                        long i, j = 0xdeadbeef;
                        int status;
                        for( i = 0; i < 0xFFFFFFF; ++i ) { j = j ^ i; }
                        status = times( & tm );
                        assert( status != -1 );
                        threadtwo = tm.tms_utime;
                        done = 1;
                        return j;
                    }
                    int main( int argc, char ** argv ) {
                        int status = pthread_create( & threadOne, NULL, doThreadOne, NULL );
                        assert( status == 0 );
                        status = pthread_create( & threadTwo, NULL, doThreadTwo, NULL );
                        assert( status == 0 );
                        status = pthread_join( threadTwo, NULL );
                        assert( status == 0 );
                        status = pthread_join( threadOne, NULL );
                        assert( status == 0 );
                        return (threadone == threadtwo);
                    }
                   ],
                   [
                    AC_MSG_RESULT([yes])
                    with_virtualtimer="times"
                   ],
                   [
                    AC_MSG_RESULT([no])
                    with_virtualtimer="default"
                   ])
    fi
fi

AX_VAR_POPVALUE([LDFLAGS])
AX_VAR_POPVALUE([CFLAGS])
AX_VAR_POPVALUE([LIBS])

AC_MSG_CHECKING([for which virtual timer to use])
case "$with_virtualtimer" in
    times)
        AC_DEFINE([HAVE_PER_THREAD_TIMES],[1],[Working per thread timer])
        ;;
    getrusage)
        AC_DEFINE([HAVE_PER_THREAD_GETRUSAGE],[1],[Working per thread getrusage])
        ;;
    clock_thread_cputime_id)
        AC_DEFINE([HAVE_CLOCK_GETTIME_THREAD],[CLOCK_THREAD_CPUTIME_ID],[POSIX 1b per-thread clock])
        ;;
    proc|default|perfctr)
        AC_DEFINE([USE_PROC_PTTIMER],[1],[Use /proc for per-thread times])
        AC_DEFINE([USE_PERFCTR_PTTIMER],[1],[Use the perfctr virtual TSC for per-thread times])
        ;;
esac
AC_MSG_RESULT([$with_virtualtimer])

AC_MSG_CHECKING([for static user preset events])
# TODO: double check this
AC_ARG_WITH([static-user-events],
            [AS_HELP_STRING([--with-static-user-events=file],
                            [Build with a static user events file.])],
            [AX_APPEND_FLAG([-D$with_static_user_events],[CPPFLAGS])])
AC_MSG_RESULT([$with_static_user_events])

AC_MSG_CHECKING([for static PAPI preset events])
# TODO: might be converted to AC_ARG_ENABLE ?
AC_ARG_WITH([static-papi-events],
            [AS_HELP_STRING([--with-static-papi-events],
                            [Build with a static papi events file.])],,
            [
             with_static_papi_events=yes
             AX_APPEND_FLAG([-DSTATIC_PAPI_EVENTS_TABLE],[CPPFLAGS])
            ])
AC_MSG_RESULT([$with_static_papi_events])

AC_MSG_CHECKING([for whether to build shared library])
AC_MSG_RESULT([$enable_shared])

AC_MSG_CHECKING([for static compile of tests and utilities])
AC_ARG_ENABLE([static-tools],
              [AS_HELP_STRING([--enable-static-tools],
                              [Specify static compile of tests and utilities])],
              [
               enable_static_tools=yes
               enable_shared_tools=no
              ])
AC_MSG_RESULT([$enable_static_tools])

AC_MSG_CHECKING([for linking with papi shared library of tests and utilities])
AC_ARG_ENABLE([shared-tools],
              [AS_HELP_STRING([--enable-shared-tools],
                              [Specify linking with papi library of tests and utilities])],
              [
               enable_shared_tools=yes
               enable_static_tools=no
              ],
              [
               enable_shared_tools=no
               enable_static_tools=yes
              ])
AC_MSG_RESULT([$enable_shared_tools])

# User may request shared tools but system might not provide it
if test "$enable_shared_tools" = "yes" -a "$enable_shared" = "no"; then
    AC_MSG_ERROR([Building static but specified shared linking for tests and utilities])
    enable_shared_tools=no
    enable_static_tools=yes
fi

if test "$enable_static_tools" = "yes" ; then
    TOOLS_LINK_MODE=-static
    AC_SUBST(TOOLS_LINK_MODE)
fi

##################################################
#     select performance interface/library
##################################################

AC_MSG_CHECKING([for performance inteface])
AC_ARG_WITH([perf-api],
            [
   --with-perf-api=interface Specify the performance interface to use.
        perf_event      - Linux Performance Events interface
        bgpm            - Blue Gene Performance Monitoring interface
        cpc             - Solaris CPU Performance Counters interface
        pmc             - FreeBSD Performance Monitoring Counters interface
        pmapi           - AIX Performance Monitoring API interface
            ],
            [
             case "$with_perf_api" in
                 perf_event|bgpm|cpc|pmc|pmapi)
                    ;;
                 *)
                    AC_MSG_RESULT([none])
                    AC_MSG_ERROR([$with_perf_api is not a valid interface.])
                    ;;
             esac
            ])
AC_MSG_RESULT([$with_perf_api])

AC_ARG_WITH([perf-api-ver],
            [AS_HELP_STRING([--with-perf-api-ver=version],
                            [Specify the performance interface version to use.])])

AC_MSG_CHECKING([for performance library])
AC_ARG_WITH([perf-lib],
            [
   --with-perf-lib=library Specify the CPU performance library to use.
        pfm             - Linux Performance Monitoring library
        bgpm            - Blue Gene Performance Monitoring library
        cpc             - Solaris CPU Performance Counters library
        pmc             - FreeBSD Performance Monitoring Counters library
        pmapi           - AIX Performance Monitoring API libary
            ],
            [
             case "$with_perf_lib" in
                 pfm|bgpm|cpc|pmc)
                    ;;
                 *)
                    AC_MSG_ERROR([lib$with_perf_lib is not a valid library.])
                    ;;
             esac
            ])
AC_MSG_RESULT([$with_perf_lib])

AM_CONDITIONAL([BUILD_PFMLIB_VER_4],[test "x$with_perf_lib-$with_perf_lib_ver" = "xpfm-4"])

AC_MSG_CHECKING([for performance library version])
AC_ARG_WITH([perf-lib-ver],
            [AS_HELP_STRING([--with-perf-lib-ver=version],
                            [Specify the performance library version to use])])
AC_MSG_RESULT([$with_perf_lib_ver])

AC_ARG_WITH([perf-lib-prefix],
            [AS_HELP_STRING([--with-perf-lib-prefix=prefix],
                            [Specify location of user provided performance library])],
            [
             with_perf_lib_emb=n
            ],
            [
             with_perf_lib_emb=y
            ])

AC_ARG_WITH([perf-lib-incdir],
            [AS_HELP_STRING([--with-perf-lib-incdir=includedir],
                            [Specify location of performance library headers])],
            [
             with_perf_lib_emb=n
             AX_APPEND_FLAG([-I$with_perf_lib_incdir],[CPPFLAGS])
            ],
            [
             with_perf_lib_emb=y
            ])

AC_ARG_WITH([perf-lib-libdir],
            [AS_HELP_STRING([--with-perf-lib-libdir=librarydir],
                            [Specify location of performance library to link to])],
            [
             with_perf_lib_emb=n
             AX_APPEND_FLAG([-L$with_perf_lib_libdir],[LDFLAGS])
            ],
            [
             with_perf_lib_emb=y
            ])

# configure compiler flags for picked performance library
with_perf_lib_name=
case "$with_perf_lib-$with_perf_lib_ver" in
    pfm-4)
        if test "$with_perf_lib_emb" = "y" ; then
            with_perf_lib_prefix=$srcdir/external/libpfm4
            with_perf_lib_incdir=$with_perf_lib_prefix/include
            with_perf_lib_libdir=$with_perf_lib_prefix/lib
            with_perf_lib_subdir=external/libpfm4
            AX_SUBDIRS_CONFIGURE([external/libpfm4],[--enable-embedded])
            libpfm_la=external/libpfm4/libpfm.la
            AC_SUBST([libpfm_la])
        else
            with_perf_lib_incdir=$with_perf_lib_prefix/include
            with_perf_lib_libdir=$with_perf_lib_prefix/lib
            with_perf_lib_subdir=
            with_perf_lib_name=$with_perf_lib
        fi
        ;;
    bgpm-*)
        if test "x$with_perf_lib_prefix" != "x" ; then
            with_perf_lib_incdir=$with_perf_lib_prefix
            with_perf_lib_libdir=$with_perf_lib_prefix/bgpm/lib
            with_perf_lib_subdir=
            AX_APPEND_FLAG([-I$with_bgpm_installdir/spi/include/kernel/cnk],[CPPFLAGS])
        else
            AC_MSG_ERROR([BGPM prefix not specified. Rerun configure with --with-perf-lib-prefix.])
        fi
        ;;
    cpc-*)
        if test "x$with_perf_lib_prefix" != "x" ; then
            with_perf_lib_incdir=$with_perf_lib_prefix/include
            with_perf_lib_libdir=$with_perf_lib_prefix/lib
            with_perf_lib_subdir=
            # the following is needed as libcpc and libcpc2 are distinguished
            with_perf_lib_name=$with_perf_lib$with_perf_lib_ver
        fi
        ;;
    pmc-*)
        if test "x$with_perf_lib_prefix" != "x" ; then
            with_perf_lib_incdir=$with_perf_lib_prefix/include
            with_perf_lib_libdir=$with_perf_lib_prefix/lib
            with_perf_lib_subdir=
        fi
        ;;
    pmapi-*)
        if test "x$with_perf_lib_prefix" != "x" ; then
            with_perf_lib_incdir=$with_perf_lib_prefix/include
            with_perf_lib_libdir=$with_perf_lib_prefix/lib
            with_perf_lib_subdir=
        else
            # Assume AIX
            with_perf_lib_incdir=/usr/pmapi/include
            with_perf_lib_libdir=/usr/pmapi/lib
            with_perf_lib_subdir=
        fi
        ;;
    *)
        AC_MSG_ERROR([$with_perf_lib-$with_perf_lib_ver is not a supported version.])
        ;;
esac

# Add performance library to compile flags
AX_APPEND_FLAG([-I$with_perf_lib_incdir],[CPPFLAGS])
AX_APPEND_FLAG([-L$with_perf_lib_libdir],[LDFLAGS])

if test "x$with_perf_lib_name" != "x" ; then
    AX_APPEND_FLAG([-l$with_perf_lib_name],[LIBS])
fi

# check combinations of OS, performance API and library
case "$PAPI_HOST_OS,$with_perf_api,$with_perf_lib-$with_perf_lib_ver" in
    aix,pmapi,pmapi-*)
        AC_MSG_CHECKING([for libpmapi])
        AC_CHECK_LIB([pmapi], [pm_initialize],
                     [AX_APPEND_FLAG([-DPM_INITIALIZE],[CPPFLAGS])],
                     [AC_CHECK_LIB([pmapi], [pm_init],
                                   [AX_APPEND_FLAG([-DPM_INIT],[CPPFLAGS])],
                                   [AC_MSG_ERROR([libpmapi.a not found, rerun configure with different flags])])
                     ])
        AC_MSG_RESULT([found])
        ;;
    linux*,perf_event,pfm-4)
        # check performance library
        if test "$with_cross_compile" = "no" -a "$with_perf_lib_emb" = "no" ; then
            AC_CHECK_LIB([pfm], [pfm_initialize])
            if test "$ac_cv_lib_pfm_pfm_initialize" = "yes" ; then
                AC_CHECK_HEADERS([perfmon/pfmlib.h],
                                 [
                                  if test "$host_cpu" = "ia64"; then
                                      AC_CHECK_HEADER([perfmon/pfmlib_montecito.h],,
                                                      [AC_MSG_ERROR([pfmlib_montecito.h not found])])
                                  fi
                                  AC_CHECK_FUNC([pfm_get_event_description],
                                                [AC_DEFINE([HAVE_PFM_GET_EVENT_DESCRIPTION],
                                                           [1],
                                                           [event description function])])
                                  AC_CHECK_MEMBER([pfmlib_reg_t.reg_evt_idx],
                                                  [AC_DEFINE([HAVE_PFM_REG_EVT_IDX],
                                                             [1],
                                                             [old reg_evt_idx])
                                                  ],,
                                                  [#include "perfmon/pfmlib.h"])
                                  AC_CHECK_MEMBER([pfmlib_output_param_t.pfp_pmd_count],
                                                  [AC_DEFINE([HAVE_PFMLIB_OUTPUT_PFP_PMD_COUNT],
                                                             [1],
                                                             [new pfmlib_output_param_t])
                                                  ],,
                                                  [#include "perfmon/pfmlib.h"])
                                  AC_CHECK_MEMBER([pfm_msg_t.type],
                                                  [AC_DEFINE([HAVE_PFM_MSG_TYPE],
                                                             [1],
                                                             [new pfm_msg_t])
                                                  ],,
                                                  [#include "perfmon/perfmon.h"])
                                 ],
                                 [
                                  AC_MSG_ERROR([perfmon/pfmlib.h not found, rerun configure with different flags])
                                 ])
            else
                AC_MSG_ERROR([libpfm not found, rerun configure with different flags])
            fi

            AC_CHECK_FILE([/proc/sys/kernel/perf_event_paranoid],
                          [
                           with_paranoid=yes
                           AX_VAR_PUSHVALUE([CPPFLAGS],[-I$with_pfm_incdir/perfmon])
                           AC_CHECK_HEADER([perf_event.h])
                           AX_VAR_POPVALUE([CPPFLAGS])
                          ])
        else
            AC_DEFINE([HAVE_PERFMON_PFMLIB_MONTECITO_H],[1],[Montecito headers])
            AC_DEFINE([HAVE_PFM_GET_EVENT_DESCRIPTION],[1],[event description function])
            AC_DEFINE([HAVE_PFMLIB_OUTPUT_PFP_PMD_COUNT],[1],[new pfmlib_output_param_t])
        fi
        ;;
    solaris*,cpc,cpc*)
        AC_CHECK_TYPE([hrtime_t],
                      [AC_DEFINE([HAVE_HRTIME_T], [1], [Define if hrtime_t is defined in <sys/time.h>])],[],
                      [
                       #if HAVE_SYS_TIME_H
                       #include <sys/time.h>
                       #endif
                      ])

        AC_CHECK_HEADER([libcpc.h],
                        [AX_APPEND_FLAG([-lcpc],[LIBS])
                         AC_TRY_RUN([
                                     #include <stdlib.h>
                                     #include <libcpc.h>
                                     int main() {
                                         // Check for libcpc 2
                                         if(CPC_VER_CURRENT == 2)
                                             exit(0);
                                         exit(1);
                                     }
                                    ],
                                    [cpc_version=2],
                                    [cpc_version=0])
                        ],
                        [
                         AC_MSG_ERROR([libcpc is needed for running PAPI on Solaris])
                        ])

        if test "$cpc_version" != "$with_perf_lib_ver" ; then
            AC_MSG_ERROR([Wrong libcpc version. Rerun configure with --with-perf-lib-ver=$cpc_version.])
        fi

        if test "$PAPI_HOST_SYSTEM" = "ultra" ; then
            AC_CHECK_LIB([cpc], [cpc_take_sample],,
                         [AC_MSG_ERROR([libcpc.a is needed on Solaris, install SUNWcpc])])
        elif test "$PAPI_HOST_SYSTEM" = "niagara2"; then
            if test "$cpc_version" != "2"; then
                AC_MSG_ERROR([libcpc2 needed for Niagara 2])
            fi
        fi
        ;;
    cnk,bgpm,bgpm*)
        # FIXME: test BGPM library and headers
        ;;
    freebsd*,pmc,pmc*)
        # HWPMC driver is available for FreeBSD >= 6
        freebsd_ver=`uname -r | cut -d'.' -f1`
        if test "${freebsd_ver}" -lt 6 ; then
            AC_MSG_ERROR([PAPI requires FreeBSD 6 or greater])
        fi
        # Determine if HWPMC module is on the kernel
        dmesg | grep hwpmc 2> /dev/null > /dev/null
        if test "$?" != "0" ; then
            AC_MSG_ERROR([HWPMC module not found. (see INSTALL)])
        fi
        # Determine the number of counters
        AC_TRY_LINK([
                     #include <unistd.h>
                     #include <pmc.h>
                    ],
                    [int i = pmc_init();],
                    [pmc_init_linked="yes"],
                    [pmc_init_linked="no"])

        if test "${pmc_init_linked}" = "no" ; then
            AC_MSG_ERROR([Failed to link hwpmc example])
        fi
        # FIXME: this test is bogus. Return values can only be in the range
        # 1 - 255. If num pcm is out of range the return value will wrap
        # around giving a wrong result.
        AC_TRY_RUN([
                    #include <unistd.h>
                    #include <pmc.h>
                    int main() {
                        const struct pmc_cpuinfo *info;
                        if (pmc_init() < 0) return 0;
                        if (pmc_cpuinfo (&info) < 0) return 0;
                            return info->pm_npmc-1;
                    }
                   ],
                   [num_counters="0"],
                   [num_counters="$?"])

        if test "${num_counters}" = "0" ; then
            AC_MSG_ERROR([pmc_npmc info returned 0. Determine if the HWPMC module is loaded (see hwpmc(4))])
        fi
        with_freebsd_pmc_counters=$num_counters
        AC_SUBST([with_freebsd_pmc_counters])
        AC_CONFIG_FILES([src/freebsd/freebsd-config.h])
        ;;
    darwin*,*,*)
        # This is here only to make darwin configurations recognized as supported.
        ;;
    *,*,*)
        # other combinations of os, performance interface and library are not supported
        AC_MSG_ERROR([$PAPI_HOST_OS,$with_perf_api,$with_perf_lib-$with_perf_lib_ver configuration not supported.])
        ;;
esac

# RDPMC support
AC_ARG_ENABLE(perfevent_rdpmc,
              [AS_HELP_STRING([--enable-perfevent-rdpmc],
                              [Enable userspace rdpmc instruction on perf_event, default: yes])],,
              [
               AX_APPEND_FLAG([-DUSE_PERFEVENT_RDPMC=1],[CPPFLAGS])
              ])

AC_ARG_ENABLE(perf_event_uncore,
              [AS_HELP_STRING([--enable-perf-event-uncore],
                              [Enable perf_event_uncore component, default: yes])],,
              [enable_perf_event_uncore=yes])

##################################################
#       Final configuration for the build
##################################################
AC_MSG_NOTICE([Building PAPI for $PAPI_HOST_SYSTEM+$PAPI_HOST_OS with $with_perf_api+$with_perf_lib.])
with_cpu_vector=
case $PAPI_HOST_SYSTEM,$PAPI_HOST_OS,$with_perf_api,$with_perf_lib in
    bgp,cnk,bgpm,bgpm)
        oslock=$srcdir/src/bgp/linux-bgp-lock.h
        oscontext=$srcdir/src/bgp/linux-bgp-context.h
        with_cpu_component=$srcdir/src/bgp/linux-bgp.c
        with_cpu_vector=_bgp_vectors
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        AX_APPEND_FLAG([-gdwarf-2],[CFLAGS])
        AX_APPEND_FLAG([-Wall],[CFLAGS])
        AX_APPEND_FLAG([-L/bgsys/drivers/ppcfloor/runtime/SPI],[LDFLAGS])
        AX_APPEND_FLAG([-lSPI.cna],[LIBS])
        AX_APPEND_FLAG([-g],[FFLAGS])
        AX_APPEND_FLAG([-gdwarf-2],[FFLAGS])
        AX_APPEND_FLAG([-O2],[FFLAGS])
        AX_APPEND_FLAG([-Wall],[FFLAGS])

        AX_APPEND_FLAG([bgpm],[components])
        ;;
    bgq,cnk,bgpm,bgpm)
        oslock=$srcdir/src/bgp/linux-bgq-lock.h
        oscontext=$srcdir/src/bgp/linux-context.h
        with_cpu_component=$srcdir/src/bgq/linux-bgq.c
        with_cpu_vector=_bgq_vectors
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        AX_APPEND_FLAG([bgpm],[components])
        ;;
    power5,aix,pmapi,pmapi    \
    | power6,aix,pmapi,pmapi  \
    | power7,aix,pmapi,pmapi)
        oslock=$srcdir/src/aix/aix-lock.h
        oscontext=$srcdir/src/aix/aix-context.h
        with_cpu_component=$srcdir/src/aix/aix.c
        with_cpu_vector=_aix_vector
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        if test "x$bitmode" = "x" -o "$bitmode" = "64"; then
            # By default AIX enforces a limit on heap space
            #( limiting the heap to share the same 256MB memory segment as stack )
            # changing the max data paramater moves the heap off the stack's memory segment
            AX_APPEND_FLAG([-q64],[CC])
            AX_APPEND_FLAG([-bmaxdata:0x07000000000000],[CC])
        else
            # If the issue ever comes up, /dsa requires AIX v5.1 or higher
            # and the Large address-space model (-bmaxdata) requires v4.3 or later
            # see http://publib.boulder.ibm.com/infocenter/pseries/v5r3/topic/com.ibm.aix.genprogc/doc/genprogc/lrg_prg_support.htm#a179c11c5d
            AX_APPEND_FLAG([-bmaxdata:0x80000000/dsa],[CC])
        fi

        if test "$PAPI_HOST_SYSTEM" = "power5" ; then
            with_arch_events_h=$srcdir/src/components/perfctr_ppc/power5_events.h
            with_cpu_model=POWER5
            with_cpu_option=pwr5
        elif test "$PAPI_HOST_SYSTEM" = "power6" ; then
            with_arch_events_h=$srcdir/src/components/perfctr_ppc/power6_events.h
            with_cpu_model=POWER6
            with_cpu_option=pwr6
        elif test "$PAPI_HOST_SYSTEM" = "power7" ; then
            with_arch_events_h=$srcdir/src/components/perfctr_ppc/power7_events.h
            with_cpu_model=POWER7
            with_cpu_option=pwr7
        fi

        if test $with_debug != "yes"; then
            AX_APPEND_FLAG([-O3],[CFLAGS])
            AX_APPEND_FLAG([-qstrict],[CC])
        fi

        # FIXME: not sure where -q flags should go: CC or CFLAGS
        AX_APPEND_FLAG([-qenum=4],[CFLAGS])
        AX_APPEND_FLAG([-DNO_VARARG_MACRO],[CPPFLAGS])
        AX_APPEND_FLAG([-D_AIX],[CPPFLAGS])
        AX_APPEND_FLAG([-D_$(with_cpu_model)],[CPPFLAGS])
        AX_APPEND_FLAG([-DNEED_FFSLL],[CPPFLAGS])
        AX_APPEND_FLAG([-DARCH_EVTS],[CPPFLAGS])
        AX_APPEND_FLAG([-DCOMP_VECTOR=_ppc64_vectors],[CPPFLAGS])
        AX_APPEND_FLAG([-DSTATIC_PAPI_EVENTS_TABLE],[CPPFLAGS])
        AX_APPEND_FLAG([-qmaxmem=-1],[CFLAGS])
        AX_APPEND_FLAG([-qarch=$(with_cpu_option)],[CFLAGS])
        AX_APPEND_FLAG([-qtune=$(with_cpu_option)],[CFLAGS])
        AX_APPEND_FLAG([-qlanglvl=extended],[CFLAGS])
        AX_APPEND_FLAG([-WF,-D_$(with_cpu_model)],[FFLAGS])
        AX_APPEND_FLAG([-WF,-DARCH_EVTS],[FFLAGS])
        AX_APPEND_FLAG([-qsmp],[CFLAGS])
        AX_APPEND_FLAG([-qsmp=omp],[CFLAGS])
        AX_APPEND_FLAG([-G],[LDFLAGS]) # this might not be necessary with libtool
        AX_APPEND_FLAG([-bnoentry],[LDFLAGS]) # this might not be necessary with libtool

        AC_CHECK_PROGS([MPICC],[mpicc mpcc],[])
        F77=xlf

        AC_CONFIG_LINKS([src/powerpc_events.h:$with_arch_events_h])
        ;;
    *,freebsd,pmc,pmc)
        oslock="$srcdir/src/freebsd/freebsd-lock.h"
        oscontext="$srcdir/src/freebsd/freebsd-context.h"
        with_cpu_component="$srcdir/src/freebsd/freebsd.c"
        with_cpu_vector=_papi_freebsd_vector
        with_papi_events_csv="$srcdir/src/freebsd/freebsd_events.csv"
        ;;
    *,linux*,perf_event,pfm)
        oslock="$srcdir/src/linux/linux-lock.h"
        oscontext="$srcdir/src/linux/linux-context.h"
        with_cpu_component=$srcdir/src/dummy_cpu_component.c
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        AX_APPEND_FLAG([perf_event],[components])
        if test "$enable_perf_event_uncore" = "yes" ; then
            AX_APPEND_FLAG([perf_event_uncore],[components])
        fi
        ;;
    *,linux*,*,*) # linux generic
        oslock="$srcdir/src/linux/linux-lock.h"
        oscontext="$srcdir/src/linux/linux-context.h"
        with_cpu_component="$srcdir/src/linux/linux-generic.c"
        with_cpu_vector=_papi_dummy_vector
        with_papi_events_csv="$srcdir/src/papi_events.csv"
        ;;
    ultra,solaris*,cpc,cpc)
        oslock="$srcdir/src/solaris/solaris-lock.h"
        oscontext="$srcdir/src/solaris/solaris-context.h"
        with_cpu_component="$srcdir/src/solaris/solaris-ultra.c"
        with_cpu_vector=_solaris_vector
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        if test "$CC" = "gcc" ; then
            AX_APPEND_FLAG([-DNEED_FFSLL],[CPPFLAGS])
            AX_APPEND_FLAG([-O3],[CFLAGS])
            F77=g77
        else
            # Sun Workshop compilers: V5.0 and V6.0 R2
            AX_APPEND_FLAG([-DPAPI_NO_MEMORY_MANAGEMENT],[CPPFLAGS])
            AX_APPEND_FLAG([-xopenmp],[CFLAGS])
        fi
        ;;
    niagara2,solaris*,cpc,cpc)
        oslock="$srcdir/src/solaris/solaris-lock.h"
        oscontext="$srcdir/src/solaris/solaris-context.h"
        with_cpu_component="$srcdir/src/solaris/solaris-niagara2.c"
        with_cpu_vector=_niagara2_vector
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        F77=f90

        AX_APPEND_FLAG([-DNO_VARARG_MACRO],[CPPFLAGS])
        AX_APPEND_FLAG([-xopenmp],[CFLAGS])
        ;;
    *,darwin*,*,*)
        oslock="$srcdir/src/darwin/darwin-lock.h"
        oscontext="$srcdir/src/darwin/darwin-context.h"
        with_cpu_component="$srcdir/src/linux/linux-generic.c"
        with_papi_events_csv="$srcdir/src/papi_events.csv"

        AX_APPEND_FLAG([-DNEED_FFSLL],[CPPFLAGS])
        ;;
    *,*,*,*)
        AC_MSG_ERROR([$PAPI_HOST_SYSTEM,$PAPI_HOST_OS,$with_perf_api,$with_perf_lib configuration not supported.])
esac

AC_CONFIG_LINKS([src/os-lock.h:$oslock src/os-context.h:$oscontext])
AC_CONFIG_LINKS([src/cpu_component.c:$with_cpu_component])

AC_MSG_CHECKING([for PAPI event CSV filename to use])
AC_MSG_RESULT([$with_papi_events_csv])
AC_CONFIG_COMMANDS([gen_papi_events_table],
                   [sh $srcdir/src/papi_events_table.sh $with_papi_events_csv > src/papi_events_table.h],
                   [with_papi_events_csv=$with_papi_events_csv])

AC_CONFIG_COMMANDS_PRE([sh $srcdir/maint/gen_comp_makefile.sh $srcdir/src/components])

AX_APPEND_FLAG([-D_REENTRANT],[CPPFLAGS])
AX_APPEND_FLAG([-D_GNU_SOURCE],[CPPFLAGS])
if test "$CC" = "gcc"; then
    AX_APPEND_FLAG([-Wall],[CFLAGS])
fi
AX_APPEND_FLAG([-Dlinux],[FFLAGS])

if test "$F77" = "pgf77"; then
    AX_APPEND_FLAG([-Wall],[FFLAGS])
    AX_APPEND_FLAG([-Mextend],[FFLAGS])
elif test "$F77" = "ifort"; then
    AX_APPEND_FLAG([-warn all],[FFLAGS])
elif test "$F77" != "xlf"; then
    AX_APPEND_FLAG([-ffixed-line-length-132],[FFLAGS])
fi

if test "$CC" = "icc"; then
    AX_APPEND_FLAG([-qopenmp],[CFLAGS])
elif test "$CC" = "gcc" ; then
    AX_APPEND_FLAG([-fopenmp],[CFLAGS])
fi

# Check for additional hardware components to build in
AC_MSG_CHECKING([for components to build])
components_vector_list="$with_cpu_vector"
components_vector_pointer_list=""

AC_ARG_WITH([components],
            [AS_HELP_STRING([--with-components=@<:@LIST@:>@],
                            [Specify space separated LIST of components to build (e.g. cuda powercap)])],
            [AX_APPEND_FLAG([$withval],[components])])

for comp in $components; do
    # FIXME: if component is of the form 'path/comp' count the number of chars
    # up to '/' (not included) and remove them from the original string. Thus,
    # if comp="path/comp", subcomp="/comp". This behavior seems wrong.
    idx=`echo $comp | sed -n "s/\/.*//p" | wc -c`
    subcomp=`echo $comp | sed -E "s/^.{${idx}}//"`
    components_vector_list="_${subcomp}_vector, ${components_vector_list}"
    components_vector_pointer_list="&_${subcomp}_vector, ${components_vector_pointer_list}"
done

# Remove trailing ',' from components_vector_list and components_vector_pointer_list
components_vector_list=`echo $components_vector_list | sed -E 's/,$//'`
components_vector_pointer_list=`echo $components_vector_pointer_list | sed -E 's/,$//'`
components_vector_pointer_list="$components_vector_pointer_list, NULL"

AC_SUBST(components_vector_list)
AC_SUBST(components_vector_pointer_list)
AC_CONFIG_FILES([src/components_config.h])

# Conditionally build components
AM_CONDITIONAL([BUILD_APPIO],
               [test "x`echo $components | grep 'appio'`" != "x"])
AM_CONDITIONAL([BUILD_BGPM],
               [test "x`echo $components | grep 'bgpm'`" != "x"])
AM_CONDITIONAL([BUILD_CORETEMP],
               [test "x`echo $components | grep 'coretemp'`" != "x"])
AM_CONDITIONAL([BUILD_CORETEMP_FREEBSD],
               [test "x`echo $components | grep 'coretemp_freebsd'`" != "x"])
AM_CONDITIONAL([BUILD_CUDA],
               [test "x`echo $components | grep 'cuda'`" != "x"])
AM_CONDITIONAL([BUILD_EMON],
               [test "x`echo $components | grep 'emon'`" != "x"])
AM_CONDITIONAL([BUILD_EXAMPLE],
               [test "x`echo $components | grep 'example'`" != "x"])
AM_CONDITIONAL([BUILD_HOST_MICPOWER],
               [test "x`echo $components | grep 'host_micpower'`" != "x"])
AM_CONDITIONAL([BUILD_INFINIBAND],
               [test "x`echo $components | grep 'infiniband'`" != "x"])
AM_CONDITIONAL([BUILD_INTEL_GPU],
               [test "x`echo $components | grep 'intel_gpu'`" != "x"])
AM_CONDITIONAL([BUILD_IO],
               [test "x`echo $components | grep 'io'`" != "x"])
AM_CONDITIONAL([BUILD_LIBMSR],
               [test "x`echo $components | grep 'libmsr'`" != "x"])
AM_CONDITIONAL([BUILD_LMSENSORS],
               [test "x`echo $components | grep 'lmsensors'`" != "x"])
AM_CONDITIONAL([BUILD_LUSTRE],
               [test "x`echo $components | grep 'lustre'`" != "x"])
AM_CONDITIONAL([BUILD_MICPOWER],
               [test "x`echo $components | grep 'micpower'`" != "x"])
AM_CONDITIONAL([BUILD_MX],
               [test "x`echo $components | grep 'mx'`" != "x"])
AM_CONDITIONAL([BUILD_NET],
               [test "x`echo $components | grep 'net'`" != "x"])
AM_CONDITIONAL([BUILD_NVML],
               [test "x`echo $components | grep 'nvml'`" != "x"])
AM_CONDITIONAL([BUILD_PCP],
               [test "x`echo $components | grep 'pcp'`" != "x"])
AM_CONDITIONAL([BUILD_PERF_EVENT],
               [test "x`echo $components | grep 'perf_event'`" != "x"])
AM_CONDITIONAL([BUILD_PERF_EVENT_UNCORE],
               [test "x`echo $components | grep 'perf_event_uncore'`" != "x"])
AM_CONDITIONAL([BUILD_POWERCAP],
               [test "x`echo $components | grep 'powercap'`" != "x"])
AM_CONDITIONAL([BUILD_POWERCAP_PPC],
               [test "x`echo $components | grep 'powercap_ppc'`" != "x"])
AM_CONDITIONAL([BUILD_RAPL],
               [test "x`echo $components | grep 'rapl'`" != "x"])
AM_CONDITIONAL([BUILD_ROCM],
               [test "x`echo $components | grep 'rocm'`" != "x"])
AM_CONDITIONAL([BUILD_ROCM_SMI],
               [test "x`echo $components | grep 'rocm_smi'`" != "x"])
AM_CONDITIONAL([BUILD_SDE],
               [test "x`echo $components | grep 'sde'`" != "x"])
AM_CONDITIONAL([BUILD_SENSORS_PPC],
               [test "x`echo $components | grep 'sensors_ppc'`" != "x"])
AM_CONDITIONAL([BUILD_STEALTIME],
               [test "x`echo $components | grep 'stealtime'`" != "x"])
AM_CONDITIONAL([BUILD_VMWARE],
               [test "x`echo $components | grep 'vmware'`" != "x"])

AC_MSG_RESULT([$components])

# we can check for cupti headers regarless cuda component usage.
# if the component is enabled the corresponding Makefile.mk will
# be included and the source code for the component compiled,
# otherwise, even if the cupti headers are found they will not
# have any effect.
# FIXME: all config logic that is cuda specific should be moved to
# src/components/cuda/subconfigure.m4
if test "x`echo $components | grep 'cuda'`" != "x" ; then
    AX_VAR_PUSHVALUE([CPPFLAGS],[-I$PAPI_CUDA_ROOT/extras/CUPTI/include])
    AC_CHECK_HEADERS([cupti_profiler_target.h],
                     [AX_APPEND_FLAG([-DCUPTI_PROFILER=1],[CPPFLAGS])],
                     [AX_APPEND_FLAG([-DCUPTI_PROFILER=0],[CPPFLAGS])])
    AX_VAR_POPVALUE([CPPFLAGS])
fi

# Conditionally build OS support
AM_CONDITIONAL([BUILD_LINUX],
               [test "x`echo $PAPI_HOST_OS | grep 'linux'`" != "x"])
AM_CONDITIONAL([BUILD_SOLARIS],
               [test "x`echo $PAPI_HOST_OS | grep 'solaris'`" != "x"])
AM_CONDITIONAL([BUILD_FREEBSD],
               [test "x`echo $PAPI_HOST_OS | grep 'freebsd'`" != "x"])
AM_CONDITIONAL([BUILD_DARWIN],
               [test "x`echo $PAPI_HOST_OS | grep 'darwin'`" != "x"])
AM_CONDITIONAL([BUILD_AIX],
               [test "x`echo $PAPI_HOST_OS | grep 'aix'`" != "x"])
AM_CONDITIONAL([BUILD_BGP],
               [test "x`echo $PAPI_HOST_SYSTEM | grep 'bgp'`" != "x"])
AM_CONDITIONAL([BUILD_BGQ],
               [test "x`echo $PAPI_HOST_SYSTEM | grep 'bgq'`" != "x"])

# Conditionally build architecture support
AM_CONDITIONAL([BUILD_X86],
               [test "x`echo $host_cpu | grep 'x86'`" != "x"])

AC_SUBST(with_perf_lib_subdir)
AC_SUBST(tests)
AC_SUBST(PAPI_HOST_SYSTEM)

AC_MSG_NOTICE([$FILENAME will be included in the generated Makefile])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([doc/Doxyfile-common \
                 doc/Doxyfile-html   \
                 doc/Doxyfile-man1   \
                 doc/Doxyfile-man3])
AC_OUTPUT

if test "$with_paranoid" = "yes"; then
    with_paranoid_level=`cat /proc/sys/kernel/perf_event_paranoid`
    if test $with_paranoid_level -gt 2; then
        warning_text=`echo -e "\n
***************************************************************************
* Insufficient permissions for accessing any hardware counters.           *
* Your current paranoid level is $with_paranoid_level.                                       *
* Set /proc/sys/kernel/perf_event_paranoid to 2 (or less) or run as root. *
*                                                                         *
* Example:                                                                *
* sudo sh -c \"echo 2 > /proc/sys/kernel/perf_event_paranoid\"              *
***************************************************************************
"\ `
        AC_MSG_WARN([$warning_text])
    fi
fi

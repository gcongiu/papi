# Process this file with autoconf to produce a configure script.
# File: configure.in

# cross compile sample
# ARCH=mips CC=scgcc ./configure --with-arch=mips --host=mips64el-gentoo-linux-gnu- --with-ffsll --with-libpfm4 --with-perf-events --with-virtualtimer=times --with-walltimer=gettimeofday --with-tls=__thread --with-CPU=mips
# cross compiling should work differently...

AC_PREREQ(2.69)
AC_INIT(PAPI, 6.0.0.1, ptools-perfapi@icl.utk.edu)
AC_CONFIG_SRCDIR([papi.c])
AC_CONFIG_HEADER([config.h])

dnl######## AUTOCONF Macro definitions begin

dnl############################################################################
dnl# FIXME: the following definitions should be moved to confdb/aclocal_util.m4
dnl# when Automake support is added to the build system.
dnl############################################################################

dnl This is from autoconf-archive
dnl It appends a FLAG to a FLAG-VARIABLE only if the FLAG is not already in it.
dnl If the FLAGS-VARIABLE is empty it sets it to FLAG. If FLAG-VARIABLE is not
dnl specified the current language flag is used (e.g. CFLAGS)
AC_DEFUN([AX_APPEND_FLAG],
[dnl
AC_PREREQ(2.64)dnl for _AC_LANG_PREFIX and AS_VAR_SET_IF
AS_VAR_PUSHDEF([FLAGS], [m4_default($2,_AC_LANG_PREFIX[FLAGS])])
AS_VAR_SET_IF(FLAGS,[
    AS_CASE([" AS_VAR_GET(FLAGS) "],
        [*" $1 "*], [AC_RUN_LOG([: FLAGS already contains $1])],
        [
            AS_VAR_APPEND(FLAGS,[" $1"])
            AC_RUN_LOG([: FLAGS="$FLAGS"])
        ])
    ],
    [
        AS_VAR_SET(FLAGS,[$1])
        AC_RUN_LOG([: FLAGS="$FLAGS"])
    ])
AS_VAR_POPDEF([FLAGS])dnl
])dnl AX_APPEND_FLAG

dnl This is from autoconf-archive
dnl It stores a copy of variable_name''s value and assigns it to 'value' If no value
dnl is given, its original value is kept.
AC_DEFUN([AX_VAR_PUSHVALUE],[
    increment([$1_counter])

    AS_VAR_PUSHDEF([variable],[$1]) dnl
    AS_VAR_PUSHDEF([backup],[save_$1_]$1_counter) dnl

    AS_VAR_SET(backup,$variable) dnl
    AS_VAR_SET(variable,["]m4_default($2,$variable)["]) dnl

    AS_VAR_POPDEF([variable]) dnl
    AS_VAR_POPDEF([backup]) dnl
])dnl AX_PUSH_VAR

dnl This is from autoconf-archive
dnl It restores the variable_name''s value changed by AX_VAR_PUSHVALUE.
AC_DEFUN([AX_VAR_POPVALUE],[
    AS_VAR_PUSHDEF([variable],[$1]) dnl
    AS_VAR_PUSHDEF([backup],[save_$1_]$1_counter) dnl

    AS_VAR_SET(variable,$backup) dnl

    decrement([$1_counter])
    AS_VAR_POPDEF([variable]) dnl
    AS_VAR_POPDEF([backup]) dnl
])dnl AX_POP_VAR

m4_define([increment],[dnl
    m4_ifdef([$1],dnl
        [m4_define([$1],m4_incr($1))],dnl
        [m4_define([$1],[1])]dnl
    )dnl
])dnl

m4_define([decrement],[dnl
    m4_ifdef([$1],dnl
        [m4_if(m4_eval($1 > 0),
            [1],m4_define([$1],m4_decr($1)),dnl
            [m4_fatal([Missing call to AX_VAR_PUSHVALUE with var $1])]dnl
    )],dnl
    [m4_fatal([Missing call to AX_VAR_PUSHVALUE with var $1])])dnl
])dnl

dnl######## AUTOCONF Macro definition end

AC_MSG_CHECKING([for architecture])
AC_ARG_WITH([arch],
            [AS_HELP_STRING([--with-arch=<arch>],
                            [Specify architecture (uname -m)])],
            [arch=$withval],
            [arch=`uname -m`])
AC_MSG_RESULT([$arch])

AC_ARG_WITH([bitmode],
            [AS_HELP_STRING([--with-bitmode=<32,64>],
                            [Specify bit mode of library])],
            [bitmode=$withval])

AC_MSG_CHECKING([for OS])
AC_ARG_WITH([OS],
            [AS_HELP_STRING([--with-OS=<os>],
                            [Specify operating system])],
            [],
            [with_OS=default])

if test "$with_OS" != default; then
    OS=$with_OS
else
    OS=`uname | tr '[A-Z]' '[a-z]'`
    if test "$OS" = "SunOS" -o "$OS" = "sunos" ; then
        OS=solaris
    fi
fi
AC_MSG_RESULT([$OS])

AC_MSG_CHECKING([for OS version])
AC_ARG_WITH([OSVER],
            [AS_HELP_STRING([--with-OSVER=<ver>],
                            [Specify operating system version])],
            [],
            [with_OSVER=default])

if test "$with_OSVER" != "default"; then
    OSVER=$with_OSVER
else
    if test "$OS" != "bgp" -a "$OS" != "bgq" ; then
        OSVER=`uname -r`
    fi
fi
AC_MSG_RESULT([$OSVER])

AC_MSG_CHECKING([for perf_event workaround level])
AC_ARG_WITH([assumed_kernel],
            [AS_HELP_STRING([--with-assumed-kernel=<ver>],
                            [Assume kernel version is <ver> for purposes of workarounds])],
            [],
            [with_assumed_kernel=default])

if test "$with_assumed_kernel" != "default" ; then
    assumed_kernel=$with_assumed_kernel
    AX_APPEND_FLAG([-DASSUME_KERNEL=$with_assumed_kernel],[CPPFLAGS])
else
    assumed_kernel="autodetect"
fi
AC_MSG_RESULT([$assumed_kernel])

AC_MSG_CHECKING([for if MIC should be used])
AC_ARG_WITH([mic],
            [AS_HELP_STRING([--with-mic],
                            [To compile for Intel MIC])],
            [with_mic=yes],
            [with_mic=no])

if test "$with_mic" = "yes" ; then
    tls=__thread
    virtualtimer=cputime_id
    perf_events=yes
    walltimer=clock_realtime_hr
    ffsll=no
    cross_compiling=yes
    arch=k1om
fi

MIC=$with_mic

AC_MSG_RESULT([$MIC])
AC_SUBST(MIC)

AC_MSG_CHECKING([for if NEC should be used])
AC_ARG_WITH([nec],
            [AS_HELP_STRING([]--with-nec,
                            [To compile for NEC])],
            [with_NEC=yes],
            [with_NEC=no])

if test "$with_NEC" = "yes" ; then
    tls=__thread
    cross_compiling=yes
    ffsll=/opt/nec/ve/lib/libc-2.21.so
    virtualtimer=cputime_id
    walltimer=clock_realtime_hr
    CC=ncc
    CC_COMMON_NAME=ncc
fi

NEC=$with_nec

AC_MSG_RESULT([$NEC])
AC_SUBST(NEC)


AX_APPEND_FLAG([-g],[CFLAGS])

#If not set, set FFLAGS to null to prevent AC_PROG_F77 from defaulting it to -g -O2
if test "x$FFLAGS" = "x"; then
    FFLAGS=""
fi

OPTFLAGS="-O2"
TOPTFLAGS="-O1"
AC_PROG_CC([xlc icc gcc cc])
AC_PROG_F77([xlf ifort gfortran f95 f90 f77])
if test "x$F77" = "x"; then
   F77=
fi
AC_CHECK_PROG([MPICC], [mpicc], [mpicc], [])

# Lets figure out what CC actually is...
# Used in later checks to set compiler specific options
if `$CC --version 2>&1 | grep '^ncc (NCC)' >/dev/null 2>&1` ; then
    CC_COMMON_NAME="ncc"
elif `$CC -V 2>&1 | grep '^Intel(R) C' >/dev/null 2>&1` ; then
    CC_COMMON_NAME="icc"
    if test "$MPICC" = "mpicc"; then
        # Check if mpiicc is available
        MPI_COMPILER_CHECK=`$MPICC -V 2>&1 | grep '^Intel(R) C'`
        if test "x$MPI_COMPILER_CHECK" = "x"; then
            AC_MSG_WARN([$MPICC is using a different compiler than $CC. MPI tests disabled.])
            NO_MPI_TESTS=yes
        fi
    fi
elif `$CC -v 2>&1 | grep 'gcc version' >/dev/null 2>&1` ; then
    CC_COMMON_NAME="gcc"
    if test "$MPICC" = "mpicc"; then
        MPI_COMPILER_CHECK=`$MPICC -v 2>&1 | grep 'gcc version'`
        if test "x$MPI_COMPILER_CHECK" = "x"; then
            AC_MSG_WARN([$MPICC is using a different compiler than $CC. MPI tests disabled.])
            NO_MPI_TESTS=yes
        fi
    fi
elif `$CC -qversion 2>&1 | grep 'IBM XL C' >/dev/null 2>&1`; then
    CC_COMMON_NAME="xlc"
    if test "$MPICC" = "mpicc"; then
        MPI_COMPILER_CHECK=`$MPICC -qversion 2>&1 | grep 'IBM XL C'`
        if test "x$MPI_COMPILER_CHECK" = "x"; then
            AC_MSG_WARN([$MPICC is using a different compiler than $CC. MPI tests disabled.])
            NO_MPI_TESTS=yes
        fi
    fi
else
    CC_COMMON_NAME="unknown"
fi

#prevent icc warnings about overriding optimization settings set by AC_PROG_CC
# remark #869: parameter was never referenced 
# remark #271: trailing comma is nonstandard
if test "$CC_COMMON_NAME" = "icc"; then
    AX_APPEND_FLAG([-diag-disable 188,869,271],[CFLAGS])
    if test "$MIC" = "yes"; then
        AX_APPEND_FLAG([-mmic],[CC])
        AX_APPEND_FLAG([-fPIC],[CC])
    fi
fi

if test "$F77" = "ifort" -a "$MIC" = "yes"; then
    AX_APPEND_FLAG([-mmic],[F77])
    AX_APPEND_FLAG([-fPIC],[F77])
fi

AC_PROG_AWK
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
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
    LRT=""
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
        AX_APPEND_FLAG([-lrt],[LRT])
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
            AX_APPEND_FLAG([-lrt -lpthread],[LRT])
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
    LDL=""
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
        LDL="-ldl"
    else
        AC_MSG_ERROR([cannot find dlopen and dlerror symbols neither in the base system libraries nor in -ldl])
    fi
fi

AC_SUBST(LDL)

if test "$OS" = "CLE"; then
    virtualtimer=times
    tls=__thread
    walltimer=cycle
    ffsll=yes
    cross_compiling=yes
    STATIC="-static"
elif test "$OS" = "bgp"; then
    CC=powerpc-bgp-linux-gcc
    F77=powerpc-bgp-linux-gfortran
    walltimer=cycle
    virtualtimer=perfctr
    tls=no
    ffsll=yes
    cross_compiling=yes
elif test "$OS" = "bgq"; then
    AC_ARG_WITH([bgpm_installdir],
                [AS_HELP_STRING([--with-bgpm_installdir=<path>],
                                [Specify the installation path of BGPM])],
                [],
                [AC_MSG_ERROR([BGQ CPU component requires installation path of BGPM (see --with-bgpm_installdir)])])
    if test "x$with_bgpm_installdir" != "x" ; then
        BGPM_INSTALL_DIR=$with_bgpm_installdir
        AX_APPEND_FLAG([-I$with_bgpm_installdir],[CPPFLAGS])
    fi
    bitmode=64
    tls=no
elif test "$OS" = "linux"; then
    if test "$arch" = "ppc64" -o "$arch" = "x86_64"; then
        if test "$bitmode" = "64" -a "$libdir" = '${exec_prefix}/lib'; then
            libdir='${exec_prefix}/lib64'
        fi
    fi
elif test "$OS" = "solaris"; then
    AC_CHECK_TYPE([hrtime_t],
                  [AC_DEFINE(HAVE_HRTIME_T, 1, [Define if hrtime_t is defined in <sys/time.h>])],[],
                  [
                   #if HAVE_SYS_TIME_H
                   #include <sys/time.h>
                   #endif
                  ])
    # FIXME: setting the AR tool manually seems error prone
    # Use AC_CHECK_TOOL instead?
    if test "x$AR" = "x"; then
        AR=/usr/ccs/bin/ar
    fi
fi

if test "x$AR" = "x"; then
    AR=ar
fi

# FIXME: setting up cross compilation environment manually is not autoconf compliant
# and probably more error prone
if test "$cross_compiling" = "yes" ; then
    AC_MSG_CHECKING([for native compiler for header generation])
    AC_ARG_WITH([nativecc],
                [AS_HELP_STRING([--with-nativecc=<path>],
                                [Specify native C compiler for header generation])],
                [nativecc=$withval],
                [nativecc=gcc])
    AC_MSG_RESULT([$nativecc])
fi

AC_MSG_CHECKING([for tests])
AC_ARG_WITH([tests],
            [AS_HELP_STRING([--with-tests=<"ctests ftests mpitests", no>],
                            [Specify which tests to run on install, or "no" tests (default: all available tests)])],
            [],
            [with_tests="ctests ftests mpitests"])

if test "$with_tests" = "no" ; then
    AC_MSG_RESULT([$with_tests])
    tests=
    NO_MPI_TESTS=yes
else
    # mpitests is not a target
    tmp_tests=
    mpi_tests=no

    if test "x`echo $with_tests | grep 'ctests'`" != "x" ; then
        tmp_tests+="ctests "
    fi

    if test "x`echo $with_tests | grep 'ftests'`" != "x" ; then
        tmp_tests+="ftests "
    fi

    if test "x`echo $with_tests | grep 'mpitests'`" != "x" ; then
        # we already checked if mpicc is working
        # FIXME: there must be a better way of doing this
        if test "x$MPICC" != "x" ; then
            if test "x$NO_MPI_TESTS" = "x" ; then
                mpi_tests=yes
                # mpitests only works together with ctests
                if test "x$tmp_tests" = "x" ; then
                    AC_MSG_RESULT([no])
                else
                    AC_MSG_RESULT([$tmp_tests mpitests])
                fi
            fi
        else
            NO_MPI_TESTS=yes
            if test "x$tmp_tests" = "x" ; then
                AC_MSG_RESULT([no])
            else
                AC_MSG_RESULT([$tmp_tests])
            fi
        fi
    fi
    # do not list mpi_tests for makefile target
    tests=$tmp_tests

    # mpi_tests is not listed by the user
    if test "$mpi_tests" = "no"; then
        NO_MPI_TESTS=yes
    fi
fi


AC_MSG_CHECKING([for debug build])
AC_ARG_WITH([debug],
            [AS_HELP_STRING([--with-debug=<yes,memory,no>],
                            [Build a debug version, debug version plus memory tracker or none])],
            [],[with_debug=no])

if test "$with_debug" != "no" ; then
    if test "$with_debug" = "yes"; then
        if test "$CC_COMMON_NAME" = "gcc"; then
            AX_APPEND_FLAG([-g3],[CFLAGS])
        fi
        AX_APPEND_FLAG([-O0],[CFLAGS])
        AX_APPEND_FLAG([-DDEBUG -DPAPI_NO_MEMORY_MANAGEMENT],[CPPFLAGS])
    elif test "$with_debug" = "memory"; then
        if test "$CC_COMMON_NAME" = "gcc"; then
            AX_APPEND_FLAG([-g3],[CFLAGS])
        fi
        AX_APPEND_FLAG([-O0],[CFLAGS])
        AX_APPEND_FLAG([-DDEBUG],[CPPFLAGS])
    fi
else
    AX_APPEND_FLAG([-DPAPI_NO_MEMORY_MANAGEMENT],[CPPFLAGS])
fi
debug=$with_debug
AC_MSG_RESULT([$debug])

if test "$CC_COMMON_NAME" = "gcc"; then
    gcc_version=`gcc -v 2>&1 | tail -n 1 | awk '{printf $3}'`
    major=`echo $gcc_version | sed 's/\([[^.]][[^.]]*\).*/\1/'`
    minor=`echo $gcc_version | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
    if (test "$major" -ge 4 || test "$major" = 3 -a "$minor" -ge 4); then
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
                      [HAVE_NO_OVERRIDE_INIT=1],
                      [HAVE_NO_OVERRIDE_INIT=0])
    AX_VAR_POPVALUE([CFLAGS])
    AC_MSG_RESULT([$HAVE_NO_OVERRIDE_INIT])
fi

AC_MSG_CHECKING([for CPU type])
AC_ARG_WITH([CPU],
            [AS_HELP_STRING([--with-CPU=<cpu>],
                            [Specify CPU type])],
            [],
            [with_CPU=default])

if test "$with_CPU" != "default" ; then
    CPU=$with_CPU
    case "$CPU" in
        core|core2|i7|atom|p4|p3|opteron|athlon)
            AX_APPEND_FLAG([x86_cpuid_info.c],[MISCSRCS])
    esac
else
    case "$OS" in
        aix)
            CPU="`/usr/sbin/lsattr -E -l proc0 | grep type | cut -d '_' -f 2 | cut -d ' ' -f 1 | tr '[A-Z]' '[a-z]'`"
            if test "$CPU" = ""; then
                CPU="`/usr/sbin/lsattr -E -l proc1 | grep type | cut -d '_' -f 2 | cut -d ' ' -f 1 | tr '[A-Z]' '[a-z]'`"
            fi ;;
        nec)
            family=nec
            ;;
        freebsd)
            family=`uname -m`

            if test "$family" = "amd64"; then
                AX_APPEND_FLAG([x86_cpuid_info.c],[MISCSRCS])
            elif test "$family" = "i386"; then
                AX_APPEND_FLAG([x86_cpuid_info.c],[MISCSRCS])
            fi ;;
        darwin)
            family=`uname -m`

            AX_APPEND_FLAG([x86_cpuid_info.c],[MISCSRCS])
            ;;
        linux)
            family=`uname -m`

            if test "$family" = "x86_64"; then
                AX_APPEND_FLAG([x86_cpuid_info.c],[MISCSRCS])
                CPU="x86"
            elif test "$family" = "i686"; then
                AX_APPEND_FLAG([x86_cpuid_info.c],[MISCSRCS])
                CPU="x86"
            elif test "$family" = "ppc64"; then
                CPU_info="`cat /proc/cpuinfo | grep cpu | cut -d: -f2 | cut -d' ' -f2 | sed '2,$d'`"
                case "$CPU_info" in
                    PPC970*) CPU="PPC970";;
                    POWER5)  CPU="POWER5";;
                    POWER5+) CPU="POWER5+";;
                    POWER6)  CPU="POWER6";;
                    POWER7)  CPU="POWER7";;
                esac
            fi ;;
        solaris)
            AC_CHECK_HEADER([libcpc.h],
                            [AX_APPEND_FLAG([-lcpc],[CFLAGS])
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
                            [AC_MSG_ERROR([libcpc is needed for running PAPI on Solaris])])
            processor=`uname -p`
            machinetype=`uname -m`
            if test "$processor" = "sparc"; then
                if test "$machinetype" = "sun4u"; then
                    CPU=ultra
                    AC_CHECK_LIB([cpc], [cpc_take_sample], [],
                                 [AC_MSG_ERROR([libcpc.a is needed on Solaris, install SUNWcpc])])
                elif test "$machinetype" = "sun4v"; then
                    CPU=niagara2
                    if test "$cpc_version" != "2"; then
                        AC_MSG_ERROR([libcpc2 needed for Niagara 2])
                    fi
                else
                    AC_MSG_ERROR([$machinetype not supported])
                fi
            else
                AC_MSG_ERROR([Only SPARC processors are supported on Solaris])
            fi ;;
        bgp)
            CPU=bgp ;;
        bgq)
            CPU=bgq ;;
    esac
fi

AC_MSG_RESULT([$CPU])
AC_DEFINE_UNQUOTED([CPU],[$CPU],[cpu type])

# First set pthread-mutexes based on arch
case $arch in
    aarch64|arm*|parisc*)
        pthread_mutexes=yes
        AX_APPEND_FLAG([-DUSE_PTHREAD_MUTEXES],[CPPFLAGS])
        echo "forcing use of pthread mutexes... " >&6
        ;;
esac


AC_ARG_WITH([pthread-mutexes],
            [AS_HELP_STRING([--with-pthread-mutexes],
                            [Specify use of pthread mutexes rather than custom PAPI locks])],
            [pthread_mutexes=yes],
            [])

if test "x$pthread_mutexes" = "xyes" ; then
    AX_APPEND_FLAG([-DUSE_PTHREAD_MUTEXES],[CPPFLAGS])
fi

AC_ARG_WITH([ffsll],
            [AS_HELP_STRING([--with-ffsll],
                            [Specify use of the ffsll() function])],
            [ffsll=$withval],
            [ffsll=no])

if test "$ffsll" = "no" ; then
    if test "$cross_compiling" = "yes" ; then
        AC_MSG_ERROR([ffsll must be specified for cross compile])
    fi
    didcheck=1
    AC_CHECK_FUNC(ffsll,[ffsll=yes],[ffsll=no])

    if test "$ffsll" = "yes" ; then
        AC_DEFINE(HAVE_FFSLL, 1, This platform has the ffsll() function)
    fi
    if test "$didcheck" != "1"; then
        AC_MSG_CHECKING([for ffsll])
        if test "$ffsll" = "yes" ; then
            AC_DEFINE(HAVE_FFSLL, 1, This platform has the ffsll() function)
        fi
        AC_MSG_RESULT([$ffsll])
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
    AC_DEFINE(HAVE_GETTID, 1, [Full gettid function])
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
        AC_DEFINE(HAVE_SYSCALL_GETTID, 1, [gettid syscall function])
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
    fi
fi

AC_ARG_WITH([walltimer],
            [AS_HELP_STRING([--with-walltimer=<gettimeofday,mmtimer,clock_realtime_hr,clock_realtime,cycle>],
                            [Specify realtime timer])],
            [walltimer=$withval],
            [walltimer=default])

if test "$walltimer" = "default" ; then
    if test "$cross_compiling" = "yes" ; then
        AC_MSG_ERROR([walltimer must be specified for cross compile])
    fi
    AC_MSG_CHECKING([for working MMTIMER])
    AC_TRY_RUN([
                #include <unistd.h>
                #include <fcntl.h>
                #include <errno.h>
                #include <stdlib.h>
                #include <sys/ioctl.h>
                #include <linux/mmtimer.h>
                #ifndef MMTIMER_FULLNAME
                #define MMTIMER_FULLNAME "/dev/mmtimer"
                #endif
                int main() {
                    int offset;
                    int fd;
                    if((fd = open(MMTIMER_FULLNAME, O_RDONLY)) == -1)
                        exit(1);
                    if ((offset = ioctl(fd, MMTIMER_GETOFFSET, 0)) < 0)
                        exit(1);
                    close(fd);
                    exit(0);
                }
               ],
               [walltimer=mmtimer],
               [])

    if test "x$walltimer" = "xmmtimer" ; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
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
                   [walltimer=clock_realtime_hr],
                   [])
        if test "x$walltimer" = "xclock_realtime_hr" ; then
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
                       [walltimer="clock_realtime"
                        AC_MSG_RESULT([yes])],
                       [walltimer="cycle"
                        AC_MSG_RESULT([no])])
        fi
    fi
fi

AC_MSG_CHECKING([for which real time clock to use])
if test "$walltimer" = "gettimeofday"; then
    AC_DEFINE(HAVE_GETTIMEOFDAY, 1, [Normal gettimeofday timer])
elif test "$walltimer" = "mmtimer"; then
    AC_DEFINE(HAVE_MMTIMER, 1, [Altix memory mapped global cycle counter])
    altix="-DALTIX"
elif test "$walltimer" = "clock_realtime_hr"; then
    AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [POSIX 1b clock])
    AC_DEFINE(HAVE_CLOCK_GETTIME_REALTIME_HR, 1, [POSIX 1b realtime HR clock])
elif test "$walltimer" = "clock_realtime"; then
    AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [POSIX 1b clock])
    AC_DEFINE(HAVE_CLOCK_GETTIME_REALTIME, 1, [POSIX 1b realtime clock])
elif test "$walltimer" = "cycle"; then
    AC_DEFINE(HAVE_CYCLE, 1, [Native access to a hardware cycle counter])
else
    AC_MSG_ERROR([Unknown value for walltimer])
fi
AC_MSG_RESULT([$walltimer])

AX_VAR_PUSHVALUE([LIBS],[])
AX_VAR_PUSHVALUE([LDFLAGS],[])
AX_VAR_PUSHVALUE([CFLAGS],[-pthread])

AC_ARG_WITH([tls],
            [AS_HELP_STRING([--with-tls=<keyword>],
                            [This platform supports thread local storage with a keyword])],
            [tls=$withval],
            [tls=default])

if test "$tls" = "default" ; then
    if test "$cross_compiling" = "yes" ; then
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
               [AC_MSG_RESULT([yes])
                tls="__thread"],
               [AC_MSG_RESULT([no])
                tls="no"])
fi

if test "$OS" = "linux"; then
    if test "x$tls" = "x__thread"; then
        # On some linux distributions, TLS works in executables, but linking against
        # a shared library containing TLS fails with: undefined reference to '__tls_get_addr'
        AX_VAR_PUSHVALUE([CFLAGS],[-fPIC --shared])
        AC_TRY_LINK([],
                    [static __thread int foo; void main() { foo = 5; }],
                    [],[AC_MSG_WARN([Disabling usage of __thread.]); tls=no])
        AX_VAR_POPVALUE([CFLAGS])
    fi
fi

AC_MSG_CHECKING([for high performance thread local storage])
if test "$tls" = "no"; then
    NOTLS="-DNO_TLS"
elif test "x$tls" != "x"; then
    if test "$tls" = "yes"; then
        tls="__thread"
    fi
    NOTLS="-DUSE_COMPILER_TLS"
    AC_DEFINE_UNQUOTED(HAVE_THREAD_LOCAL_STORAGE,$tls,[Keyword for per-thread variables])
fi
AC_MSG_RESULT([$tls])

AC_ARG_WITH([virtualtimer],
            [AS_HELP_STRING([--with-virtualtimer=<clock_thread_cputime_id,times,proc,getrusage,perfctr>],
                            [Specify per-thread virtual timer])],
            [virtualtimer=$withval],
            [virtualtimer=default])

if test "$virtualtimer" = "default" ; then
    if test "$cross_compiling" = "yes" ; then
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
               [virtualtimer=clock_thread_cputime_id],
               [])

    if test "x$virtualtimer" = "xclock_thread_cputime_id" ; then
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
                   [AC_MSG_RESULT([yes])
                    virtualtimer="times"],
                   [AC_MSG_RESULT([no])
                    virtualtimer="default"])
    fi
fi

AX_VAR_POPVALUE([LDFLAGS])
AX_VAR_POPVALUE([CFLAGS])
AX_VAR_POPVALUE([LIBS])

AC_MSG_CHECKING([for which virtual timer to use])
case "$virtualtimer" in
    times)
        AC_DEFINE(HAVE_PER_THREAD_TIMES, 1, [Working per thread timer]) ;;
    getrusage)
        AC_DEFINE(HAVE_PER_THREAD_GETRUSAGE, 1, [Working per thread getrusage]) ;;
    clock_thread_cputime_id)
        AC_DEFINE(HAVE_CLOCK_GETTIME_THREAD, CLOCK_THREAD_CPUTIME_ID, [POSIX 1b per-thread clock]) ;;
    proc|default|perfctr)
        AC_DEFINE(USE_PROC_PTTIMER, 1, [Use /proc for per-thread times])
        AC_DEFINE(USE_PERFCTR_PTTIMER, 1, [Use the perfctr virtual TSC for per-thread times]) ;;
esac
AC_MSG_RESULT([$virtualtimer])

if test "$OS" = "aix"; then
    AC_ARG_WITH([pmapi],
                [AS_HELP_STRING([--with-pmapi=<path>],
                                [Specify path of pmapi on aix system])],
                [PMAPI=$withval],
                [PMAPI="/usr/pmapi"])

    # TODO: are we sure we want to overwrite existing LIBS variable?
    LIBS="-L$PMAPI/lib -lpmapi"
    AX_APPEND_FLAG([-I$PMAPI/include],[CPPFLAGS])
    AC_CHECK_LIB([pmapi], [pm_initialize],
                 [PMINIT="-DPM_INITIALIZE"],
                 [AC_CHECK_LIB([pmapi], [pm_init],
                               [PMINIT="-DPM_INIT"],
                               [AC_MSG_ERROR([libpmapi.a not found, rerun configure with different flags])])])
fi

AC_MSG_CHECKING([for static user preset events])
AC_ARG_WITH([static_user_events],
            [AS_HELP_STRING([--with-static-user-events],
                            [Build with a static user events file.])],
            [STATIC_USER_EVENTS=$withval],
            [STATIC_USER_EVENTS=no])

if test "$STATIC_USER_EVENTS" = "yes"; then
    AX_APPEND_FLAG([-DSTATIC_USER_EVENTS],[CPPFLAGS])
fi
AC_MSG_RESULT([$STATIC_USER_EVENTS])

AC_MSG_CHECKING([for static PAPI preset events])
AC_ARG_WITH([static_papi_events],
            [AS_HELP_STRING([--with-static-papi-events],
                            [Build with a static papi events file.])],
            [STATIC_PAPI_EVENTS=$withval],
            [STATIC_PAPI_EVENTS=yes])

if test "$STATIC_PAPI_EVENTS" = "yes"; then
    AX_APPEND_FLAG([-DSTATIC_PAPI_EVENTS_TABLE],[CPPFLAGS])
fi
AC_MSG_RESULT([$STATIC_PAPI_EVENTS])

AC_MSG_CHECKING([for whether to build static library])
AC_ARG_WITH([static_lib],
            [AS_HELP_STRING([--with-static-lib=<yes,no>],
                            [Build a static library])],
            [static_lib=$withval],
            [static_lib=yes])
AC_MSG_RESULT([$static_lib])

AC_MSG_CHECKING([for whether to build shared library])
AC_ARG_WITH([shared_lib],
            [AS_HELP_STRING([--with-shared-lib=<yes,no>],
                            [Build a shared library])],
            [shared_lib=$withval],
            [shared_lib=yes])
AC_MSG_RESULT([$shared_lib])

if test "$shared_lib" = "no" -a "$static_lib" = "no"; then
    AC_MSG_ERROR([Both shared and static libs are disabled])
fi

if test "$shared_lib" = "yes"; then
    AX_APPEND_FLAG([shared],[papiLIBS])
fi
if test "$static_lib" = "yes"; then
    AX_APPEND_FLAG([static],[papiLIBS])
fi

if test "$shared_lib" = "no" -a "$static_lib" = "yes"; then
    NO_MPI_TESTS="yes"
fi

AC_MSG_CHECKING([for static compile of tests and utilities])
AC_ARG_WITH([static_tools],
            [AS_HELP_STRING([--with-static-tools],
                            [Specify static compile of tests and utilities])],
            [STATIC="-static"
             AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no])])

# Disable LDL AND SDE for static builds
if test "$STATIC" = "-static"; then
    LDL=""
fi

AC_MSG_CHECKING([for linking with papi shared library of tests and utilities])
AC_ARG_WITH([shlib_tools],
            [AS_HELP_STRING([--with-shlib-tools],
                            [Specify linking with papi library of tests and utilities])],
            [shlib_tools=yes
             AC_MSG_RESULT([yes])],
            [shlib_tools=no
             AC_MSG_RESULT([no])])

if test "$static_lib" = "no"; then
    shlib_tools=yes
fi

if test "$static_lib" = "no" -a "$shlib_tools" = "no"; then
    AC_MSG_ERROR([Building tests and utilities static but no static papi library to be built])
fi

if test "$shlib_tools" = "yes"; then
    if test "$shared_lib" != "yes"; then 
        AC_MSG_ERROR([Building static but specified shared linking for tests and utilities])
    fi
    if test "$STATIC" = "-static"; then
        AC_MSG_ERROR([Building shared but specified static linking])
    fi
    LINKLIB='$(SHLIB)'
    # Set rpath and runpath to find libpfm.so and libpapi.so if not specified via LD_LIBRARY_PATH. The search path at runtime can be overriden by LD_LIBRARY_PATH.
    AX_APPEND_FLAG([-Wl,-rpath=$PWD/libpfm4/lib:$PWD,--enable-new-dtags],[LDFLAGS])
elif test "$shlib_tools" = "no"; then
    if test "$static_lib" != "yes"; then
        AC_MSG_ERROR([Building shared but specified static linking for tests and utilities])
    fi
    LINKLIB='$(LIBRARY)' 
fi

##################################################
#          perfctr
##################################################
perfctr=0
force_perfctr=no

AC_ARG_WITH([perfctr],
            [AS_HELP_STRING([--with-perfctr],
                            [Specify perfctr as the performance interface])],
            [
             force_perfctr=yes
             user_specified_interface=perfctr
             if test "$arch" != "ppc64"; then
                 perfctr=6
             else
                 perfctr=7
             fi
            ],
            [perfctr=0])

if test "$cross_compiling" = "no" ; then
    AC_CHECK_FILE(/sys/class/perfctr,[perfctr=7],[AC_CHECK_FILE(/dev/perfctr,[perfctr=6])])
fi

if test "$perfctr" != 0; then
    pfm_incdir="libpfm-3.y/include/"
    AC_MSG_CHECKING([for perfctr version])
    if test "$perfctr" = 7 -a "$arch" != "ppc64"; then
        AC_MSG_ERROR([Perfctr 2.7.x only works on PPC64 machines. Patch kernel with 2.6.x.])
    fi
    AC_MSG_RESULT([2.$perfctr])
fi

AC_ARG_WITH([perfctr_root],
            [AS_HELP_STRING([--with-perfctr-root=<path>],
                            [Specify path to source tree (for use by developers only)])],
            [perfctr_root=$withval
             user_specified_interface=perfctr])

AC_ARG_WITH([perfctr_prefix],
            [AS_HELP_STRING([--with-perfctr-prefix=<path>],
                            [Specify prefix to installed perfctr distribution])],
            [perfctr_prefix=$withval
             user_specified_interface=perfctr])

AC_ARG_WITH([perfctr_incdir],
            [AS_HELP_STRING([--with-perfctr-incdir=<path>],
                            [Specify directory of perfctr header files in non-standard location])],
           [perfctr_incdir=$withval
            user_specified_interface=perfctr])

AC_ARG_WITH([perfctr_libdir],
            [AS_HELP_STRING([--with-perfctr-libdir=<path>],
                            [Specify directory of perfctr library in non-standard location])],
            [perfctr_libdir=$withval
             user_specified_interface=perfctr])

if test "$perfctr" != 0; then
    dotest=0
    if test "x$perfctr_root" != "x"; then
        # TODO: are we sure we want to overwrite existing LIBS variable?
        LIBS="-L$perfctr_root/usr.lib -lperfctr"
        AX_APPEND_FLAG([-I$perfctr_root/usr.lib -I$perfctr_root/linux/include],[CPPFLAGS])
        dotest=1
    elif test "x$perfctr_prefix" != "x"; then
        # TODO: are we sure we want to overwrite existing LIBS variable?
        LIBS="-L$perfctr_prefix/lib -lperfctr"
        AX_APPEND_FLAG([-I$perfctr_prefix/include],[CPPFLAGS])
        perfctr_libdir="$perfctr_prefix/lib"
        perfctr_incdir="$perfctr_prefix/include"
        dotest=1
    else
        if test "x$perfctr_libdir" != "x"; then
            # TODO: are we sure we want to overwrite existing LIBS variable?
            LIBS="-L$perfctr_libdir -lperfctr"
            dotest=1
        fi
    fi
    if test "x$perfctr_incdir" != "x"; then
        AX_APPEND_FLAG([-I$perfctr_incdir],[CPPFLAGS])
        dotest=1
    fi
fi

if test "$dotest" = 1; then
    AC_CHECK_LIB([perfctr], [vperfctr_open])
    if test "$ac_cv_lib_perfctr_vperfctr_open" = "yes" ; then
        AC_CHECK_HEADERS([libperfctr.h])
        if test "$ac_cv_header_libperfctr_h" = "yes" ; then
            AX_VAR_PUSHVALUE([CFLAGS],[-static])
            AC_TRY_RUN([
                        #include <stdlib.h>
                        #include "libperfctr.h"
                        int main() {
                            if ((PERFCTR_ABI_VERSION >> 24) != 5) 
                                exit(1);
                            exit(0);
                        }
                       ],
                       [perfctr=6],
                       [perfctr=7])
            AX_VAR_POPVALUE([CFLAGS])
        else
            AC_MSG_ERROR([libperfctr.h not found, rerun configure with different flags])
        fi
    else
        AC_MSG_ERROR([libperfctr.a not found, rerun configure with different flags])
    fi
else
    AC_DEFINE(HAVE_LIBPERFCTR_H,1,[perfctr header file])
fi

user_specified_interface=no
##################################################
#          perfnec
##################################################
force_perfnec=no
perfnec=0

AC_ARG_WITH([perfnec],
            [AS_HELP_STRING([--with-perfnec],
                            [Specify perf_nec as the performance interface])],
            [
             perfnec=$withval
             user_specified_interface=perfnec
             force_perfnec=yes
             pfm_root=libperfnec
             pfm_incdir="libperfnec/include"
            ],
            [
             perfnec=0
             if test "$perfnec" != 0; then
                 pfm_incdir="libpperfnec/include"
             fi
            ])


force_pfm_incdir=no

##################################################
#          perfmon
##################################################
old_pfmv2=n
perfmon=0
perfmon2=no
force_perfmon2=no

AC_ARG_WITH([perfmon],
            [AS_HELP_STRING([--with-perfmon=<x.y>],
                            [Specify perfmon as the performance interface and specify version])],
            [perfmon=$withval],
            [perfmon=default])

if test "$perfmon" != "default" ; then
    user_specified_interface=perfmon
    force_perfmon2=yes
    pfm_incdir="libpfm-3.y/include"
    perfmon=`echo ${perfmon} | sed 's/^[ \t]*//;s/[ \t]*$//'`
    perfmon=`echo ${perfmon} | grep -e '[[1-9]]\.[[0-9]][[0-9]]*'`
    if test "x$perfmon" = "x"; then
        AC_MSG_ERROR(["Badly formed perfmon version string"])
    fi
    perfmon=`echo ${perfmon} | sed 's/\.//'`
    if test $perfmon -gt 20; then
        perfmon2=yes
    fi
    if test $perfmon -lt 25; then
        old_pfmv2=y
        PFMCFLAGS="-DPFMLIB_OLD_PFMV2"
    fi
else
    perfmon=0
    if test "$cross_compiling" = "no" ; then
        AC_CHECK_FILE(/sys/kernel/perfmon/version,
                      [perfmon=`cat /sys/kernel/perfmon/version`],
                      [AC_CHECK_FILE(/proc/perfmon,
                                     [perfmon=`cat /proc/perfmon | grep version | cut -d: -f2`],
                                     [perfmon=0])])
        if test "$perfmon" != 0; then
            pfm_incdir="libpfm-3.y/include"
            perfmon=`echo ${perfmon} | sed 's/^[ \t]*//;s/[ \t]*$//'`
            perfmon=`echo ${perfmon} | grep -e '[[1-9]]\.[[0-9]][[0-9]]*'`
            perfmon=`echo ${perfmon} | sed 's/\.//'`
            if test $perfmon -gt 20; then
                perfmon2=yes
            fi
            if test $perfmon -lt 25; then
                # must be y, not yes, or libpfm breaks
                old_pfmv2="y"
                PFMCFLAGS="-DPFMLIB_OLD_PFMV2"
            fi
        fi
    fi
fi

force_pfm_incdir=no
# default

AC_ARG_WITH([pfm_root],
            [AS_HELP_STRING([--with-pfm-root=<path>],
                            [Specify path to source tree (for use by developers only)])],
            [
             pfm_root=$withval
             pfm_incdir=$withval/include
             pfm_libdir=$withval/lib
            ])

AC_ARG_WITH([pfm_prefix],
            [AS_HELP_STRING([--with-pfm-prefix=<path>],
                            [Specify prefix to installed pfm distribution])],
            [
             pfm_prefix=$withval
             pfm_incdir=$pfm_prefix/include
             pfm_libdir=$pfm_prefix/lib
            ])

AC_ARG_WITH([pfm_incdir],
            [AS_HELP_STRING([--with-pfm-incdir=<path>],
                            [Specify directory of pfm header files in non-standard location])],
            [pfm_incdir=$withval])

AC_ARG_WITH([pfm_libdir],
            [AS_HELP_STRING([--with-pfm-libdir=<path>],
                            [Specify directory of pfm library in non-standard location])],
            [pfm_libdir=$withval])

# if these are both empty, it means we haven't set either pfm_prefix or pfm_root
# which would have set them. Thus it means that we set this to our included
# libpfm4 library. Shame on the person that sets one but not the other.

if test "x$pfm_incdir" = "x" -a "x$pfm_libdir" = "x"; then
    pfm_root="libpfm4"
    pfm_incdir="libpfm4/include"
    pfm_libdir="libpfm4/lib"
fi

##################################################
#          Linux perf_event/perf_counter
##################################################

if test "x$mic" = "xno"; then
    perf_events=no
fi

force_perf_events=no
disable_uncore=yes

AC_ARG_WITH([perf_events],
            [AS_HELP_STRING([--with-perf-events],
                            [Specify use of Linux Performance Event (requires kernel 2.6.32 or greater)])],
            [
             force_perf_events=yes
             user_specified_interface=pe
            ])

# RDPMC support

AC_ARG_ENABLE(perfevent_rdpmc,
              [AS_HELP_STRING([--enable-perfevent-rdpmc],
                              [Enable userspace rdpmc instruction on perf_event, default: yes])],
              [
               case "${enableval}" in
                   yes) enable_perfevent_rdpmc=true ;;
                   no)  enable_perfevent_rpdmc=false ;;
                   *)   AC_MSG_ERROR([bad value ${enableval} for --enable-perfevent-rdpmc]) ;;
               esac
              ],
              [enable_perfevent_rdpmc=true])

if test "$enable_perfevent_rdpmc" = "true"; then
    AX_APPEND_FLAG([-DUSE_PERFEVENT_RDPMC=1],[PECFLAGS])
fi

# Uncore support
AC_ARG_WITH([pe_incdir],
            [AS_HELP_STRING([--with-pe-incdir=<path>],
                            [Specify path to the correct perf header file])],
            [
             pe_incdir=$withval
             force_perf_events=yes
             user_specified_interface=pe
            ],
            [pe_incdir=$pfm_incdir/perfmon])

AC_ARG_ENABLE(perf_event_uncore,
              [AS_HELP_STRING([--disable-perf-event-uncore],
                              [Disable perf_event uncore component])])

AS_IF([test "x$enable_perf_event_uncore" != "xno"],[
    disable_uncore=no
])

# Check for perf_event.h
if test "$force_perf_events" = "yes"; then
    perf_events="yes"
fi
if test "$cross_compiling" = "no"; then
    AC_CHECK_FILE(/proc/sys/kernel/perf_event_paranoid,
                  [have_paranoid=yes
                   AC_CHECK_FILE($pe_incdir/perf_event.h,perf_events="yes")])
fi
if test "$perf_events" = "yes"; then
    AX_APPEND_FLAG([-DPEINCLUDE=\\\"$pe_incdir/perf_event.h\\\"],[PECFLAGS])
fi

#
# Sort out the choice of the user vs. what we detected
#
# MESSING WITH CFLAGS IS STUPID!
#
if test "$user_specified_interface" != "no"; then
    if test "$user_specified_interface" = "perfctr"; then
        perfmon=0
        perf_events="no"
        perfnec=0
    else
        if test "$user_specified_interface" = "perfmon"; then
            perfctr=0
            perf_events="no"
            AX_APPEND_FLAG([$PFMCFLAGS],[CPPFLAGS])
            perfnec=0
        else
            if test "$user_specified_interface" = "pe"; then
                perfctr=0
                perfmon=0
                AX_APPEND_FLAG([$PECFLAGS],[CPPFLAGS])
                perfnec=0
            else
                if test "$user_specified_interface" = "perfnec"; then
                    perfctr=0
                    perfmon=0
                    perf_events=0
                    AX_APPEND_FLAG([-DPERFNEC],[CPPFLAGS])
                    AC_MSG_NOTICE([XXXXX user_specified_interface perfnec])
                else
                    AC_MSG_ERROR(["Unknown user_specified_interface=$user_specified_interface perfctr=$perfctr perfmon=$perfmon perfmon2=$perfmon2 perf-events=$perf_events perfnec=$perfnec"])
                fi
            fi
        fi
    fi
else
    if test "$perfmon" != 0; then
        AX_APPEND_FLAG([$PFMCFLAGS],[CPPFLAGS])
    fi
    if test "$perf_events" = "yes"; then
        AX_APPEND_FLAG([$PECFLAGS],[CPPFLAGS])
    fi
fi


#
# User has made no choice, so we default to the ordering below in the platform section, if
# we detect more than one.
#

#
# What does this next section do? It determines whether or not to run the tests for libpfm
# based on the settings of pfm_root, pfm_prefix, pfm_incdir, pfm_libdir
#

# Both should be 0 for NEC
if test "$perfmon" != 0 -o "$perf_events" = "yes"; then
    # if prefix set, then yes
    if test "x$pfm_prefix" != "x"; then
        dotest=1
    # if root not set and libdir set, then yes
    elif test "x$pfm_root" = "x" -a "x$pfm_libdir" != "x"; then
        dotest=1
    else
        dotest=0
    fi
    if test "$dotest" = 1; then
        # TODO: are we sure we want to overwrite existing LIBS variable?
        LIBS="-L$pfm_libdir -lpfm"
        AX_APPEND_FLAG([-I$pfm_incdir],[CPPFLAGS])
        AC_CHECK_LIB([pfm], [pfm_initialize])
        if test "$ac_cv_lib_pfm_pfm_initialize" = "yes" ; then
            AC_CHECK_HEADERS([perfmon/pfmlib.h],
                             [
                              if test "$arch" = "ia64"; then
                                  AC_CHECK_HEADERS([perfmon/pfmlib_montecito.h])
                              fi
                              AC_CHECK_FUNC(pfm_get_event_description,
                                            [AC_DEFINE(HAVE_PFM_GET_EVENT_DESCRIPTION,1,[event description function])],[])
                              AC_CHECK_MEMBER(pfmlib_reg_t.reg_evt_idx,
                                              [AC_DEFINE(HAVE_PFM_REG_EVT_IDX,1,[old reg_evt_idx])],[],[#include "perfmon/pfmlib.h"])
                              AC_CHECK_MEMBER(pfmlib_output_param_t.pfp_pmd_count,
                                              [AC_DEFINE(HAVE_PFMLIB_OUTPUT_PFP_PMD_COUNT,1,[new pfmlib_output_param_t])],[],[#include "perfmon/pfmlib.h"])
                              AC_CHECK_MEMBER(pfm_msg_t.type,
                                              [AC_DEFINE(HAVE_PFM_MSG_TYPE,1,[new pfm_msg_t])],[],[#include "perfmon/perfmon.h"])
                             ],
                             [AC_MSG_ERROR([perfmon/pfmlib.h not found, rerun configure with different flags])])],
        else
            AC_MSG_ERROR([libpfm.a not found, rerun configure with different flags])
        fi
    else
        AC_DEFINE(HAVE_PERFMON_PFMLIB_MONTECITO_H,1,[Montecito headers])
        AC_DEFINE(HAVE_PFM_GET_EVENT_DESCRIPTION,1,[event description function])
        AC_DEFINE(HAVE_PFMLIB_OUTPUT_PFP_PMD_COUNT,1,[new pfmlib_output_param_t])
    fi
fi

##################################################
#          Checking platform
##################################################
AC_MSG_CHECKING([platform])
case "$OS" in
    nec)
        MAKEVER=nec-nec
        ;;
    aix)
        MAKEVER="$OS"-"$CPU" ;;
    bgp)
        MAKEVER=bgp ;;
    bgq)
        MAKEVER=bgq ;;
    CLE)
        if test "$perfmon2" = "yes"; then
            #  major_version=`echo $OSVER | sed 's/\([[^.]][[^.]]*\).*/\1/'`
            #  minor_version=`echo $OSVER | sed 's/[[^.]][[^.]]*.\([[^.]][[^.]]*\).*/\1/'`
            #  point_version=`echo $OSVER | sed -e 's/[[^.]][[^.]]*.[[^.]][[^.]]*.\(.*\)/\1/' -e 's/[[^0-9]].*//'`
            #    if (test "$major_version" = 2 -a "$minor_version" = 6 -a "$point_version" -lt 31 -a "$perfmon2" != "yes" ); then
            MAKEVER="$OS"-perfmon2
        else
            MAKEVER="$OS"-pe
        fi ;;
    freebsd)
        MAKEVER="freebsd"
        LDFLAGS="-lpmc"
        # HWPMC driver is available for FreeBSD >= 6
        FREEBSD_VERSION=`uname -r | cut -d'.' -f1`
        if test "${FREEBSD_VERSION}" -lt 6 ; then
            AC_MSG_ERROR([PAPI requires FreeBSD 6 or greater])
        fi
        # Determine if HWPMC module is on the kernel
        dmesg | grep hwpmc 2> /dev/null > /dev/null
        if test "$?" != "0" ; then
            AC_MSG_ERROR([HWPMC module not found. (see INSTALL.TXT)])
        fi
        # Determine the number of counters
        AC_TRY_LINK([
                     #include <unistd.h>
                     #include <pmc.h>
                    ],
                    [int i = pmc_init();],
                    [pmc_init_linked="yes"], [pmc_init_linked="no"])

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
        freebsd_pmc_counters=$num_counters
        AC_SUBST([freebsd_pmc_counters])
        AC_CONFIG_FILES([freebsd-config.h])
        ;;
    linux)
        if test "$force_perf_events" = "yes" ; then
            MAKEVER="$OS"-pe
        elif test "$force_perfmon2" = "yes" ; then
            MAKEVER="$OS"-perfmon2
        elif test "$force_perfctr" = "yes" ; then
            MAKEVER="$OS"-perfctr-x86
            case "$CPU" in
                itanium2|montecito)
                    if test "$bitmode" = "32"; then
                        AC_MSG_ERROR([The bitmode you specified is not supported])
                    fi
                    MAKEVER="$OS"-pfm-"$CPU"
                    ;;
                POWER5|POWER5+|POWER6|POWER7|PPC970)
                    MAKEVER="$OS"-perfctr-"$CPU" ;;
            esac
        elif test "$perf_events" = "yes" ; then
            MAKEVER="$OS"-pe
        elif test "$perfmon2" = "yes" ; then
            MAKEVER="$OS"-perfmon2
        elif test "$old_pfmv2" = "y" ; then
            MAKEVER="$OS"-pfm-"$CPU"
        elif test "$perfctr" != 0 ; then
            case "$CPU" in
                itanium2|montecito)
                    if test "$bitmode" = "32"; then
                        AC_MSG_ERROR([The bitmode you specified is not supported])
                    fi
                    MAKEVER="$OS"-pfm-"$CPU"
                    ;;
                x86)
                    MAKEVER="$OS"-perfctr-x86 ;;
                POWER5|POWER5+|POWER6|POWER7|PPC970)
                    MAKEVER="$OS"-perfctr-"$CPU" ;;
                *)
                    MAKEVER="$OS"-generic ;;
            esac
        else
            MAKEVER="$OS"-generic
        fi ;;
    solaris)
        if test "$bitmode" = "64" -a "`isainfo -v | grep "64"`" = ""; then
            AC_MSG_ERROR([The bitmode you specified is not supported])
        fi
        MAKEVER="$OS"-"$CPU" ;;
     darwin)
        MAKEVER="$OS" ;;
esac

AC_MSG_RESULT([$MAKEVER])
if test "x$MAKEVER" = "x"; then
    AC_MSG_NOTICE([This platform is not supported so a generic build without CPU counters will be used])
    MAKEVER="generic_platform"
fi

##################################################
#          Set build macros 
##################################################
FILENAME=Makefile.inc
SHOW_CONF=showconf
CTEST_TARGETS="all"
FTEST_TARGETS="all"
LIBRARY=libpapi.a
SHLIB='libpapi.so.AC_PACKAGE_VERSION'
PAPISOVER='$(PAPIVER).$(PAPIREV)'
VLIB='libpapi.so.$(PAPISOVER)'
OMPCFLGS=-fopenmp
CC_R='$(CC) -pthread'
CC_SHR='$(CC) -fPIC -DPIC -shared -Wl,-soname -Wl,$(VLIB) -Xlinker "-rpath" -Xlinker "$(LIBDIR)"'
if test "$CC_COMMON_NAME" = "gcc"; then
    if test "$bitmode" = "32"; then
        BITFLAGS=-m32
    elif test "$bitmode" = "64"; then
        BITFLAGS=-m64
    fi
fi

OPTFLAGS="$OPTFLAGS"
AX_APPEND_FLAG([-D_REENTRANT -D_GNU_SOURCE $NOTLS],[CPPFLAGS])
AX_APPEND_FLAG([$BITFLAGS],[CC])
if test "$CC_COMMON_NAME" = "gcc"; then
    AX_APPEND_FLAG([-Wall],[CFLAGS])
fi
AX_APPEND_FLAG([-Dlinux],[FFLAGS])
AX_APPEND_FLAG([$BITFLAGS],[F77])

# OS Support

if (test "$OS" = "aix"); then
    OSFILESSRC=aix-memory.c
    OSLOCK=aix-lock.h
    OSCONTEXT=aix-context.h
elif (test "$OS" = "bgp"); then
    OSFILESSRC=linux-bgp-memory.c
    OSLOCK=linux-bgp-lock.h
    OSCONTEXT=linux-bgp-context.h
elif (test "$OS" = "bgq"); then
    OSFILESSRC=linux-bgq-memory.c
    OSLOCK=linux-bgq-lock.h
    OSCONTEXT=linux-context.h
elif (test "$OS" = "freebsd"); then
    OSFILESSRC=freebsd-memory.c
    OSLOCK="freebsd-lock.h"
    OSCONTEXT="freebsd-context.h"
elif (test "$OS" = "nec"); then
    OSFILESSRC="linux-memory.c linux-timer.c linux-common.c"
    OSFILESHDR="linux-memory.h linux-timer.h linux-common.h"
    OSLOCK="linux-lock.h"
    OSCONTEXT="linux-context.h"
elif (test "$OS" = "linux"); then
    OSFILESSRC="linux-memory.c linux-timer.c linux-common.c"
    OSFILESHDR="linux-memory.h linux-timer.h linux-common.h"
    OSLOCK="linux-lock.h"
    OSCONTEXT="linux-context.h"
elif (test "$OS" = "solaris"); then
    OSFILESSRC="solaris-memory.c solaris-common.c"
    OSFILESHDR="solaris-memory.h solaris-common.h"
    OSLOCK="solaris-lock.h"
    OSCONTEXT="solaris-context.h"
elif (test "$OS" = "darwin"); then
    OSFILESSRC="darwin-memory.c darwin-common.c"
    OSFILESHDR="darwin-memory.h darwin-common.h"
    OSLOCK="darwin-lock.h"
    OSCONTEXT="darwin-context.h"
fi

OSFILESOBJ='$(OSFILESSRC:.c=.o)'


if (test "$MAKEVER" = "aix-power5" || test "$MAKEVER" = "aix-power6" || test "$MAKEVER" = "aix-power7"); then
    if test "$bitmode" = "64"; then
        LIBRARY=libpapi64.a
        SHLIB=libpapi64.so
# By default AIX enforces a limit on heap space 
#( limiting the heap to share the same 256MB memory segment as stack )
# changing the max data paramater moves the heap off the stack's memory segment
        BITFLAGS='-q64 -bmaxdata:0x07000000000000'
        ARG64=-X64
    else
# If the issue ever comes up, /dsa requires AIX v5.1 or higher
# and the Large address-space model (-bmaxdata) requires v4.3 or later
# see http://publib.boulder.ibm.com/infocenter/pseries/v5r3/topic/com.ibm.aix.genprogc/doc/genprogc/lrg_prg_support.htm#a179c11c5d
        SHLIB=libpapi.so
        BITFLAGS="-bmaxdata:0x80000000/dsa"
    fi

    CPUCOMPONENT_NAME=aix
    CPUCOMPONENT_C=aix.c
    CPUCOMPONENT_OBJ=aix.o
    VECTOR=_aix_vector
    PAPI_EVENTS_CSV="papi_events.csv"
    MISCHDRS="aix.h components/perfctr_ppc/ppc64_events.h papi_events_table.h"
    MISCSRCS="aix.c"
    AX_APPEND_FLAG([-qenum=4],[CFLAGS])
    AX_APPEND_FLAG([-Icomponents/perfctr_ppc -DNO_VARARG_MACRO],[CPPFLAGS])
    AX_APPEND_FLAG([-D_AIX -D_$(CPU_MODEL) -DNEED_FFSLL],[CPPFLAGS])
    AX_APPEND_FLAG([-DARCH_EVTS=\\\"$(ARCH_EVENTS).h\\\" -DCOMP_VECTOR=_ppc64_vectors],[CPPFLAGS])
    AX_APPEND_FLAG([-DSTATIC_PAPI_EVENTS_TABLE -I$(PMAPI)/include],[CPPFLAGS])
    AX_APPEND_FLAG([-Icomponents/perfctr_ppc],[CPPFLAGS])
    AX_APPEND_FLAG([-qmaxmem=-1 -qarch=$(cpu_option) -qtune=$(cpu_option) -qlanglvl=extended],[CFLAGS])
    AX_APPEND_FLAG([$(BITFLAGS)],[CC])
    AX_APPEND_FLAG([-WF,-D_$(CPU_MODEL) -WF,-DARCH_EVTS=\\\"$(ARCH_EVENTS).h\\\"],[FFLAGS])
    if test $debug != "yes"; then
        OPTFLAGS='-O3 -qstrict $(PMINIT)'
    else
        OPTFLAGS='$(PMINIT)'
    fi
    SMPCFLGS=-qsmp
    OMPCFLGS='-qsmp=omp'
    LDFLAGS='-L$(PMAPI)/lib -lpmapi'
    CC_R=xlc_r
    CC=xlc
    CC_SHR="xlc -G -bnoentry"
    AC_CHECK_PROGS( [MPICC], [mpicc mpcc], [])
    F77=xlf
    CPP='xlc -E $(CPPFLAGS)'
    if test "$MAKEVER" = "aix-power5"; then
        ARCH_EVENTS=power5_events
        CPU_MODEL=POWER5
        cpu_option=pwr5
        DESCR="AIX 5.1.0 or greater with POWER5"
        if test "$bitmode" = "64"; then
            DESCR="$DESCR 64 bit build"
        fi
    elif test "$MAKEVER" = "aix-power6"; then
        ARCH_EVENTS=power6_events
        CPU_MODEL=POWER6
        cpu_option=pwr6
        DESCR="AIX 5.1.0 or greater with POWER6"
        AX_APPEND_FLAG([-qlanglvl=extended],[CFLAGS])
        if test "$bitmode" = "64"; then
            DESCR="$DESCR 64 bit build"
        fi
    elif test "$MAKEVER" = "aix-power7"; then
        ARCH_EVENTS=power7_events
        CPU_MODEL=POWER7
        cpu_option=pwr7
        DESCR="AIX 5.1.0 or greater with POWER7"
        AX_APPEND_FLAG([-qlanglvl=extended],[CFLAGS])
        if test "$bitmode" = "64"; then
            DESCR="$DESCR 64 bit build"
        fi
    fi
elif test "$MAKEVER" = "bgp"; then
    CPP="$CC -E"
    CPUCOMPONENT_NAME=linux-bgp
    CPUCOMPONENT_C=linux-bgp.c
    CPUCOMPONENT_OBJ=linux-bgp.o
    VECTOR=_bgp_vectors
    PAPI_EVENTS_CSV="papi_events.csv"
    MISCSRCS=
    AX_APPEND_FLAG([-gdwarf-2 -Wall],[CFLAGS])
    AX_APPEND_FLAG([-I. -I$(BGP_SYSDIR)/arch/include -DCOMP_VECTOR=_bgp_vectors],[CPPFLAGS])
    tests="$tests bgp_tests"
    SHOW_CONF=show_bgp_conf
    BGP_SYSDIR=/bgsys/drivers/ppcfloor
    BGP_GNU_LINUX_PATH='${BGP_SYSDIR}/gnu-linux'
    LDFLAGS='-L$(BGP_SYSDIR)/runtime/SPI -lSPI.cna'
    FFLAGS='-g -gdwarf-2 -O2 -Wall -I. -Dlinux'
    OPTFLAGS="-g -Wall -O3"
    TOPTFLAGS="-g -Wall -O0"
    SHLIB=libpapi.so
    DESCR="Linux for BlueGene/P"
    LIBS=static
    CC_SHR='$(CC) -shared -Xlinker "-soname" -Xlinker "$(SHLIB)" -Xlinker "-rpath" -Xlinker "$(LIBDIR)"'
    OMPCFLGS=""
elif test "$MAKEVER" = "bgq"; then
    FILENAME=Rules.bgpm
    VECTOR=_bgq_vectors
    CPUCOMPONENT_NAME=linux-bgq
    CPUCOMPONENT_C=linux-bgq.c
    CPUCOMPONENT_OBJ=linux-bgq.o
    PAPI_EVENTS_CSV="papi_events.csv"
    MISCSRCS="linux-bgq-common.c"
    OPTFLAGS="-g -Wall -O3"
    TOPTFLAGS="-g -Wall -O0"
    SHLIB=libpapi.so
    DESCR="Linux for Blue Gene/Q"
    CC_SHR='$(CC) -fPIC -DPIC -shared -Wl,-soname -Wl,$(SHLIB) -Xlinker "-rpath" -Xlinker "$(LIBDIR)"'
    OMPCFLGS=""
elif test "$MAKEVER" = "CLE-perfmon2"; then
    FILENAME=Rules.perfmon2
    CPUCOMPONENT_NAME=perfmon
    CPUCOMPONENT_C=perfmon.c
    CPUCOMPONENT_OBJ=perfmon.o
    VECTOR=_papi_pfm_vector
    PAPI_EVENTS_CSV="papi_events.csv"
    F77=gfortran
    AX_APPEND_FLAG([-D__crayxt],[CPPFLAGS])
    FFLAGS=""
elif test "$MAKEVER" = "freebsd"; then
    CPUCOMPONENT_NAME=freebsd
    CPUCOMPONENT_C=freebsd.c
    CPUCOMPONENT_OBJ=freebsd.o
    VECTOR=_papi_freebsd_vector
    PAPI_EVENTS_CSV="freebsd_events.csv"
    MISCHDRS="freebsd/map-unknown.h freebsd/map.h freebsd/map-p6.h freebsd/map-p6-m.h freebsd/map-p6-3.h freebsd/map-p6-2.h freebsd/map-p6-c.h freebsd/map-k7.h freebsd/map-k8.h freebsd/map-p4.h freebsd/map-atom.h freebsd/map-core.h freebsd/map-core2.h freebsd/map-core2-extreme.h freebsd/map-i7.h freebsd/map-westmere.h"
    MISCSRCS="$MISCSRCS freebsd/map-unknown.c freebsd/map.c freebsd/map-p6.c freebsd/map-p6-m.c freebsd/map-p6-3.c freebsd/map-p6-2.c freebsd/map-p6-c.c freebsd/map-k7.c freebsd/map-k8.c freebsd/map-p4.c freebsd/map-atom.c freebsd/map-core.c freebsd/map-core2.c freebsd/map-core2-extreme.c freebsd/map-i7.c freebsd/map-westmere.c"
    DESCR="FreeBSD -over libpmc- "
    AX_APPEND_FLAG([-fPIC],[CFLAGS])
    AX_APPEND_FLAG([-I. -Ifreebsd -DPIC],[CPPFLAGS])
    CC_SHR='$(CC) -shared -Xlinker "-soname" -Xlinker "libpapi.so" -Xlinker "-rpath" -Xlinker "$(LIBDIR)" -DPIC -fPIC -I. -Ifreebsd'
elif test "$MAKEVER" = "linux-generic"; then
    CPUCOMPONENT_NAME=linux-generic
    CPUCOMPONENT_C=linux-generic.c
    CPUCOMPONENT_OBJ=linux-generic.o
    PAPI_EVENTS_CSV="papi_events.csv"
    VECTOR=_papi_dummy_vector
elif test "$MAKEVER" = "linux-pe"; then
    FILENAME=Rules.pfm4_pe
    CPUCOMPONENT_NAME=perf_event
    components="perf_event"
    if test "$disable_uncore" = "no"; then
        components="$components perf_event_uncore"
    fi
elif test "$MAKEVER" = "linux-perfctr-x86"; then
    FILENAME=Rules.perfctr-pfm
    CPUCOMPONENT_NAME=perfctr-x86
    VERSION=2.6.x
    components="perfctr"
elif (test "$MAKEVER" = "linux-perfctr-POWER5" || test "$MAKEVER" = "linux-perfctr-POWER5+" || test "$MAKEVER" = "linux-perfctr-POWER6" || test "$MAKEVER" = "linux-perfctr-POWER7" || test "$MAKEVER" = "linux-perfctr-PPC970"); then
    FILENAME=Rules.perfctr
    CPUCOMPONENT_NAME=perfctr-ppc
    VERSION=2.7.x
    components="perfctr_ppc"
    CPU=ppc64
    if test "$MAKEVER" = "linux-perfctr-POWER5"; then
        CPU_MODEL=POWER5
        ARCH_EVENTS=power5_events
        ARCH_SPEC_EVTS=power5_events_map.c
    elif test "$MAKEVER" = "linux-perfctr-POWER5+"; then
        CPU_MODEL=POWER5p
        ARCH_EVENTS=power5+_events
        ARCH_SPEC_EVTS=power5+_events_map.c
    elif test "$MAKEVER" = "linux-perfctr-POWER6"; then
        CPU_MODEL=POWER6
        ARCH_EVENTS=power6_events
        ARCH_SPEC_EVTS=power6_events_map.c
    elif test "$MAKEVER" = "linux-perfctr-POWER7"; then
        CPU_MODEL=POWER7
        ARCH_EVENTS=power7_events
        ARCH_SPEC_EVTS=power7_events_map.c
    elif test "$MAKEVER" = "linux-perfctr-PPC970"; then
        CPU_MODEL=PPC970
        ARCH_EVENTS=ppc970_events
        ARCH_SPEC_EVTS=ppc970_events_map.c
    fi
    AX_APPEND_FLAG([-DPPC64 -D_$(CPU_MODEL) -D__perfctr__ -DARCH_EVTS=\\\"$(ARCH_EVENTS).h\\\" -DCOMP_VECTOR=_ppc64_vectors],[CPPFLAGS])
    FFLAGS='-D_$(CPU_MODEL)'
elif test "$MAKEVER" = "nec-nec"; then
    FILENAME=Rules.perfnec
    CPUCOMPONENT_NAME=perfnec
    components="perfnec"
elif test "$MAKEVER" = "linux-perfmon2"; then
    FILENAME=Rules.perfmon2
    CPUCOMPONENT_NAME=perfmon2
    components="perfmon2"
elif (test "$MAKEVER" = "linux-pfm-ia64" || test "$MAKEVER" = "linux-pfm-itanium2" || test "$MAKEVER" = "linux-pfm-montecito"); then
    FILENAME=Rules.pfm
    CPUCOMPONENT_NAME=perfmon-ia64
    components="perfmon_ia64"
    VERSION=3.y
    if test "$MAKEVER" = "linux-pfm-itanium2"; then
        CPU=2
    else
        CPU=3
    fi
    AX_APPEND_FLAG([-DITANIUM$CPU],[CPPFLAGS])
    AX_APPEND_FLAG([-DITANIUM$CPI],[FFLAGS])

    CC_SHR='$(CC) -fPIC -DPIC -shared -Wl,-soname -Wl,$(SHLIB) -Xlinker "-rpath" -Xlinker "$(LIBDIR)"'
elif test "$MAKEVER" = "solaris-ultra"; then
    CPUCOMPONENT_NAME=solaris-ultra
    CPUCOMPONENT_C=solaris-ultra.c
    CPUCOMPONENT_OBJ=solaris-ultra.obj
    VECTOR=_solaris_vector
    PAPI_EVENTS_CSV="papi_events.csv"
    DESCR="Solaris 5.8 or greater with UltraSPARC I, II or III"
    if test "$CC" = "gcc"; then
        F77=g77
        CPP="$CC -E"
        CC_R="$CC"
        CC_SHR="$CC -shared -fpic"
        OPTFLAGS=-O3
        AX_APPEND_FLAG([-DNEED_FFSLL],[CPPFLAGS])
        FFLAGS="$CPPFLAGS $CFLAGS"
    else
        # Sun Workshop compilers: V5.0 and V6.0 R2
        CPP="$CC -E"
        CC_R="$CC -mt"
        CC_SHR="$CC -ztext -G -Kpic"
        AX_APPEND_FLAG([-xtarget=ultra3 -xarch=v8plusa],[CFLAGS])
        AX_APPEND_FLAG([-DNO_VARARG_MACRO -D__EXTENSIONS__ -DPAPI_NO_MEMORY_MANAGEMENT -DCOMP_VECTOR=_solaris_vectors],[CPPFLAGS])
        SMPCFLGS=-xexplicitpar
        OMPCFLGS=-xopenmp
        F77=f90
        FFLAGS=$CFLAGS
        NOOPT=-xO0
        OPTFLAGS="-g -fast -xtarget=ultra3 -xarch=v8plusa"
    fi
    LDFLAGS="$LDFLAGS -lcpc"
    if test "$bitmode" = "64"; then
        LIBRARY=libpapi64.a
        SHLIB=libpapi64.so
        AX_APPEND_FLAG([-xtarget=ultra3 -xarch=v9a],[CFLAGS])
        AX_APPEND_FLAG([-DNO_VARARG_MACRO -D__EXTENSIONS__ -DPAPI_NO_MEMORY_MANAGEMENT -DCOMP_VECTOR=_solaris_vectors],[CPPFLAGS])
        OPTFLAGS="-g -fast -xtarget=ultra3 -xarch=v9a"
    fi
elif test "$MAKEVER" = "solaris-niagara2"; then
    CPUCOMPONENT_NAME=solaris-niagara2
    CPUCOMPONENT_C=solaris-niagara2.c
    CPUCOMPONENT_OBJ=solaris-niagara2.obj
    VECTOR=_niagara2_vector
    PAPI_EVENTS_CSV="papi_events.csv"
    AX_APPEND_FLAG([-xtarget=native -xarch=native],[CFLAGS])
    AX_APPEND_FLAG([-DNO_VARARG_MACRO -D__EXTENSIONS__ -DCOMP_VECTOR=_niagara2_vector],[CPPFLAGS])
    ORY_MANAGEMENT="-DCOMP_VECTOR=_solaris_vector"
    DESCR="Solaris 10 with libcpc2 and UltraSPARC T2 (Niagara 2)"
    CPP="$CC -E"
    CC_R="$CC -mt"
    CC_SHR="$CC -ztext -G -Kpic"
    SMPCFLGS=-xexplicitpar
    OMPCFLGS=-xopenmp
    F77=f90
    FFLAGS="$CPPFLAGS $CFLAGS"
    NOOPT=-xO0
    OPTFLAGS="-fast"
    FOPTFLAGS=$OPTFLAGS
    AX_APPEND_FLAG([-lcpc],[LDFLAGS])
    if test "$bitmode" = "64";  then
        LIBRARY=libpapi64.a
        SHLIB=libpapi64.so
        # FIXME: -m64 should not be added to CFLAGS (FFLAGS) but CC (F77) instead
        AX_APPEND_FLAG([-m64],[CC])
        AX_APPEND_FLAG([-m64],[F77])
    fi
elif test "$MAKEVER" = "darwin"; then
    DESCR="Darwin"
    CPUCOMPONENT_NAME=darwin
    CPUCOMPONENT=linux-generic.c
    CPUCOMPONENT=linux-generic.obj
    AX_APPEND_FLAG([-DNEED_FFSLL],[CPPFLAGS])
    CC_SHR='$(CC) -fPIC -DPIC -shared -Wl,-dylib -Xlinker "-rpath" -Xlinker "$(LIBDIR)"'
    SHLIB=libpapi.dylib
elif test "$MAKEVER" = "generic_platform"; then
    DESCR="Generic platform"
fi

MISCOBJS='$(MISCSRCS:.c=.o)'



if test "$F77" = "pgf77"; then
    AX_APPEND_FLAG([-Wall -Mextend],[FFLAGS])
elif test "$F77" = "ifort"; then
    AX_APPEND_FLAG([-warn all],[FFLAGS])
elif test "$F77" != "xlf"; then
    AX_APPEND_FLAG([-ffixed-line-length-132],[FFLAGS])
fi

if test "$CC_COMMON_NAME" = "icc"; then
    OMPCFLGS=-qopenmp
fi

AC_MSG_CHECKING([for components to build])
COMPONENT_RULES=components/Rules.components
components_vector_list="$VECTOR"
components_vector_pointer_list=""

AC_ARG_WITH([components],
            [AS_HELP_STRING([--with-components=<"component1 component2">],
                            [Specify which components to build])],
            [components="$components $withval"])

for comp in $components; do
    # FIXME: if component is of the form 'path/comp' count the number of chars
    # up to '/' (not included) and remove them from the original string. Thus,
    # if comp="path/comp", subcomp="/comp". This behavior seems wrong.
    idx=`echo $comp | sed -n "s/\/.*//p" | wc -c`
    subcomp=`echo $comp | sed -E "s/^.{${idx}}//"`
    components_vector_list="_${subcomp}_vector, ${components_vector_list}"
    components_vector_pointer_list="&_${subcomp}_vector, ${components_vector_pointer_list}"
    COMPONENT_RULES="$COMPONENT_RULES components/$comp/Rules.$subcomp"
done

# Remove trailing ',' from components_vector_list and components_vector_pointer_list
components_vector_list=`echo $components_vector_list | sed -E 's/,$//'`
components_vector_pointer_list=`echo $components_vector_pointer_list | sed -E 's/,$//'`

AC_SUBST(components_vector_list)
AC_SUBST(components_vector_pointer_list)
AC_CONFIG_FILES([components_config.h])

# check for component tests
# FIXME: As for previous loop above, comp can be 'path/comp'. Thus, we should be
# removing the leading part of the path from here too.
# FIXME: top_srcdir currently set to '.' should be set to absolute srcdir path.
top_srcdir="."
for comp in $components; do
    if test -d "${top_srcdir}/components/${comp}/tests" ; then
        COMPONENTS="$COMPONENTS $comp"
    fi
done
tests="$tests comp_tests"

for comp in $components; do
    # check for SDE component to determine if we need -lrt in LDFLAGS
    if test "x$comp" = "xsde" ; then
        # FIXME: we should not put LRT into LDFLAGS
        AX_APPEND_FLAG([-DSDE],[CPPFLAGS])
        AX_APPEND_FLAG([$LRT],[LDFLAGS])
        AX_APPEND_FLAG([$LRT],[LIBS])
    fi

    # check for intel_gpu component to determine if we need -lstdc++ in LDFLAGS
    if test "x$comp" = "xintel_gpu" ; then
        AX_APPEND_FLAG([-lstdc++],[LDFLAGS])
    fi

    # check for cuda component to determine if we need to check on Profiler.
    if test "x$comp" = "xcuda" ; then
        if test "`find $PAPI_CUDA_ROOT -name "cupti_profiler_target.h"`" != "" ; then
            AX_APPEND_FLAG([-DCUPTI_PROFILER=1],[CPPFLAGS])
        else
            AX_APPEND_FLAG([-DCUPTI_PROFILER=0],[CPPFLAGS])
        fi
    fi
done

AC_MSG_RESULT([$components])

AX_APPEND_FLAG([-I. -DOSLOCK=\\\"$OSLOCK\\\" -DOSCONTEXT=\\\"$OSCONTEXT\\\"],[CPPFLAGS])
AX_APPEND_FLAG([$OPTFLAGS],[CFLAGS])
AX_APPEND_FLAG([$OPTFLAGS],[FFLAGS])

AC_MSG_CHECKING([for PAPI event CSV filename to use])
if test "x$PAPI_EVENTS_CSV" == "x"; then
    PAPI_EVENTS_CSV="papi_events.csv"
fi
AC_MSG_RESULT([$PAPI_EVENTS_CSV])

AC_SUBST(prefix)
AC_SUBST(exec_prefix)
AC_SUBST(libdir)
AC_SUBST(includedir)
AC_SUBST(mandir)
AC_SUBST(bindir)
AC_SUBST(datadir)
AC_SUBST(datarootdir)
AC_SUBST(docdir)
AC_SUBST(PACKAGE_TARNAME)
AC_SUBST(arch)
AC_SUBST(MAKEVER)
AC_SUBST(PMAPI)
AC_SUBST(PMINIT)
AC_SUBST(F77)
AC_SUBST(CPP)
AC_SUBST(CC)
AC_SUBST(AR)
AC_SUBST(papiLIBS)
AC_SUBST(STATIC)
AC_SUBST(NO_MPI_TESTS)
AC_SUBST(LDFLAGS)
AC_SUBST(altix)
AC_SUBST(perfctr_root)
AC_SUBST(perfctr_prefix)
AC_SUBST(perfctr_incdir)
AC_SUBST(perfctr_libdir)
AC_SUBST(pfm_root)
AC_SUBST(old_pfmv2)
AC_SUBST(pfm_prefix)
AC_SUBST(pfm_incdir)
AC_SUBST(pfm_libdir)
AC_SUBST(OS)
AC_SUBST(CFLAGS)
AC_SUBST(FFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(PAPI_EVENTS)
AC_SUBST(PAPI_EVENTS_CSV)
AC_SUBST(SETPATH)
AC_SUBST(LINKLIB)
AC_SUBST(VERSION)
AC_SUBST(CPU)
AC_SUBST(FILENAME)
AC_SUBST(LIBRARY)
AC_SUBST(SHLIB)
AC_SUBST(PAPISOVER)
AC_SUBST(VLIB)
AC_SUBST(CPUCOMPONENT_NAME)
AC_SUBST(CPUCOMPONENT_C)
AC_SUBST(CPUCOMPONENT_OBJ)
AC_SUBST(OSFILESSRC)
AC_SUBST(OSFILESOBJ)
AC_SUBST(OSFILESHDR)
AC_SUBST(DESCR)
AC_SUBST(LIBS)
AC_SUBST(CTEST_TARGETS)
AC_SUBST(CC_R)
AC_SUBST(CC_SHR)
AC_SUBST(SMPCFLGS)
AC_SUBST(OMPCFLGS)
AC_SUBST(NOOPT)
AC_SUBST(MISCSRCS)
AC_SUBST(MISCOBJS)
AC_SUBST(POST_BUILD)
AC_SUBST(ARCH_EVENTS)
AC_SUBST(CPU_MODEL)
AC_SUBST(cpu_option)
AC_SUBST(ARG64)
AC_SUBST(FLAGS)
AC_SUBST(MPICC)
AC_SUBST(MISCHDRS)
AC_SUBST(SHLIBDEPS)
AC_SUBST(TOPTFLAGS)
AC_SUBST(TESTS)
AC_SUBST(tests)
AC_SUBST(SHOW_CONF)
AC_SUBST(BGP_SYSDIR)
AC_SUBST(COMPONENT_RULES)
AC_SUBST(COMPONENTS)
AC_SUBST(FTEST_TARGETS)
AC_SUBST(HAVE_NO_OVERRIDE_INIT)
AC_SUBST(BGPM_INSTALL_DIR)
AC_SUBST(CC_COMMON_NAME)

if test "$cross_compiling" = "yes" ; then
    AC_MSG_NOTICE([Compiling genpapifdef with $nativecc because cross compiling])
    $nativecc -I. genpapifdef.c -o genpapifdef
else
    AC_MSG_NOTICE([Compiling genpapifdef with $CC])
    $CC -I. genpapifdef.c -o genpapifdef
fi

AC_MSG_NOTICE([Generating fpapi.h])
./genpapifdef -c > fpapi.h
AC_MSG_NOTICE([Generating f77papi.h])
./genpapifdef -f77 > f77papi.h
AC_MSG_NOTICE([Generating f90papi.h])
./genpapifdef -f90 > f90papi.h

AC_MSG_NOTICE([$FILENAME will be included in the generated Makefile])
AC_CONFIG_FILES([Makefile papi.pc])
AC_CONFIG_FILES([components/Makefile_comp_tests.target testlib/Makefile.target utils/Makefile.target ctests/Makefile.target ftests/Makefile.target validation_tests/Makefile.target])
AC_OUTPUT

if test "$have_paranoid" = "yes"; then
    paranoid_level=`cat /proc/sys/kernel/perf_event_paranoid`
    if test $paranoid_level -gt 2; then
        warning_text=`echo -e "\n
***************************************************************************
* Insufficient permissions for accessing any hardware counters.           *
* Your current paranoid level is $paranoid_level.                                       *
* Set /proc/sys/kernel/perf_event_paranoid to 2 (or less) or run as root. *
*                                                                         *
* Example:                                                                *
* sudo sh -c \"echo 2 > /proc/sys/kernel/perf_event_paranoid\"              *
***************************************************************************
"\ `
        AC_MSG_WARN([$warning_text])
    fi
fi

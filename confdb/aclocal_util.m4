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

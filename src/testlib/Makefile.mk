##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

AM_CPPFLAGS += -I$(top_srcdir)/src/testlib

noinst_LTLIBRARIES = src/testlib/libtestlib.la

src_testlib_libtestlib_la_SOURCES = src/testlib/clockcore.c  \
                                    src/testlib/do_loops.c   \
                                    src/testlib/test_utils.c \
                                    src/testlib/ftests_util.F


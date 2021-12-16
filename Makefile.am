##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

ACLOCAL_AMFLAGS = -I confdb
EXTRA_PROGRAMS = $(testlist)
testlist =

AM_CFLAGS   = $(CFLAGS)
AM_CPPFLAGS = $(CPPFLAGS)
AM_LDFLAGS  = $(LDFLAGS)
AM_FFLAGS   = $(FFLAGS)

papi_sources =
papi_f77_sources =

dist_noinst_SCRIPTS = autogen.sh
noinst_HEADERS =
bin_PROGRAMS =
include_HEADERS =
perflib = @libpfm_la@

include $(top_srcdir)/src/Makefile.mk
include $(top_srcdir)/doc/Makefile.mk

SUBDIRS = $(with_perf_lib_subdir)
DIST_SUBDIRS = external/libpfm4

lib_LTLIBRARIES = lib/lib@PAPILIBNAME@.la
lib_lib@PAPILIBNAME@_la_SOURCES      = $(papi_sources)
lib_lib@PAPILIBNAME@_la_CPPFLAGS     = $(AM_CPPFLAGS)
lib_lib@PAPILIBNAME@_la_CFLAGS       = $(AM_CFLAGS)
lib_lib@PAPILIBNAME@_la_LDFLAGS      = $(AM_LDFLAGS) $(AM_FFLAGS) -release $(PAPI_VERSION)
lib_lib@PAPILIBNAME@_la_LIBADD       = $(LIBS) $(perflib)
lib_lib@PAPILIBNAME@_la_DEPENDENCIES =

EXTRA_DIST = README.md COPYRIGHT maint/gen_comp_makefile.sh

test: $(testlist)

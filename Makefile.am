##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

ACLOCAL_AMFLAGS = -I confdb

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
instdir = $(prefix)

include $(top_srcdir)/src/Makefile.mk

# export destination dir for performance lib
export PREFIX=$(prefix)

# FIXME: all set and export is necessary because external
#        libraries are not autotools compatible
CONFIG_PFMLIB_ARCH_X86=@CONFIG_PFMLIB_ARCH_X86@
CONFIG_PFMLIB_OLD_PFMV2=@CONFIG_PFMLIB_OLD_PFMV2@
ARCH=@PAPI_HOST_SYSTEM@
OPTIM=$(CFLAGS)

# export environment for pfm
export FORCE_FPM_ARCH
export CONFIG_PFMLIB_OLD_PFMV2
export ARCH
export OPTIM

SUBDIRS = $(with_perf_lib_subdir)
DIST_SUBDIRS = external/libpfm4

lib_LTLIBRARIES = lib/lib@PAPILIBNAME@.la
lib_lib@PAPILIBNAME@_la_SOURCES      = $(papi_sources)
lib_lib@PAPILIBNAME@_la_CPPFLAGS     = $(AM_CPPFLAGS)
lib_lib@PAPILIBNAME@_la_CFLAGS       = $(AM_CFLAGS)
lib_lib@PAPILIBNAME@_la_LDFLAGS      = $(AM_LDFLAGS) $(AM_FFLAGS) $(PAPI_VERSION)
lib_lib@PAPILIBNAME@_la_LIBADD       = $(LIBS)
lib_lib@PAPILIBNAME@_la_DEPENDENCIES =

EXTRA_DIST = README.md COPYRIGHT

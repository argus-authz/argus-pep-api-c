#
# $Id: Makefile.am,v 1.1 2008/12/12 11:27:12 vtschopp Exp $
#

SUBDIRS = hessian pep util 

lib_LTLIBRARIES = libpep-c.la
libpep_c_la_LDFLAGS = -version-info 1:0:0
libpep_c_la_LIBADD = hessian/libhessian.la util/libutil.la pep/libpep.la

bin_PROGRAMS = test-pep
test_pep_SOURCES = $(top_builddir)/test/test_pep.c
test_pep_LDADD = libpep-c.la
test_pep_CPPFLAGS = -I$(top_srcdir)/src
#
# $Id: Makefile.am,v 1.11 2009/04/15 14:35:03 vtschopp Exp $
#

SUBDIRS = util hessian pep . cli

#
# libpep-c = util + hessian + pep
#
lib_LTLIBRARIES = libpep-c.la
libpep_c_la_LIBADD = util/libutil.la hessian/libhessian.la pep/libpep.la 
libpep_c_la_LDFLAGS = -version-info 1:2:0
libpep_c_la_SOURCES = libpep-c.c

# headers files in <prefix>/include (public API)
nobase_include_HEADERS = pep/pep.h pep/error.h pep/xacml.h

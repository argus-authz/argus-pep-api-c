#
# $Id: Makefile.am,v 1.9 2009/03/23 14:03:15 vtschopp Exp $
#

SUBDIRS = util hessian pep . cli

#
# libpep-c = util + hessian + pep
#
lib_LTLIBRARIES = libpep-c.la
libpep_c_la_LIBADD = util/libutil.la hessian/libhessian.la pep/libpep.la 
libpep_c_la_LDFLAGS = -version-info 1:0:0
libpep_c_la_SOURCES = libpep-c.c

# headers files in <prefix>/include (public API)
nobase_include_HEADERS = pep/pep.h pep/error.h pep/xacml.h

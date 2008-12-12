#
# $Id: Makefile.am,v 1.2 2008/12/12 13:32:11 vtschopp Exp $
#

lib_LTLIBRARIES = libpep-c.la
libpep_c_la_LIBADD = ../util/libutil.la ../hessian/libhessian.la 

libpep_c_la_SOURCES = \
action.c \
attribute.c \
attributeassignment.c \
environment.c \
error.c \
error.h \
io.c \
io.h \
model.h \
obligation.c \
pep.c \
pep.h \
request.c \
resource.c \
response.c \
result.c \
status.c \
subject.c

#
# link with libcurl
#
libpep_c_la_LDFLAGS = -version-info 0:0:0 -lcurl

#
# headers files
#
includedir=$(prefix)/include/pep
include_HEADERS = pep.h model.h error.h

SOURCES = $(libpep_c_la_SOURCES)

#
# $Id: Makefile.am,v 1.1 2008/12/12 11:27:12 vtschopp Exp $
#

noinst_LTLIBRARIES = libpep.la

libpep_la_SOURCES = \
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
libpep_la_LDFLAGS = -lcurl

#
# headers files
#
includedir=$(prefix)/include/pep
include_HEADERS = pep.h model.h error.h

SOURCES = $(libpep_la_SOURCES)
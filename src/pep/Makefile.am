#
# $Id: Makefile.am,v 1.3 2008/12/18 11:45:38 vtschopp Exp $
#

include $(top_srcdir)/Makefile.flags.am 

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
# linker flags
#
libpep_c_la_LDFLAGS = -version-info 0:0:0

#
# headers files
#
includedir=$(prefix)/include/pep
include_HEADERS = pep.h model.h error.h


#
# $Id: Makefile.am,v 1.4 2009/03/13 10:15:58 vtschopp Exp $
#

include $(top_srcdir)/Makefile.flags.am 

#
# library to build
#
lib_LTLIBRARIES = libpep-c.la
libpep_c_la_LIBADD = ../util/libutil.la ../hessian/libhessian.la 
libpep_c_la_LDFLAGS = -version-info 0:0:0 -lcurl

# sources not distributed
libpep_c_la_SOURCES = \
action.c \
attribute.c \
attributeassignment.c \
environment.c \
error.c \
error.h \
io.c \
io.h \
obligation.c \
pep.c \
pep.h \
request.c \
resource.c \
response.c \
result.c \
status.c \
subject.c \
xacml.h

# headers files (public API)
includedir=$(prefix)/include/pep
include_HEADERS = pep.h error.h xacml.h


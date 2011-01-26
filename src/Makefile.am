#
# Copyright (c) Members of the EGEE Collaboration. 2006-2010.
# See http://www.eu-egee.org/partners/ for details on the copyright holders.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

SUBDIRS = util hessian pep 

#
# libpep-c = util + hessian + pep
#
lib_LTLIBRARIES = libargus_pep_api.la

libargus_pep_api_la_CFLAGS = \
    $(LIBCURL_CFLAGS)

libargus_pep_api_la_LIBADD = \
    util/libutil.la \
    hessian/libhessian.la \
    pep/libpep.la \
    $(LIBCURL_LIBS)

libargus_pep_api_la_LDFLAGS = \
    -version-info 2:0:0

libargus_pep_api_la_SOURCES = libargus_pep.c

# headers files in <prefix>/include (public API)
pepincludedir = $(includedir)/argus
pepinclude_HEADERS = pep/pep.h pep/pip.h pep/oh.h pep/error.h pep/xacml.h pep/profiles.h

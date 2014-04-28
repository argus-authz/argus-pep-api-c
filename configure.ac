#                                              -*- Autoconf -*-
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

##
# VERSION number
AC_INIT([argus-pep-api-c], [2.3.0], [argus-support@googlegroups.com])
##

AC_CONFIG_AUX_DIR([project])
# ACLOCAL_AMFLAGS = -I <dir> required in your top-level Makefile.am
#AC_CONFIG_MACRO_DIR([project])

# OSX requires ./configure --disable-dependency-tracking to compile, others don't care
AM_INIT_AUTOMAKE([no-dependencies])
# disable static library
AM_DISABLE_STATIC

#
# check for options
#
AC_MSG_CHECKING([whether to package header files and doc])
AC_ARG_ENABLE([devel],
    [AS_HELP_STRING([--disable-devel],
        [create a package without the header files and doc])],
    [enable_devel=no],
    [enable_devel=yes])
AM_CONDITIONAL([ENABLE_DEVEL], [test "x$enable_devel" == xyes])
AC_MSG_RESULT([$enable_devel])

AC_MSG_CHECKING([whether to package the shared library])
AC_ARG_ENABLE([library],
    [AS_HELP_STRING([--disable-library],
        [create a package without the shared library])],
    [enable_library=no],
    [enable_library=yes])
AM_CONDITIONAL([ENABLE_LIBRARY], [test "x$enable_library" == xyes])
AC_MSG_RESULT([$enable_library])

# RHEL6/SL6 use libcurl with NSS, not OpenSSL!!!
#AC_MSG_CHECKING([whether libcurl uses NSS])
#AC_ARG_WITH([libcurl-nss],
#   [AS_HELP_STRING([--with-libcurl-nss],[assume libcurl is compiled with NSS [default=libcurl-openssl]])],
#   [],
#   [with_libcurl_nss=no])
#
#AS_IF([test "x$with_libcurl_nss" != xno],
#   [AC_DEFINE([HAVE_LIBCURL_NSS],[1],[Define to 1 if libcurl is compiled with NSS, not OpenSSL])])
#AC_MSG_RESULT([$with_libcurl_nss])



# Checks for programs.
AM_PROG_MKDIR_P
AC_PROG_LIBTOOL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S

# Check for doxygen
AC_CHECK_PROG(DOXYGEN,[doxygen],[doxygen],[$ac_aux_dir/missing --run doxygen])
AC_SUBST(DOXYGEN)

# GCC CFLAGS
if test "$GCC" = "yes"; then
    # pedantic ANSI C flags
    CFLAGS="$CFLAGS -Wall -ansi -pedantic -std=c99"
fi

AC_MSG_NOTICE([libdir for $host: $libdir])

#
# use pkg-config tool for libcurl
#
# export PKG_CONFIG_PATH=...
#
AC_MSG_NOTICE([PKG_CONFIG_PATH=$PKG_CONFIG_PATH])
PKG_CHECK_MODULES(
    LIBCURL, [libcurl],
    [
        AC_MSG_NOTICE([LIBCURL_CFLAGS=$LIBCURL_CFLAGS])
        AC_MSG_NOTICE([LIBCURL_LIBS=$LIBCURL_LIBS])
    ],
    [
        # Checks for curl headers and library
        # will automatically add -lcurl in LDFLAGS
        AC_CHECK_HEADER([curl/curl.h],,[AC_MSG_ERROR(can not find libcurl header curl/curl.h)])
        AC_CHECK_LIB(curl,curl_easy_init)
    ]
)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h stdlib.h stdio.h stdint.h stdarg.h float.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library and functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strerror strrchr calloc])

#AC_PREFIX_DEFAULT([/opt/emi])

AM_CONFIG_HEADER([src/config.h])

AC_CONFIG_FILES([
libargus-pep.pc
Makefile
src/Makefile
src/util/Makefile
src/hessian/Makefile
src/argus/Makefile
])

AC_OUTPUT



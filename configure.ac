#                                              -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# $Id: configure.ac,v 1.4 2008/12/18 11:44:14 vtschopp Exp $
#

AC_INIT(glite-authz-pep-c, 1.0.0, grid@switch.ch)
AC_CONFIG_AUX_DIR(project)
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC
#CFLAGS="-Wall -g -O2"

if test "x$host_cpu" = "xx86_64"; then
    AC_SUBST([libdir], ['${exec_prefix}/lib64'])
fi

# Checks for curl headers and library
# will automatically add -lcurl in LDFLAGS
AC_CHECK_HEADERS(curl/curl.h)
AC_CHECK_LIB(curl,curl_easy_init)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h stdlib.h stdio.h stdint.h stdarg.h float.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# FUCK!!! int32_t and int64_t macro not available on SLC4
#AC_TYPE_INT32_T
#AC_TYPE_INT64_T

# Checks for library and functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strerror strrchr calloc])

AC_PREFIX_DEFAULT("/opt/glite")

AC_CONFIG_SRCDIR(src)
AM_CONFIG_HEADER(src/config.h)

AC_CONFIG_FILES([Makefile src/Makefile src/util/Makefile src/hessian/Makefile src/pep/Makefile])

AC_OUTPUT



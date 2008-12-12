#                                              -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# $Id: configure.ac,v 1.2 2008/12/12 16:35:14 vtschopp Exp $
#

#AC_PREREQ(2.59)
AC_INIT(glite-authz-pep-c, 1.0.0, grid@switch.ch)
AC_CONFIG_AUX_DIR(project)

AM_INIT_AUTOMAKE(glite-authz-pep-c, 1.0.0)

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC

if test "x$host_cpu" = "xx86_64"; then
    AC_SUBST([libdir], ['${exec_prefix}/lib64'])
fi

# Checks for libraries.
#AC_LIBCURL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([curl/curl.h string.h stdlib.h stdio.h stdint.h stdarg.h float.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# FUCK!!! int32_t and int64_t macro not available on SLC4
#AC_TYPE_INT32_T
#AC_TYPE_INT64_T

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strerror strrchr calloc])

AC_PREFIX_DEFAULT("/opt/glite")

AC_CONFIG_SRCDIR(src)
AM_CONFIG_HEADER(src/config.h)

AC_CONFIG_FILES([Makefile src/Makefile src/util/Makefile src/hessian/Makefile src/pep/Makefile])

AC_OUTPUT



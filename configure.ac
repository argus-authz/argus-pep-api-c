#                                              -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# $Id$
#

AC_INIT(glite-authz-pep-c, 1.1.6-2, grid@switch.ch)
AC_CONFIG_AUX_DIR(project)

# OSX requires ./configure --disable-dependency-tracking to compile, others don't care
AM_INIT_AUTOMAKE([no-dependencies])
# disable static library
AM_DISABLE_STATIC

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC
#AC_PROG_MKDIR_P # not defined on SL4 !!!
AC_PROG_INSTALL
AC_PROG_LN_S

# Check for doxygen
AC_CHECK_PROG(DOXYGEN,doxygen,doxygen,$ac_aux_dir/missing --run doxygen)
AC_SUBST(DOXYGEN)

# GCC CFLAGS
if test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -Wall "
fi
# OSX CFLAGS
case $host in
*-*-darwin*)
    AC_MSG_NOTICE([Mac OSX ($host_os): building for i386 and x86_64 architectures])
    CFLAGS="$CFLAGS -arch i386 -arch x86_64 "
    ;;
esac

AC_MSG_NOTICE([libdir for $host: $libdir])
# <prefix>/lib64 for x86_64 cpu 
#AC_MSG_CHECKING([libdir for $host])
#if test "x$host_cpu" = "xx86_64"; then
#    AC_SUBST([libdir], ['${exec_prefix}/lib64'])
#fi
#AC_MSG_RESULT([$libdir])

# Checks for curl headers and library
# will automatically add -lcurl in LDFLAGS
AC_CHECK_HEADER([curl/curl.h],,[AC_MSG_ERROR(can not find libcurl header)])
AC_CHECK_LIB(curl,curl_easy_init)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h stdlib.h stdio.h stdint.h stdarg.h float.h getopt.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# FUCK!!! int32_t and int64_t macro not available on SLC4
#AC_TYPE_INT32_T
#AC_TYPE_INT64_T

# Checks for library and functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strerror strrchr calloc])

# Sets default prefix
AC_PREFIX_DEFAULT(/opt/glite)

AC_CONFIG_SRCDIR(src)
AM_CONFIG_HEADER(src/config.h)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/util/Makefile
src/hessian/Makefile
src/pep/Makefile
src/cli/Makefile
])

AC_OUTPUT



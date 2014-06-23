#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69.120])
AC_INIT([cascade_cvs], [0.1.0], [])
AC_CONFIG_SRCDIR([src/CascadeCVSDrawing.cpp])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([acbuild-aux])

AM_INIT_AUTOMAKE([1.14 foreign subdir-objects -Wall -Werror])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_MKDIR_P

AC_LANG([C++])


# Checks for libraries.
AC_ARG_WITH(mptl,
	[AS_HELP_STRING([--with-mpc=PATH],
	[specify directory for mptl directory])])

AS_IF( [test "x$with_mptl" != "xno" -a "x$with_mptl" != "xyes"],
       [AM_CPPFLAGS=-I$with_mptl $AM_CPPFLAGS])

LIBS="$LIBS -lopencv_highgui -lopencv_core"

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime memset sqrt])

AC_SUBST([AM_CPPFLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

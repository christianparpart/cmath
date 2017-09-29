AC_PREREQ([2.69])
AC_INIT([cmath], [0.11.0-dev], [christian@parpart.family], [], [https://github.com/christianparpart/cmath])
dnl AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([configure.ac])
dnl AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects serial-tests])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

AC_LANG_CPLUSPLUS
AC_PROG_RANLIB
AC_PROG_CC
#AC_PROG_CC_FOR_BUILD
AC_PROG_CXX
#AC_PROG_CXX_FOR_BUILD
#AC_PROC_LIBTOOL
AM_PROG_AR
AM_PROG_AS
#LT_INIT
PKG_PROG_PKG_CONFIG([0.25])

uname_system=`uname -s`
AC_MSG_NOTICE("Operating system: $uname_system")
AM_CONDITIONAL([PLATFORM_DARWIN], [test x$uname_system = xDarwin])
AM_CONDITIONAL([PLATFORM_LINUX], [test x$uname_system = xLinux])
AM_CONDITIONAL([PLATFORM_WSL], [uname -r | grep -q Microsoft])

AC_HEADER_STDC

####### OUTPUT ################################################################

BASE_CXXFLAGS="-DXOPEN_SOURCE=600 -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE \
 -D_LARGEFILE_SOURCE -D_LARGE_FILES -Wall -Wextra -std=c++1z \
 -Wno-unused-variable -Wno-unused-parameter"
AC_SUBST([BASE_CXXFLAGS])

BASE_LIBS=""
AC_SUBST([BASE_LIBS])

BASE_LDFLAGS="-rdynamic"
AC_SUBST([BASE_LDFLAGS])

AC_CONFIG_HEADERS(src/cmath/sysconfig.h)
AC_CONFIG_FILES(Makefile libcmath.pc)
AC_OUTPUT

dnl Copyright (C) 2008 Tavian Barnes <tavianator@gmail.com>
dnl
dnl This file is part of The Sandglass Library.
dnl
dnl The Sandglass Library is free software; you can redistribute it
dnl and/or modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either version
dnl 3 of the License, or (at your option) any later version.
dnl
dnl The Sandglass Library is distributed in the hope that it will be
dnl useful, but WITHOUT ANY WARRANTY; without even the implied warranty
dnl of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this program.  If not, see
dnl <http://www.gnu.org/licenses/>.

AC_PREREQ(2.65)
AC_INIT(The Sandglass Library, 0.2, tavianator@gmail.com, libsandglass)
AM_INIT_AUTOMAKE([parallel-tests])

dnl Programs
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AM_PROG_AS
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h time.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Find out which assembly files to compile
AC_CANONICAL_HOST

tsc=0

if echo $host_cpu | grep '86$' 2>&1 >/dev/null
then
  arch_x86=1
  tsc=1
fi

if echo $host_cpu | grep 'x86_64$' 2>&1 >/dev/null
then
  arch_x86_64=1
  tsc=1
fi

AM_CONDITIONAL([X86], [test "$arch_x86"])
AM_CONDITIONAL([X86_64], [test "$arch_x86_64"])

AC_DEFINE_UNQUOTED([SANDGLASS_TSC], [$tsc])
AM_CONDITIONAL([TSC], [test "$tsc" -ne 0])

dnl Generate Makefiles
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libsandglass.pc
                 tests/Makefile])
AC_OUTPUT

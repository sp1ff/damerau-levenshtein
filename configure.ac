dnl Copyright (C) 2020-2024 Michael Herstine <sp1ff@pobox.com>

dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or (at
dnl your option) any later version.

dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
dnl General Public License for more details.

dnl You should have received a copy of the GNU General Public License
dnl along with this program. If not, see https://www.gnu.org/licenses/.
    
AC_INIT([damerau levenshtein], [0.2.2], [sp1ff@pobox.com], [], [https://github.com/sp1ff/damerau-levenshtein])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/dl.cc])
AM_INIT_AUTOMAKE([-Wall -Werror gnits std-options dist-bzip2 dist-xz])

AC_PROG_CXX
AC_PROG_INSTALL

# Checks for header files. 
AC_CHECK_HEADERS([stdlib.h]) 

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT

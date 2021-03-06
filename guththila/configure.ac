dnl Licensed to the Apache Software Foundation (ASF) under one or more
dnl contributor license agreements.  See the NOTICE file distributed with
dnl this work for additional information regarding copyright ownership.
dnl The ASF licenses this file to You under the Apache License, Version 2.0
dnl (the "License"); you may not use this file except in compliance with
dnl the License.  You may obtain a copy of the License at
dnl 
dnl   http://www.apache.org/licenses/LICENSE-2.0
dnl 
dnl Unless required by applicable law or agreed to in writing, software
dnl distributed under the License is distributed on an "AS IS" BASIS,
dnl WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl See the License for the specific language governing permissions and
dnl limitations under the License.
dnl run autogen.sh to generate the configure script.

AC_PREREQ(2.59)

AC_INIT(guththilac-src, 1.7.0)
AC_CANONICAL_SYSTEM
AM_CONFIG_HEADER(config.h)
dnl AM_INIT_AUTOMAKE([tar-ustar])
AM_INIT_AUTOMAKE
AC_PREFIX_DEFAULT(/usr/local/guththila)

m4_ifdef([_A][M_PROG_TAR],[_A][M_SET_OPTION([tar-ustar])])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl check for flavours of varargs macros (test from GLib)
AC_MSG_CHECKING(for ISO C99 varargs macros in C)
AC_TRY_COMPILE([],[
int a(int p1, int p2, int p3);
#define call_a(...) a(1,__VA_ARGS__)
call_a(2,3);
],axis2c_have_iso_c_varargs=yes,axis2c_have_iso_c_varargs=no)
AC_MSG_RESULT($axis2c_have_iso_c_varargs)

AC_MSG_CHECKING(for GNUC varargs macros)
AC_TRY_COMPILE([],[
int a(int p1, int p2, int p3);
#define call_a(params...) a(1,params)
call_a(2,3);
],axis2c_have_gnuc_varargs=yes,axis2c_have_gnuc_varargs=no)
AC_MSG_RESULT($axis2c_have_gnuc_varargs)

dnl Output varargs tests
if test x$axis2c_have_iso_c_varargs = xyes; then
    AC_DEFINE(HAVE_ISO_VARARGS,1,[Have ISO C99 varargs macros])
fi
if test x$axis2c_have_gnuc_varargs = xyes; then
    AC_DEFINE(HAVE_GNUC_VARARGS,1,[Have GNU-style varargs macros])
fi

dnl Checks for libraries.
AC_CHECK_LIB(dl, dlopen)

CFLAGS="$CFLAGS -D_LARGEFILE64_SOURCE -DAXIS2_GUTHTHILA_ENABLED"

if test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -ansi -ggdb3 -Wall -Wno-implicit-function-declaration "
fi

AC_MSG_CHECKING(whether to enable code coverage statistics)
AC_ARG_ENABLE(coverage, [  --enable-coverage       enable code coverage statistics (default=no)],
[ case "${enableval}" in
  no)
    AC_MSG_RESULT(no)
    CFLAGS="$CFLAGS"
    CPPFLAGS="$CPPFLAGS"
    ;;
  *)
    AC_MSG_RESULT(yes)
    CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
    CPPFLAGS="$CPPFLAGS -fprofile-arcs -ftest-coverage"
    LDFLAGS="$LDFLAGS -lgcov --coverage"
    ;;
  esac ],[
  AC_MSG_RESULT(no)
  CFLAGS="$CFLAGS"
  CPPFLAGS="$CPPFLAGS"]
)

AC_MSG_CHECKING(whether to use the Google test framework)
AC_ARG_WITH(gtest,
            [  --with-gtest[=PATH]      Find the gtest source files in 'PATH'.],
[ case "$withval" in
  no)
    AC_MSG_RESULT(no)
    USE_GTEST=""
    GTEST_DIR=""
    GTEST=""
    ;;
  *)
    AC_MSG_RESULT(yes)
    GTEST_DIR="$withval"
    GTEST="gtest"
    CXXFLAGS="$CXXFLAGS -g -Wall -Wextra -pthread"
    ;;
  esac ],
  AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(whether to build tests)
AC_ARG_ENABLE(tests, [  --enable-tests    build tests. default=no],
[ case "${enableval}" in
  yes)
    AC_MSG_RESULT(yes)
    TESTDIR="tests"
    ;;
  *)
    AC_MSG_RESULT(no)
    TESTDIR=""

    ;;
  esac ],
  AC_MSG_RESULT(no)
  TESTDIR=""
)




dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h string.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
#AC_CHECK_FUNCS([memmove])


UTILINC=$axis2_utilinc
VERSION_NO="7:0:7"

AC_SUBST(UTILINC)
AC_SUBST(VERSION_NO)
AC_SUBST(GTEST_DIR)
AC_SUBST(GTEST)
AC_SUBST(TESTDIR)

AC_CONFIG_FILES([Makefile \
    src/Makefile \
    gtest/Makefile \
    tests/Makefile \
    ])

AC_OUTPUT

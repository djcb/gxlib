## Copyright (C) 2015,2016 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
##
##  This library is free software; you can redistribute it and/or
##  modify it under the terms of the GNU Lesser General Public License
##  as published by the Free Software Foundation; either version 2.1
##  of the License, or (at your option) any later version.
##
##  This library is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##  Lesser General Public License for more details.
##
##  You should have received a copy of the GNU Lesser General Public
##  License along with this library; if not, write to the Free
##  Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
##  02110-1301, USA.

AC_PREREQ([2.62])
AC_INIT([gxlib],[2.0.0],
        [https://github.com/djcb/gxlib/issues],[gxlib])
AC_COPYRIGHT([Copyright (C) 2015 Dirk-Jan C. Binnema])
AC_CONFIG_AUX_DIR([.])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([gxlib/gxlib.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 -Wno-portability no-define no-dist-gzip
                       dist-xz tar-ustar])
AM_MAINTAINER_MODE([enable])

LT_PREREQ([2.2])
LT_INIT([disable-static])

# Support silent build rules. Disable by passing
# --disable-silent-rules to configure or passing V=1 to make
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CPP
AC_PROG_SED
AC_CANONICAL_HOST

AX_COMPILER_FLAGS([WARN_CFLAGS],[WARN_LDFLAGS])
LT_LIB_M

AC_PATH_PROG([PKG_CONFIG],[pkg-config],[no])
AS_IF([test "x$PKG_CONFIG" = "xno"],[
   AC_MSG_ERROR([
   *** The pkg-config script could not be found. Make sure it is
   *** in your path, or set the PKG_CONFIG environment variable
   *** to the full path to pkg-config.])
])

PKG_CHECK_MODULES(GLIB,glib-2.0 >= 2.38)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GOBJECT,gobject-2.0 >= 2.38)
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

PKG_CHECK_MODULES(GIO,gio-2.0 >= 2.38)
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

# we need some special tricks for filesystems that don't have d_type;
# e.g. Solaris. See mu-maildir.c. Explicitly disabling it is for
# testing purposes only
AC_ARG_ENABLE([dirent-d-type],
	AC_HELP_STRING([--disable-dirent-d-type],
			[Don't use dirent->d_type]),
	[], [AC_STRUCT_DIRENT_D_TYPE]
)
AS_IF([test "x$ac_cv_member_struct_dirent_d_type" != "xyes"],
	    [use_dirent_d_type="no"], [use_dirent_d_type="yes"])

# support for d_ino (inode) in struct dirent is optional; if it's
# available we can sort direntries by inode and access them in that
# order; this is much faster on some file systems (such as extfs3).
# Explicity disabling it is for testing purposes only.
AC_ARG_ENABLE([dirent-d-ino],
	AC_HELP_STRING([--disable-dirent-d-ino],
			[Don't use dirent->d_ino]),
	[], [AC_STRUCT_DIRENT_D_INO]
)
AS_IF([test "x$ac_cv_member_struct_dirent_d_ino" != "xyes"],
	    [use_dirent_d_ino="no"], [use_dirent_d_ino="yes"])

AC_CHECK_HEADERS([wordexp.h])

# check for gtk-doc (optional)
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AX_VALGRIND_CHECK

####################################################################
dnl code coverage
AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov], [Enable gcov])],
              [use_gcov=$enableval],
              [use_gcov=no])
AS_IF([test "x$use_gcov" = "xyes"],[
  AC_CHECK_PROG(LTP, lcov, lcov)
  AC_CHECK_PROG(LTP_GENHTML, genhtml, genhtml)

  AC_DEFINE(HAVE_GCOV,1,[Whether you have gcov])

        dnl Remove all optimization flags from CFLAGS
        changequote({,})
        CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
        CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
        changequote([,])

        dnl Define the special gcc flags
        GCOV_CFLAGS="-O0 -fprofile-arcs -ftest-coverage"
        GCOV_LDADD="-lgcov"

        AC_SUBST(GCOV_CFLAGS)
        AC_SUBST(GCOV_LDADD)
])
AM_CONDITIONAL(ENABLE_GCOV, test "x$use_gcov" = "xyes")
####################################################################

AC_CONFIG_FILES([
Makefile
gxlib/Makefile
gxlib/gxlib-2.0.pc
gxlib/tests/Makefile
gxio/gxio-2.0.pc
gxio/Makefile
gxio/tests/Makefile
docs/Makefile
docs/reference/Makefile
docs/reference/gxlib/Makefile
docs/reference/gxio/Makefile
m4/Makefile
])
AC_OUTPUT



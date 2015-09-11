## Copyright (C) 2015 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software Foundation,
## Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

include $(top_srcdir)/gtest.mk

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS=					\
	-I$(top_srcdir)				\
	$(GLIB_CFLAGS)				\
	$(GIO_CFLAGS)				\
	$(GOBJECT_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(GCOV_CFLAGS)				\
	-DTESTTREE1=\"${srcdir}/tree1\"

AM_LDFLAGS=					\
	$(GLIB_LIBS)				\
	$(GIO_LIBS)				\
	$(GOBJECT_LIBS)				\
	$(WARN_LDFLAGS)				\
	$(GCOV_LDADD)				\
	${top_builddir}/gxio/libgxio-2.0.la

VALGRIND_SUPPRESSIONS_FILES =			\
	${top_srcdir}/gxlib.supp

noinst_PROGRAMS=$(TEST_PROGS)

TEST_PROGS += test-gxdirwatcher
test_gxdirwatcher_SOURCES=test-gxdirwatcher.c

TESTS=$(TEST_PROGS)

EXTRA_DIST=					\
	tree1/file1				\
	tree1/file2				\
	tree1/dir1/file4.foo			\
	tree1/dir1/file5.bar			\
	tree1/dir2/file6.bar			\
	tree1/dir2/file6.foo

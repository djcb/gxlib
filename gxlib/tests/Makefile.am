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
	$(WARN_CFLAGS)				\
	$(GCOV_CFLAGS)

AM_LDFLAGS=					\
	$(GLIB_LIBS)				\
	$(WARN_LDFLAGS)				\
	$(GCOV_LDADD)				\
	${top_builddir}/gxlib/libgxlib-2.0.la

noinst_PROGRAMS=$(TEST_PROGS)

TEST_PROGS += test-gxlist
test_gxlist_SOURCES=test-gxlist.c

TEST_PROGS += test-gxfunc
test_gxfunc_SOURCES=test-gxfunc.c

TEST_PROGS += test-gxpred
test_gxpred_SOURCES=test-gxpred.c

TEST_PROGS += test-gxstr
test_gxstr_SOURCES=test-gxstr.c

TEST_PROGS += test-examples
test_examples_SOURCES=test-examples.c

TESTS=$(TEST_PROGS)

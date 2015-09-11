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

SUBDIRS=. tests

AM_CPPFLAGS=						\
	-I$(top_srcdir)					\
	$(GLIB_CFLAGS)					\
	$(WARN_CFLAGS)					\
	$(GCOV_CFLAGS)

AM_LDFLAGS=						\
	$(GLIB_LIBS)					\
	$(WARN_LDFLAGS)					\
	$(GCOV_LDADD)					\
	$(LIBM)

lib_LTLIBRARIES=					\
	libgxlib-2.0.la

libgxlib_2_0_la_SOURCES=				\
	gxlist.c					\
	gxfunc.c					\
	gxpred.c					\
	gxstr.c

libgxlibincludedir=$(includedir)/gxlib-2.0/gxlib
libgxlibinclude_HEADERS=				\
	gxfunc.h					\
	gxlib.h						\
	gxlist.h					\
	gxpred.h					\
	gxstr.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gxlib-2.0.pc

EXTRA_DIST=						\
	gxlib-2.0.pc.in

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

AM_CPPFLAGS=					\
	-I$(top_srcdir)				\
	$(GLIB_CFLAGS)				\
	$(GIO_CFLAGS)				\
	$(GOBJECT_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(GCOV_CFLAGS)

AM_LDFLAGS=					\
	$(GLIB_LIBS)				\
	$(GIO_LIBS)				\
	$(GOBJECT_LIBS)				\
	$(WARN_LDFLAGS)				\
	$(GCOV_LDADD)

lib_LTLIBRARIES=				\
	libgxio-2.0.la

libgxio_2_0_la_SOURCES=				\
	gxdirwatcher.c

libgxioincludedir=				\
	$(includedir)/gxlib-2.0/gxio

libgxioinclude_HEADERS=				\
	gxio.h					\
	gxdirwatcher.h

pkgconfigdir =					\
	$(libdir)/pkgconfig
pkgconfig_DATA =				\
	gxio-2.0.pc

EXTRA_DIST=					\
	gxio-2.0.pc.in

## Copyright (C) 2015 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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

include $(top_srcdir)/gtest.mk

check: test

ACLOCAL_AMFLAGS=-I m4
SUBDIRS=gxlib gxio m4 docs

# borrowed from Json-Glib
if ENABLE_GCOV
# use recursive makes in order to ignore errors during check/perf
lcov:
	-$(MAKE) $(AM_MAKEFLAGS) check
	$(MAKE) $(AM_MAKEFLAGS) genlcov

# we have to massage the lcov.info file slightly to hide the effect of libtool
# placing the objects files in the .libs/ directory separate from the *.c
genlcov:
	$(LTP) --directory $(top_builddir) --capture --output-file gxlib-lcov.info --test-name GXLIB_TEST --no-checksum
	$(SED) -e 's#.libs/##' < gxlib-lcov.info > gxlib-lcov.info.tmp
	LANG=C $(LTP_GENHTML) --prefix $(top_builddir) --output-directory gxlib-lcov --title "GXLib Code Coverage" --show-details gxlib-lcov.info.tmp
	$(RM) -f gxlib-lcov.info.tmp

lcov-clean:
	-$(LTP) --directory $(top_builddir) -z
	-$(RM) -rf gxlib-lcov.info gxlib-lcov
else
lcov genlcov lcov-clean:
	@echo You need to configure GXLib with support for gcov enabled.
	@echo e.g., ./configure --enable-gcov
endif

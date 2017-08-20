## Copyright (C) 2017 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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


test('test-examples', executable('test-examples', 'test-examples.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		install: false))

test('test-gxpath', executable('test-gxpath', 'test-gxpath.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		c_args: '-DSRCDIR="' + meson.current_source_dir() + '"',
		install: false))

test('test-gxfunc', executable('test-gxfunc', 'test-gxfunc.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		install: false))

test('test-gxpred', executable('test-gxpred', 'test-gxpred.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		install: false))

test('test-gxlist', executable('test-gxlist', 'test-gxlist.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		install: false))

test('test-gxstr', executable('test-gxstr', 'test-gxstr.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		install: false))

test('test-gxoption', executable('test-gxoption', 'test-gxoption.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, gxlib_dep],
		install: false))


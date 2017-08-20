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

test('test-gxdirwatcher', executable('test-gxdirwatch', 'test-gxdirwatcher.c',
		include_directories : include_directories('../..'),
		dependencies: [glibdep, giodep, gxio_dep],
		c_args: '-DTESTTREE1="' + meson.current_source_dir() + '/tree1"',
		install: false))

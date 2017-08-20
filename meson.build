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
project('gxlib', 'c',
	version: '2.0.1',
	license: 'LGPLV2.1+'
)

glibdep    = dependency('glib-2.0', version: '>= 2.38')
gobjectdep = dependency('gobject-2.0', version: '>= 2.38')
giodep     = dependency('gio-2.0', version: '>= 2.38')

cc         = meson.get_compiler('c')
libm       = cc.find_library('m', required : false)

#
# gxlib
#
gxlib_srcs=[
  'gxlib/gxfunc.c',
  'gxlib/gxlist.c',
  'gxlib/gxoption.c',
  'gxlib/gxpath.c',
  'gxlib/gxpred.c',
  'gxlib/gxstr.c'
]
gxlib_hdrs=[
  'gxlib/gxfunc.h',
  'gxlib/gxlib.h',
  'gxlib/gxlist.h',
  'gxlib/gxoption.h',
  'gxlib/gxpath.h',
  'gxlib/gxpred.h',
  'gxlib/gxstr.h'
]
gxlib = shared_library('gxlib-2.0', gxlib_srcs,
		       version: meson.project_version(),
		       include_directories: include_directories('gxlib'),
		       dependencies: [glibdep, libm],
		       install: true)
gxlib_dep = declare_dependency(
  include_directories: include_directories('gxlib/'),
  link_with : gxlib)

subdir('gxlib/tests')


# generate gxlib-2.0.pc
pkg = import('pkgconfig')
gxlib_pc = pkg.generate(libraries:   gxlib,
			name:        'gxlib-2.0',
			filebase:    'gxlib-2.0',
			version:     meson.project_version(),
			description: 'Extensions for GLib 2.x',
			requires:    'glib-2.0')

# gxio
#
gxiolib_srcs=[
  'gxio/gxdirwatcher.c'
]

gxiolib_hdrs=[
  'gxio/gxdirwatcher.h',
  'gxio/gxio.h'
]
gxiolib = shared_library('gxio-2.0', gxiolib_srcs,
 			 version: meson.project_version(),
			 include_directories: include_directories('gxio'),
			 dependencies: [glibdep, giodep, gobjectdep],
			 install: true)

gxio_dep = declare_dependency(
  include_directories: include_directories('gxio/'),
  link_with : gxiolib)

subdir('gxio/tests')

# generate gxio-2.0.pc
pkg = import('pkgconfig')
gxio_pc = pkg.generate(libraries:   gxiolib,
		       name:        'gxio-2.0',
		       filebase:    'gxio-2.0',
		       version:     meson.project_version(),
		       description: 'Extensions for GIO 2.x',
		       requires:    ['gio-2.0', 'gobject-2.0'])

/*
** Copyright (C) 2015 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public License
**  as published by the Free Software Foundation; either version 2.1
**  of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free
**  Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA
**  02110-1301, USA.
*/

#include <gxlib/gxlib.h>

static void
test_expand (void)
{
  char *s, *t;

  s = gx_path_resolve ("~/hello.txt");
  t = g_build_path ("/", g_get_home_dir(), "hello.txt", NULL);

  g_assert_cmpstr (s,==,t);

  g_free (s);
  g_free (t);
}

static void
test_resolve (void)
{
  char *s;

  s = gx_path_resolve (SRCDIR);
  g_assert_cmpstr (s,==,ABS_SRCDIR);

  g_free (s);
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL); 
  g_test_add_func ("/gx-path/expand", test_expand);
  g_test_add_func ("/gx-path/resolve", test_resolve);
  
  return g_test_run ();
}


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
test_even (void)
{
  g_assert_false (gx_is_even (-1));
  g_assert_true  (gx_is_even (0));
  g_assert_false (gx_is_even (1));
  g_assert_true  (gx_is_even (2));
}


static void
test_odd (void)
{
  g_assert_true  (gx_is_odd (-1));
  g_assert_false (gx_is_odd (0));
  g_assert_true  (gx_is_odd (1));
  g_assert_false (gx_is_odd (2));
}

static void
test_prime (void)
{
  g_assert_false  (gx_is_prime (-1));
  g_assert_false (gx_is_prime (0));
  g_assert_true  (gx_is_prime (2));
  g_assert_true (gx_is_prime (3));
  g_assert_false (gx_is_prime (4));
  g_assert_false (gx_is_prime (10));
  g_assert_false (gx_is_prime (21));
  g_assert_true (gx_is_prime (53));
  g_assert_true (gx_is_prime (953));
  g_assert_false (gx_is_prime (955));
}


static void
test_str_equal (void)
{
  g_assert_true (gx_is_str_equal ("foo", "foo"));
  g_assert_false (gx_is_str_equal ("foo", "bar"));

  g_assert_true (gx_is_str_equal (NULL, NULL));
  g_assert_false (gx_is_str_equal (NULL, "bar"));
  g_assert_false (gx_is_str_equal ("foo", NULL));
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gx-pred/is-even", test_even);
  g_test_add_func ("/gx-pred/is-odd", test_odd);
  g_test_add_func ("/gx-pred/is-prime", test_prime);
  g_test_add_func ("/gx-pred/is-str-equal", test_str_equal);
  
  return g_test_run ();
}


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

#include <gxlib.h>

/**
 * gx_is_prime:
 * @i: an integer
 * 
 * Predicate function that returns %TRUE if i is a prime number, %FALSE
 * otherwise. A prime number is a positive number that is only divisible by
 * itself and 1.
 *
 * |[<!-- language="C" -->
 * g_assert_cmpuint (gx_is_prime (13),==, TRUE);
 * g_assert_cmpuint (gx_is_prime (52),==, FALSE);
 * ]|
 * 
 * Returns: %TRUE if @i is prime; %FALSE otherwise.
 */
gboolean
gx_is_prime (gint i)
{
  int j;
  
  if (i < 2)
    return FALSE;
  
  for (j = 2; j <= sqrt(i); ++j)
    if (i % j == 0)
      return FALSE;

  return TRUE;
}


/**
 * gx_is_str_equal:
 * @s1: a string
 * @s2: another string
 * 
 * Predicate function that returns %TRUE if the strings are equal; %FALSE
 * otherwise. Safe for %NULL strings.
 *
 * |[<!-- language="C" -->
 * g_assert_true (gx_is_str_equal ("foo", "foo"));
 * g_assert_false (gx_is_str_equal ("foo", "bar"));
 * ]|
 * 
 * Returns: %TRUE if @s1 and @s2 are equal, %FALSE otherwise.
 */
gboolean
gx_is_str_equal (const char *s1, const char *s2)
{
  return g_strcmp0 (s1, s2) == 0 ? TRUE : FALSE;
}

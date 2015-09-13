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

#ifndef __GX_PRED_H__
#define __GX_PRED_H__

#include <math.h>

/**
 * SECTION:gxpred
 * @title: Predicates
 * @short_description: common predicate functions that map some value to %TRUE
 * or %FALSE.
 *
 * These are designed as simple adapters, for use with functions such as
 * gx_list_filter().
 *
 * For example, we can get a list of the prime numbers up to 100 by taking an list
 * of 1..100, then filter out all elements that are not prime.
 * 
 * |[<!-- language="C" -->
 * GList *lst, *cur;
 *
 * lst = gx_list_filter_in_place (
 *   gx_list_iota (100, 1, 1), (GXPred)gx_is_prime, NULL, NULL);
 *
 * for (cur = lst; cur; cur = g_list_next (cur))
 *    g_print ("%d\n", GPOINTER_TO_INT(cur->data));
 * 
 * g_list_free (lst);
 * ]|
 */

G_BEGIN_DECLS

/**
 * GXPred:
 * @data: a data pointer
 * @user_data: a user data pointer
 *
 * Prototype for a predicate function that takes a pointer and some
 * user-provided data, and returns either %TRUE or %FALSE.
 *
 * It can be used with gx_list_filter() and gx_list_filter_in_place().
 */
typedef gboolean (*GXPred) (gconstpointer data, gconstpointer user_data);

/**
 * gx_is_even:
 * @i: an integer
 * 
 * Predicate function that returns %TRUE if @i is an even number, %FALSE
 * otherwise. An even number is a number that is divisible by 2.
 *
 * |[<!-- language="C" -->
 * g_assert_cmpuint (gx_is_even (2),==, TRUE);
 * g_assert_cmpuint (gx_is_even (3),==, FALSE);
 * ]|
 *
 * Returns: %TRUE if @i is even, %FALSE otherwise.
 */
static inline gboolean
gx_is_even (gint i)
{
  return (i & 1) == 0 ? TRUE : FALSE;
}

/**
 * gx_is_odd:
 * @i: an integer
 * 
 * Predicate function that returns %TRUE if @i is an odd number, %FALSE
 * otherwise. An odd number is a number that is not divisible by 2.
 *
 * |[<!-- language="C" -->
 * g_assert_cmpuint (gx_is_odd (2),==, FALSE);
 * g_assert_cmpuint (gx_is_odd (3),==, TRUE);
 * ]|
 * 
 * Returns: %TRUE if @i is odd, %FALSE otherwise.
 */
static inline gboolean
gx_is_odd (gint i)
{
  return (i & 1) == 0 ? FALSE : TRUE;
}

gboolean gx_is_prime (gint i);

gboolean gx_is_str_equal (const char *s1, const char *s2);


G_END_DECLS

#endif /* __GX_PRED_H__ */


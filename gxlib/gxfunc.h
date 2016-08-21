/*
** Copyright (C) 2016 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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


#ifndef __GX_FUNC_H__
#define __GX_FUNC_H__

G_BEGIN_DECLS

/**
 * SECTION:gxfunc
 * @title: Binary and ternary functions
 * @short_description: common functions that map 2 or 3 parameters to some
 * value.
 *
 * These functions are designed as simple adapters for use with functions such
 * as gx_list_fold() .
 *
 * For example, let's calculate the product of 5, 10, 15, 20, 25.
 * |[<!-- language="C" -->
 * GList *lst;
 * int    sum;
 *
 * lst = gx_list_iota (5, 5, 5); // 5, 10, 15, 20, 25
 * sum = GPOINTER_TO_INT(gx_list_fold (lst, (GXTernaryFunc)gx_times,
 *                                     GINT_TO_POINTER(1), NULL, NULL));
 * g_assert_cmpint (sum,==, 375000);
 * g_list_free (lst);
 * ]|
 *
 * Obviously, for such a simple case, the gx_list_fold() solution looks a bit
 * convoluted compared to an explicit loop.
 */

/**
 * GXBinaryFunc:
 * @ptr: a pointer
 * @user_data: (allow-none): a user-provided data pointer
 *
 * Prototype for a binary function to be used in e.g. gx_list_map(), that takes
 * some data-pointer and and an optional user-provided pointer, and returns a
 * pointer as result.
 *
 * Return value: some pointer value for the result.
 */
typedef gpointer (*GXBinaryFunc) (gconstpointer ptr, gconstpointer user_data);


/**
 * gx_identity:
 * @ptr: a pointer
 *
 * The identity function; maps a value to itself.
 *
 * |[<!-- language="C" -->
 * const char *str;
 * str =  "FOO";
 * g_assert (gx_identity (str) == str);
 * ]|
 *
 * Return value: the pointer @ptr.
 */
static inline gconstpointer
gx_identity (gconstpointer ptr)
{
  return ptr;
}


/**
 * GXTernaryFunc:
 * @p1: first pointer argument
 * @p2: second pointer argument
 * @user_data: (allow-none): a user-provided data pointer
 *
 * Prototype for a ternary function to be used in e.g. gx_list_fold(), that
 * takes two data-pointers and an optional user-provided pointer, and returns a
 * pointer as result.
 *
 * Return value: some pointer value for the result.
 */
typedef gpointer (*GXTernaryFunc) (gconstpointer p1, gconstpointer p2,
                                   gconstpointer user_data);


/**
 * gx_str_chain:
 * @s1:(allow-none): a string value
 * @s2: another string value
 * @sepa: a separator character
 *
 * If @s1 and @s2 are not %NULL, return the contatenation of @s1 and @sepa and
 * @s2. Otherwise, return a copy of @s2.
 *
 * |[<!-- language="C" -->
 * g_assert_cmpint (gx_str_chain ("a", "b", "+"),==, "a+b");
 * ]|
 *
 * Return value:(transfer-full): If @s1 and @s2 are not %NULL, return the
 * contatenation of @s1 and @sepa and @s2. Otherwise, return a copy of @s2. Free
 * with g_free().
 */
char* gx_str_chain (const char *s1, const char *s2, const char *sepa)
  G_GNUC_WARN_UNUSED_RESULT;


/**
 * gx_plus:
 * @i: an integer value
 * @j: another integer value
 *
 * Calculates the sum of @i and @j. This function is useful when composing it
 * with other functions, such as gx_list_fold().
 *
 * |[<!-- language="C" -->
 * g_assert_cmpint (gx_plus (3, 4),==, 3 + 4);
 * ]|
 *
 * Return value: i + j
 */
static inline gint
gx_plus (gint i, gint j)
{
  return i + j;
}

/**
 * gx_times:
 * @i: an integer value
 * @j: another integer value
 *
 * Calculates the product of @i and @j. This function is useful when composing
 * it with other functions, such as gx_list_fold().
 *
 * |[<!-- language="C" -->
 *  g_assert_cmpint (gx_times (8, 7),==, 8 * 7);
 * ]|
 *
 * Return value: i ✕ j
 */
static inline gint
gx_times (gint i, gint j)
{
  return i * j;
}


/**
 * gx_max:
 * @i: an integer value
 * @j: another integer value
 *
 * Get the greatest value of @i and @j. This function is useful when composing
 * it with other functions, such as gx_list_fold().
 *
 * |[<!-- language="C" -->
 *  g_assert_cmpint (gx_max (100, 1000),==, MAX(100, 1000));
 * ]|
 *
 * Return value: the greatest of @i and @j.
 */
static inline gint
gx_max (gint i, gint j)
{
  return MAX(i,j);
}

/**
 * gx_min:
 * @i: an integer value
 * @j: another integer value
 *
 * Get the smallest value of @i and @j. This function is useful when composing
 * it with other functions, such as gx_list_fold().
 *
 * |[<!-- language="C" -->
 *   g_assert_cmpint (gx_min (123456, 54321),==, MIN(123456, 54321));
 * ]|
 *
 * Return value: the smallest of @i and @j.
 */
static inline gint
gx_min (gint i, gint j)
{
  return MIN(i,j);
}

G_END_DECLS

#endif /* __GX_FUNC_H__ */

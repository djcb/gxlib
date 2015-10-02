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

#include "gxlist.h"

/**
 * SECTION:gxlist
 * @title: Functional GLists
 * @short_description: using #GList with a functional flavor
 *
 * A #GList is a double-linked list data structure for storing arbitrary
 * data. GXLib offers a number of functions for #GList that allow for list
 * manipulation with a functional flavor. The functions are inspired by the list
 * operations in <ulink url="https://www.rust-lang.org/">Mozilla's Rust</ulink>,
 * <ulink url="http://srfi.schemers.org/srfi-1/srfi-1.html">Scheme's
 * SRFI-1</ulink> and their use for problems such as those from
 * <ulink url="https://projecteuler.net/">Project Euler</ulink>.
 *
 * Suppose we want to find the sum of the prime numbers up to 100. We can use a
 * combination of gx_list_iota(), gx_list_filter_in_place(), gx_is_prime() and
 * gx_list_sum() to accomplish this. You could use a one-liner for this, but
 * let's do it in steps:
 *
 * |[<!-- language="C" -->
 * gint sum;
 * GList *nums;
 * 
 * // numbers 1..100 (inclusive)
 * nums = gx_list_iota (100, 1, 1);
 * // filter out the non-primes
 * nums = gx_list_filter_in_place (nums, (GXPred)gx_is_prime, NULL, NULL);
 * // take the sum
 * sum = gx_list_sum (num);
 * 
 * g_assert_cmpint (sum, ==, 1060);
 * g_list_free (nums);
 * ]|
 */

/**
 * gx_list_filter:
 * @list: a #GList
 * @pred_func: a predicate function
 * @user_data: (allow-none): a user pointer passed to @pred_func.
 *
 * Create a #GList consisting of a shallow copy of the elements of @list for
 * which the predicate function @pred_func returns %TRUE.
 *
 * Note, for the inverse operation (removing all items for with a predicate
 * function returns %FALSE), see g_list_remove_all().
 * 
 * Example: getting a list of natural numbers up to 100 that can be divided
 * by either 3 or 5
 * |[<!-- language="C" -->
 *
 * static gboolean div_3_5 (gint n) { return n % 3 == 0 || n % 5 == 0; }
 *  
 * GList *lst, *filtered;
 *   
 * lst = gx_list_iota (100, 1, 1); // getting a list 1..100
 * filtered = gx_list_filter (lst, (GXPred)div_3_5, NULL);
 *   
 * // do something useful with filtered
 *   
 * g_list_free (lst);
 * g_list_free (filtered);
 * ]|
 * 
 * Returns: (transfer full): the filtered list.
 */
GList*
gx_list_filter (GList *list, GXPred pred_func, gpointer user_data)
{
  GList *cur, *filtered;

  g_return_val_if_fail (pred_func, FALSE);
  
  for (filtered = NULL, cur = list; cur; cur = g_list_next (cur))
    {
      if ((*pred_func) (cur->data, user_data))
	  filtered = g_list_prepend (filtered, cur->data);
    }

  return g_list_reverse (filtered);
}


/**
 * gx_list_every:
 * @list: a #GList
 * @pred_func: a predicate function
 * @user_data: (allow-none): a user pointer passed to @pred_func.
 * 
 * Check if the predicate is true for every element in @list. If @list is empty,
 * this is considered %TRUE.
 *
 * Example: let's verify that not every number in [1..10] is a prime number:
 * |[<!-- language="C" -->
 * GList *lst;
 * 
 * lst =  gx_list_iota (10, 1, 1);
 * g_assert_false (gx_list_every (lst, (GXPred)gx_is_prime, NULL));
 *
 * g_list_free (lst);
 * ]|
 * Returns: %TRUE if #pred_func returns %TRUE for every element in @list; %FALSE
 * otherwise.
 */
gboolean
gx_list_every (GList *list, GXPred pred_func, gpointer user_data)
{
  GList *cur;

  g_return_val_if_fail (pred_func, FALSE);
  
  for (cur = list; cur; cur = g_list_next (cur))
    if (!(*pred_func) (cur->data, user_data))
        return FALSE;

  return TRUE;
}

/**
 * gx_list_any:
 * @list: a #GList
 * @pred_func: a predicate function
 * @user_data: (allow-none): a user pointer passed to @pred_func.
 * 
 * Check if the predicate is true for any element in @list. If @list is empty,
 * this is considered %FALSE.
 *
 * Example: let's see if there are any prime-numbers between 20 and 30: 
 * |[<!-- language="C" -->
 * GList *lst;
 *
 * lst =  gx_list_iota (10, 20, 1);
 * g_assert_true (gx_list_any (lst, (GXPred)gx_is_prime, NULL));
 *
 * g_list_free (lst);
 * ]|
 *
 * Returns: %TRUE if #pred_func returns %TRUE for at least one element in @list;
 * %FALSE otherwise.
 */
gboolean
gx_list_any (GList *list, GXPred pred_func, gpointer user_data)
{
  GList *cur;

  g_return_val_if_fail (pred_func, FALSE);

  for (cur = list; cur; cur = g_list_next (cur))
    if ((*pred_func) (cur->data, user_data))
        return TRUE;

  return FALSE;
}


/**
 * gx_list_filter_in_place:
 * @list: a #GList
 * @pred_func: a predicate function
 * @user_data: (allow-none): a user pointer passed to @pred_func
 * @free_func: (allow-none): function to free elements that are filtered-out
 *
 * Remove elements from @list for which @pred_func does not return %TRUE. The
 * removed elements are freed with @free_func.
 *
 * Note, for the inverse operation (removing all items for with a predicate
 * function returns %TRUE), see g_list_remove_all().
 *
 * |[<!-- language="C" -->
 * // Example
 * ]|
 *
 * Returns: the filtered list, consisting of the remaining elements of
 * @list. Note that the start of the list may have changed.
 */
GList*
gx_list_filter_in_place (GList *list, GXPred pred_func,
                         gpointer user_data, GDestroyNotify free_func)
{
  GList *cur, *prev;

  g_return_val_if_fail (pred_func, NULL);

  prev = NULL;
  cur = list;
  
  while (cur)
    {
      GList *next;
	
      if ((*pred_func) (cur->data, user_data)) {
        cur = g_list_next (cur);
        continue;
      }

      next      = cur->next;
      prev      = cur->prev;
      
      if (prev)
	prev->next = next;
      else
	list = next;

      if (next)
	next->prev = prev;

      if (free_func) 
	(*free_func) (cur->data);

      g_list_free_1 (cur);
      
      cur = next;
    }

  return list;
}


/**
 * gx_list_take:
 * @list: a #GList
 * @n: the number of elements to take
 *
 * Take up to @n elements from @list; if @n is greater than the length of @list,
 * return the whole list.
 *
 * Note that this uses a shallow copy of the values in @list, so the list from
 * which the elements were taken and the resulting list, share the data items.
 *
 * |[<!-- language="C" -->
 * const char* words[] = { "foo", "bar", "cuux", NULL };
 * GList *lst, *first2;
 * 
 * lst = gx_strv_to_list (words, -1);
 * first2 = gx_list_take (lst, 2); // foo, bar
 *
 * // do something useful with first2
 *  
 * g_list_free (lst);
 * g_list_free (lst2);
 * ]|
 *
 * Returns:(transfer full): a new list with up to n elements; free with
 * g_list_free().
 */
GList*
gx_list_take (GList *list, gsize n)
{
  GList *taken;

  for (taken = NULL; n > 0 && list; list = g_list_next (list), --n)
    taken = g_list_prepend (taken, list->data);

  return g_list_reverse (taken);
}


static void
gx_list_free_full (GList *lst, GDestroyNotify free_func)
{
  if (free_func)
    g_list_free_full (lst, free_func);
  else
    g_list_free (lst);
}


/**
 * gx_list_take_in_place:
 * @list: a #GList
 * @n: the number of elements to take
 * @free_func: (allow-none): function to free the removed elements
 *
 * Like gx_list_take(), but affects the list in-place; effectively, this reduces
 * @list to its first @n elements, or all elements if @n is greater than the
 * length of @list.
 * 
 * |[<!-- language="C" -->
 * // Example
 * ]|
 *
 * Returns:(transfer full): the list reduced to up to @n elements.
 */
GList*
gx_list_take_in_place (GList *list, gsize n, GDestroyNotify free_func)
{
  GList *cur;
  
  if (n == 0)
    {
      gx_list_free_full (list, free_func);
      return NULL;
    }

  for (cur = list; n > 1 && cur; cur = g_list_next (cur), --n);

  if (cur && cur->next)
    {
      gx_list_free_full (cur->next, free_func);
      cur->next = NULL;
    }

  return list;
}



/**
 * gx_list_skip:
 * @list: a #GList
 * @n: the number of elements to take
 *
 * Return a list of all but the first @n elements of @list. If @n is greater
 * than the length of @list, return the empty list.
 *
 * Note that this uses a shallow copy of the values in @list, so the list from
 * which the elements were taken and the resulting list, share the data items.
 *
 * |[<!-- language="C" -->
 * // Example
 * ]|
 *
 * Returns:(transfer full): a new list with up to n elements; free with
 * g_list_free().
 */
GList*
gx_list_skip (GList *list, gsize n)
{
  GList *cur;

  for (cur = list; cur && n != 0; cur = g_list_next(cur), --n);

  return g_list_copy (cur);
}


/**
 * gx_list_skip_in_place:
 * @list: a #GList
 * @n: the number of elements to take
 * @free_func: (allow-none): function to free the removed elements
 *
 * Remove the first @n elements from @list. If @n is greater than the length of
 * @list, return the empty list.
 *
 * |[<!-- language="C" -->
 * // Example
 * ]|
 *
 * Returns:(transfer full): a list with up to n elements; free with
 * g_list_free() or g_list_free_full(), depending on the the 
 */
GList*
gx_list_skip_in_place (GList *list, gsize n, GDestroyNotify free_func)
{
  GList *cur;

  for (cur = list; cur && n != 0; cur = g_list_next(cur), --n);

  if (cur && cur->prev)
    {
      cur->prev->next = NULL;
      gx_list_free_full (list, free_func);
    }
  
  return cur;
}



/**
 * gx_list_map:
 * @list: a #GList
 * @map_func: a predicate function
 * @user_data: (allow-none): a user pointer passed to @map_func
 *
 * Create a new list of consisting of the elements obtained by applying
 * @map_func to the corresponding elements in @list.
 *
 * |[<!-- language="C" -->
 * GList *lst, *upper;
 * const char* cities[] = { "Aruba", "Hawaii", "Zanzibar", NULL };
 * 
 * lst = gx_strv_to_list (cities, -1);
 * upper = gx_list_map (lst, (GXBinaryFunc)g_ascii_strup, GINT_TO_POINTER(-1));
 *
 * // upper contains AMSTERDAM, SAN FRANCISCO, HELSINKI
 * 
 * g_list_free (lst);
 * g_list_free_full (upper, g_free);
 * ]|
 *
 * Returns: (transfer full): the list with mapped values. Whether to free with
 * g_list_free() or with g_list_free_full() depends on @map_func.
 */
GList*
gx_list_map (GList *list, GXBinaryFunc map_func, gpointer user_data)
{
  GList *cur, *mapped;

  g_return_val_if_fail (map_func, NULL);

  for (mapped = NULL, cur = list; cur; cur = g_list_next (cur))
    {
      mapped = g_list_prepend (mapped, (*map_func) (cur->data, user_data));
    }

  return g_list_reverse (mapped);
}


/**
 * gx_list_map_in_place:
 * @list: a #GList
 * @map_func: a predicate function
 * @user_data: (allow-none): a user pointer passed to @map_func
 * @free_func: (allow-none): a function to free the replaced element
 *
 * Replace each element in @list with the value obtained from applying @map_func
 * to it. Free the old element using @free_func.
 *
 * Returns: (transfer full): the list with mapped values.
 */
GList*
gx_list_map_in_place (GList *list, GXBinaryFunc map_func, gpointer user_data,
		      GDestroyNotify free_func)
{
  GList *cur;

  g_return_val_if_fail (map_func, NULL);

  for (cur = list; cur; cur = g_list_next (cur))
    {
      gpointer old;

      old = cur->data;
      cur->data = (*map_func) (old, user_data);

      if (free_func)
	(*free_func) (old);
    }

  return list;
}

/**
 * gx_list_fold:
 * @list: a #GList
 * @fold_func: a ternary function
 * @init: the start value
 * @user_data: (allow-none): a user pointer passed to @fold_func
 * @free_func: (allow-none): a function to free the intermediate values
 *
 * Given a list (a, b, c), compute
 *      fold_func (fold_func (fold_func (@init, a), b), c)
 *
 * gx_list_fold() is useful when computing a value from the elements in a list.
 * 
 * For example, we can use gx_list_fold() to turn a list of strings into a
 * single string, wit the elements separated by ", ". This is similar to what
 * g_build_path() does, but uses lists:
 * 
 * |[<!-- language="C" -->
 * static char*
 * chain (const char *s1, const char *s2, const char *sepa)
 * {
 *   if (!s1)
 *     return g_strdup (s2);
 *   else
 *     return g_strconcat (s1, sepa, s2, NULL);
 * }
 *
 * GList *lst;
 * char  *str;
 * const char* cities[] = { "Amsterdam", "San Francisco", "Helsinki", NULL };
 * 
 * lst = gx_strv_to_list ((gchar**)cities, -1);
 * str = gx_list_fold (lst, (GXTernaryFunc)chain, NULL, "; ", g_free);
 * 
 * g_assert_cmpstr (str, ==, "Amsterdam; San Francisco; Helsinki");
 * 
 * g_free (str);
 * g_list_free (lst);
 * ]|
 * Returns: (transfer full): the computed value.
 */
gpointer
gx_list_fold (GList *list, GXTernaryFunc fold_func,
	      gpointer init, gpointer user_data, GDestroyNotify free_func)
{
  GList *cur;
  gpointer result, first;

  g_return_val_if_fail (fold_func, NULL);
  
  result = NULL;
  for (result = NULL, first = init, cur = list; cur; cur = g_list_next (cur))
    {
      gpointer tmp;

      tmp = (*fold_func) (first, cur->data, user_data);
      if (free_func)
	(*free_func) (result);

      result = tmp;
      first  = result;
    }

  return result;
}


/**
 * gx_list_iota:
 * @count: number of elements to generate
 * @start: the start value
 * @step: the step size, must be > 0
 *
 * Create a #GList with @count numbers starting at @start, and then increasing
 * by @step; similar to Scheme's SRFI-1's iota procedure.
 *
 * For example, to get the first 100 even numbers: 
 * |[<!-- language="C" -->
 * GList *lst;
 * lst = gx_list_iota (100, 0, 2);
 * // do something with lst
 * g_list_free (lst);
 * ]|
 * 
 * Returns: (transfer full): a list with numbers. Free with g_free().
 */
GList*
gx_list_iota (gsize count, gint start, gsize step)
{
  GList *lst;

  g_return_val_if_fail (step > 0, NULL);
  
  for (lst = 0; count > 0; --count, start += step)
    lst = g_list_prepend (lst, GINT_TO_POINTER(start));

  return g_list_reverse (lst);
}


/**
 * gx_list_sum:
 * @list: a #GList
 *
 * Calculate the sum of a list of integers. This is an optimized shorthand for
 * |[<!-- language="C" -->
 * sum = GPOINTER_TO_INT(gx_list_fold (list, (GTernaryFunc)gx_plus,
 *                       GINT_TO_POINTER(0), NULL, NULL));
 * ]|
 *
 * Returns: (transfer full): the sum of the integers in @list.
 */
gint
gx_list_sum (GList *list)
{
  gint sum;
  for (sum = 0; list; list = g_list_next (list))
    sum += GPOINTER_TO_INT (list->data);

  return sum;
}


/**
 * gx_list_product:
 * @list: a #GList
 *
 * Calculate the product of a list of integers. This is an optimized shorthand for
 * |[<!-- language="C" -->
 * sum = GPOINTER_TO_INT(gx_list_fold (list, (GTernaryFunc)gx_times,
 *                       GINT_TO_POINTER(1), NULL, NULL));
 * ]|
 *
 * Returns: (transfer full): the product of the integers in @list.
 */
gint
gx_list_product (GList *list)
{
  gint product;
  for (product = 1; list; list = g_list_next (list))
    product *= GPOINTER_TO_INT (list->data);

  return product;
}


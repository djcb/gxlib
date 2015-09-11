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
 * SECTION:gxstr
 * @title: Working with strings
 * @short_description: functions to create and manipulate strings and
 * string-arrays.
 *
 * Some functions for common operations on strings and arrays of strings.
 */

static GList*
strv_to_list (gchar **strv, gssize n, gboolean copy)
{
  gint  i;
  GList *lst;
  
  if (n < 0)
    for (lst = NULL; *strv; ++strv) {
      lst = g_list_prepend (lst, copy ? g_strdup (*strv): *strv);
    }
  else
    for (lst = NULL, i = 0; i != n; ++i, ++strv)
      lst = g_list_prepend (lst, copy ? g_strdup (*strv): *strv);

  return g_list_reverse (lst);
}
  
/**
 * gx_strv_to_list:
 * @strv: an array of strings
 * @n: (allow-none): the number of strings in the array, or < 0 if it is
 * %NULL-terminated.
 *
 * Create a #GList from the string in @strv. The strings are not copied.
 * 
 * Returns: (transfer full): a list with the strings; free with g_list_free().
 */
GList*
gx_strv_to_list (gchar **strv, gssize n)
{
  return strv_to_list (strv, n, FALSE/*!copy*/);
}


/**
 * gx_strv_to_list_copy:
 * @strv: an array of strings
 * @n: (allow-none): the number of strings in the array, or < 0 if it is
 * %NULL-terminated.
 *
 * Create a #GList from the string in @strv and copy the strings.
 * 
 * Returns: (transfer full): a list with the strings; free with
 * g_list_free_full() using g_free().
 */
GList*
gx_strv_to_list_copy (gchar **strv, gssize n)
{
  return strv_to_list (strv, n, TRUE/*copy*/);
}



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
#include <string.h>

static void
test_strv_to_list (void)
{
  guint  u;
  GList *lst, *cur;
  const char *strv0[] = { NULL };
  const char *strv1[] = { "foo", "bar", "cuux" };
  const char *strv2[] = { "Amsterdam", "Paris", "London", "Helsinki", NULL };

  lst = gx_strv_to_list ((gchar**)strv0, -1);
  g_assert_cmpint (g_list_length(lst),==, 0);

  lst = gx_strv_to_list ((gchar**)strv0, 0);
  g_assert_cmpint (g_list_length(lst),==, 0);
  
  lst = gx_strv_to_list ((gchar**)strv1, G_N_ELEMENTS(strv1));
  g_assert_cmpint (g_list_length(lst),==, 3);
  for (u = 0, cur = lst; cur; cur = g_list_next(cur), ++u) 
    g_assert_cmpstr ((char*)cur->data, ==, strv1[u]);
  g_list_free (lst);
    
  lst = gx_strv_to_list ((gchar**)strv2, -1);
  g_assert_cmpint (g_list_length(lst),==, 4);
  for (u = 0, cur = lst; cur; cur = g_list_next(cur), ++u) 
    g_assert_cmpstr ((char*)cur->data, ==, strv2[u]);
  g_list_free (lst);
}



static void
test_strv_to_list_copy (void)
{
  guint  u;
  GList *lst, *cur;
  const char *strv0[] = { NULL };
  const char *strv1[] = { "foo", "bar", "cuux" };
  const char *strv2[] = { "Amsterdam", "Paris", "London", "Helsinki", NULL };

  lst = gx_strv_to_list_copy ((gchar**)strv0, -1);
  g_assert_cmpint (g_list_length(lst),==, 0);

  lst = gx_strv_to_list_copy ((gchar**)strv0, 0);
  g_assert_cmpint (g_list_length(lst),==, 0);
  
  lst = gx_strv_to_list_copy ((gchar**)strv1, G_N_ELEMENTS(strv1));
  g_assert_cmpint (g_list_length(lst),==, 3);
  for (u = 0, cur = lst; cur; cur = g_list_next(cur), ++u) 
    g_assert_cmpstr ((char*)cur->data, ==, strv1[u]);
  g_list_free_full (lst, g_free);
    
  lst = gx_strv_to_list_copy ((gchar**)strv2, -1);
  g_assert_cmpint (g_list_length(lst),==, 4);
  for (u = 0, cur = lst; cur; cur = g_list_next(cur), ++u) 
    g_assert_cmpstr ((char*)cur->data, ==, strv2[u]);
  g_list_free_full (lst, g_free);
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gx-str/strv-to-list", test_strv_to_list);
  g_test_add_func ("/gx-str/strv-to-list-copy", test_strv_to_list_copy);

  return g_test_run ();
}


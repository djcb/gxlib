/*
** Copyright (C) 2015 djcb <djcb@borealis>
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
test_identity (void)
{
  GList *lst, *cur;
  int i;
  
  g_assert_cmpint (123, ==, GPOINTER_TO_INT(gx_identity(GINT_TO_POINTER(123))));
  g_assert (NULL == gx_identity(NULL));

  lst = gx_list_iota (100, 1, 1);
  gx_list_map_in_place (lst, (GXBinaryFunc)gx_identity, NULL, NULL);
  for (i = 1, cur = lst; cur; cur = g_list_next(cur), ++i)
    g_assert_cmpint (i, ==, GPOINTER_TO_INT(cur->data));
    
  g_list_free (lst);
}


static void
test_plus (void)
{
  GList *lst;
  gpointer ptr;

  g_assert_cmpint (1 + 1, ==, gx_plus(1, 1));
  g_assert_cmpint (-1 + 1, ==, gx_plus(-1, 1));

  lst = gx_list_iota (100, 1, 1);
  ptr = gx_list_fold (lst, (GXTernaryFunc)gx_plus, GINT_TO_POINTER(0), NULL, NULL);
  g_assert_cmpint (GPOINTER_TO_INT(ptr), ==, 5050);
  
  g_list_free (lst);
}


static void
test_times (void)
{
  GList *lst;
  gpointer ptr;

  g_assert_cmpint (10 * 5, ==, gx_times (10, 5));
  g_assert_cmpint (-1 * 0, ==, gx_times (-1, 0));

  lst = gx_list_iota (5, 1, 1);
  ptr = gx_list_fold (lst, (GXTernaryFunc)gx_times, GINT_TO_POINTER(1), NULL, NULL);
  g_assert_cmpint (GPOINTER_TO_INT(ptr), ==, 120);
  
  g_list_free (lst);
}

static void
test_max (void)
{
  g_assert_cmpint (gx_max (1,-1), ==, 1);
  g_assert_cmpint (gx_max (-1, 1), ==, 1);
  g_assert_cmpint (gx_max (100, 1), ==, 100);
}


static void
test_min (void)
{
  g_assert_cmpint (gx_min (1,-1), ==, -1);
  g_assert_cmpint (gx_min (-1, 1), ==, -1);
  g_assert_cmpint (gx_min (100, 1), ==, 1);
}


static void
test_concat (void)
{
  char *s;
  GList *lst;
 
  lst = g_list_append (NULL,(gpointer)"This ");
  lst = g_list_append (lst, (gpointer)"is ");
  lst = g_list_append (lst, (gpointer)"a ");
  lst = g_list_append (lst, (gpointer)"sentence.");

  s = gx_list_fold (lst, (GXTernaryFunc)g_strconcat,
                    (gpointer)"", NULL, g_free);
  g_assert_cmpstr (s, ==, "This is a sentence.");
  g_free (s);
  g_list_free (lst);
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gx-func/identity", test_identity);
  g_test_add_func ("/gx-func/plus", test_plus);
  g_test_add_func ("/gx-func/times", test_times);
  g_test_add_func ("/gx-func/max", test_max);
  g_test_add_func ("/gx-func/min", test_min);
  g_test_add_func ("/gx-func/concat", test_concat);
  
  return g_test_run ();
}


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
test_filter (void)
{
  gint x;
  GList *lst, *cur, *even;

  lst = gx_list_filter (NULL, (GXPred)gx_is_even, NULL);
  g_assert (!lst);

  lst = gx_list_filter (NULL, (GXPred)gx_is_even, GINT_TO_POINTER(123));
  g_assert (!lst);
  
  lst = gx_list_iota (1000, 0, 1);
  g_assert_cmpint (g_list_length (lst), ==, 1000);
  
  even = gx_list_filter (lst, (GXPred)gx_is_even, NULL);
  g_assert_cmpint (g_list_length (even), ==, 500);
  
  for (x = 0, cur = even; cur; cur = g_list_next (cur),x+=2)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);

  g_list_free (lst);
  g_list_free (even);
}

static void
test_filter_in_place (void)
{
  gint x;
  GList *lst, *cur, *lst2;

  lst = gx_list_filter_in_place (NULL, (GXPred)gx_is_even, NULL, NULL);
  g_assert (!lst);

  lst = gx_list_filter_in_place (NULL, (GXPred)gx_is_even, GINT_TO_POINTER(123), NULL);
  g_assert (!lst);
  
  lst = gx_list_iota (1000, 0, 1);
  g_assert_cmpint (g_list_length (lst), ==, 1000);
  
  lst2 = gx_list_filter_in_place (lst, (GXPred)gx_is_even, NULL, NULL);
  g_assert (lst2 == lst);
  g_assert_cmpint (g_list_length (lst), ==, 500);
  
  for (x = 0, cur = lst; cur; cur = g_list_next (cur),x+=2)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);
  
  g_list_free (lst);
}

static gboolean
even_length (const char *s)
{
  return strlen (s) % 2 == 0 ? TRUE : FALSE;
}

static void
test_filter_in_place_free (void)
{
  gint x;
  GList *lst;
  const char* str;

  str = "Hello, world!";
  for (x = 0, lst = NULL; x != 10; ++x)
    lst = g_list_append (lst, g_strndup (str, x));
  
  lst = gx_list_filter_in_place (lst, (GXPred)even_length, NULL, g_free);
  g_assert_cmpint (g_list_length (lst), ==, 5);
  
  g_list_free_full (lst, g_free);
}

static void
test_every (void)
{
  GList *nums;

  g_assert (gx_list_every (NULL, (GXPred)gx_is_odd, NULL));
  
  nums = gx_list_iota (20, 1, 2);
  g_assert (gx_list_every (nums, (GXPred)gx_is_odd, NULL));
  g_assert (!gx_list_any (nums, (GXPred)gx_is_even, NULL));

  g_list_free (nums);
}


static void
test_any (void)
{
  GList *nums;

  g_assert (!gx_list_any (NULL, (GXPred)gx_is_odd, NULL));
  
  nums = gx_list_iota (20, 1, 1);
  nums = gx_list_filter_in_place (nums, (GXPred)gx_is_prime, NULL, NULL);
  
  g_assert (gx_list_any (nums, (GXPred)gx_is_even, NULL));
  g_assert (!gx_list_every (nums, (GXPred)gx_is_odd, NULL));

  g_list_free (nums);
}


static void
test_take (void)
{
  gint x;
  GList *lst, *cur, *lst2;

  lst = gx_list_take (NULL, 100);
  g_assert (!lst);
  
  lst = gx_list_iota (100, 0, 1);
  g_assert_cmpint (g_list_length (lst), ==, 100);

  lst2 = gx_list_take (lst, 0);
  g_assert (!lst2);

  lst2 = gx_list_take (lst, 15);
  g_assert_cmpint (g_list_length (lst2), ==, 15);
  
  for (x = 0, cur = lst2; cur; cur = g_list_next (cur),++x)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);
  
  g_list_free (lst);
  g_list_free (lst2);
}

static void
test_take_in_place (void)
{
  gint x;
  GList *lst, *cur, *lst2;
  const char *words[] = { "butter", "bread", "green", "cheese" };

  lst = gx_list_take_in_place (NULL, 234, NULL);
  g_assert (!lst);
  
  lst = gx_strv_to_list_copy ((char**)words, G_N_ELEMENTS(words));
  g_assert_cmpint (g_list_length (lst), ==, 4);

  lst2 = gx_list_take_in_place (lst, 0, g_free);
  g_assert (!lst2);

  lst = gx_strv_to_list ((char**)words, G_N_ELEMENTS(words));
  g_assert_cmpint (g_list_length (lst), ==, 4);
  
  lst2 = gx_list_take_in_place (lst, 2, NULL);
  g_assert_cmpint (g_list_length (lst2), ==, 2);
  
  for (x = 0, cur = lst2; cur; cur = g_list_next (cur),++x)
    g_assert_cmpstr (cur->data,==,words[x]);
  g_list_free (lst2);

  lst = gx_strv_to_list ((char**)words, G_N_ELEMENTS(words));
  lst = gx_list_take_in_place (lst, 1000, 0);
  g_assert_cmpint (g_list_length (lst), ==, 4);

  g_list_free (lst);
}


static void
test_skip (void)
{
  gint x;
  GList *lst, *cur, *lst2;

  lst = gx_list_skip (NULL, 100);
  g_assert (!lst);
  
  lst = gx_list_iota (100, 0, 1);
  g_assert_cmpint (g_list_length (lst), ==, 100);

  lst2 = gx_list_skip (lst, 0);
  g_assert_cmpint (g_list_length (lst2), ==, 100);
  g_assert (lst2 != lst);
  g_list_free (lst2);
    
  lst2 = gx_list_skip (lst, 15);
  g_assert_cmpint (g_list_length (lst2), ==, 85);
  
  for (x = 15, cur = lst2; cur; cur = g_list_next (cur),++x)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);
  
  g_list_free (lst);
  g_list_free (lst2);
}


static void
test_skip_in_place (void)
{
  gint x;
  GList *lst, *cur, *lst2;
  const char *words[] = { "butter", "bread", "green", "cheese" };

  lst = gx_list_skip_in_place (NULL, 234, NULL);
  g_assert (!lst);
  
  lst = gx_strv_to_list_copy ((char**)words, G_N_ELEMENTS(words));
  g_assert_cmpint (g_list_length (lst), ==, 4);

  lst2 = gx_list_skip_in_place (lst, 0, g_free);
  g_assert_cmpint (g_list_length (lst2), ==, 4);
  
  lst2 = gx_list_skip_in_place (lst, 2, g_free);
  g_assert_cmpint (g_list_length (lst2), ==, 2);
  
  for (x = 2, cur = lst2; cur; cur = g_list_next (cur),++x)
    g_assert_cmpstr (cur->data,==,words[x]);
  g_list_free_full (lst2, g_free);
}


static int
square (gint num)
{
  return num * num;
}


static int
cube (gint num)
{
  return num * num * num;
}


static void
test_map (void)
{
  gint x;
  GList *lst, *cur, *squares;
  
  lst = gx_list_map (NULL, (GXBinaryFunc)cube, NULL);
  g_assert (!lst);

  lst = gx_list_map (NULL, (GXBinaryFunc)square, (gpointer)"test");
  g_assert (!lst);
  
  lst = gx_list_iota (1000, 0, 1);
  g_assert_cmpint (g_list_length (lst), ==, 1000);

  squares = gx_list_map (lst, (GXBinaryFunc)square, NULL);

  g_assert_cmpint (g_list_length (squares), ==, 1000);
  
  for (x = 0, cur = squares; cur; cur = g_list_next (cur),x++)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x*x);
  
  g_list_free (lst);
  g_list_free (squares);
}



static void
test_map_in_place (void)
{
  gint x;
  GList *lst, *lst2,  *cur;

  lst2 = gx_list_map_in_place (NULL, (GXBinaryFunc)cube, NULL, NULL);
  g_assert (!lst2);

  lst2 = gx_list_map_in_place (NULL, (GXBinaryFunc)square,
                               (gpointer)"test", NULL);
  g_assert (!lst2);
  
  lst = gx_list_iota (77, 1, 5);
  g_assert_cmpint (g_list_length (lst), ==, 77);

  lst2 = gx_list_map_in_place (lst, (GXBinaryFunc)cube, NULL, NULL);
  g_assert (lst2 == lst);
  g_assert_cmpint (g_list_length (lst), ==, 77);

  for (x = 1, cur = lst; cur; cur = g_list_next (cur), x+=5)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x*x*x);

  g_list_free (lst);
}


static char*
upcase (char *s)
{
  return g_ascii_strup (s, -1);
}


static void
test_map_in_place_free (void)
{
  gint x;
  GList *lst, *cur;
  const char *letters;

  for (lst = NULL, x = 0; x != 10; ++x) {
    char buf[2];
    buf [0] = 'a' + x;
    buf [1] = '\0';
    lst = g_list_append (lst, g_strdup (buf));
  }
  
  gx_list_map_in_place (lst, (GXBinaryFunc)upcase, NULL, g_free);
  letters = "ABCDEFGHIJ";
  for (x = 0, cur = lst; cur; cur = g_list_next(cur), ++x)
    g_assert_cmpint (((char*)cur->data)[0], ==, letters[x]);
      
  g_list_free_full (lst, g_free);
}



static void
test_fold (void)
{
  gint sum;
  GList *lst;
  
  lst = gx_list_iota (100, 1, 1);
  g_assert_cmpint (g_list_length (lst), ==, 100);

  sum = GPOINTER_TO_INT(gx_list_fold (lst, (GXTernaryFunc)gx_plus,
				      NULL, GINT_TO_POINTER(0), NULL));
  g_assert_cmpint (sum, ==, 5050);
  
  g_list_free (lst);
}

static void
test_sum (void)
{
  GList *lst;
  
  lst = gx_list_iota (100, 1, 1);
  g_assert_cmpint (g_list_length (lst), ==, 100);
  g_assert_cmpint (gx_list_sum(lst),==,5050);
  
  g_list_free (lst);
}



static void
test_product (void)
{
  GList *lst;
  
  lst = gx_list_iota (5, 1, 1);
  g_assert_cmpint (g_list_length (lst), ==, 5);
  g_assert_cmpint (gx_list_product(lst),==,120);
  
  g_list_free (lst);
}




static void
test_iota (void)
{
  gint	 x;
  GList *lst, *cur;

  lst = gx_list_iota (0, 0, 1);
  g_assert (!lst);

  lst = gx_list_iota (10, 1, 1);
  g_assert_cmpint (g_list_length (lst), ==, 10);
  for (x = 1, cur = lst; cur; cur = g_list_next (cur),++x)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);
  g_list_free (lst);

  lst = gx_list_iota (100, 10, 5);
  g_assert_cmpint (g_list_length (lst), ==, 100);  
  for (x = 10, cur = lst; cur; cur = g_list_next (cur), x +=5)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);
  g_list_free (lst);

  lst = gx_list_iota (77, 7, 7);
  g_assert_cmpint (g_list_length (lst), ==, 77);
  for (x = 7, cur = lst; cur; cur = g_list_next (cur), x +=7)
    g_assert_cmpint (GPOINTER_TO_INT(cur->data),==,x);
  g_list_free (lst);
}

int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gx-list/filter", test_filter);
  g_test_add_func ("/gx-list/filter-in-place", test_filter_in_place);
  g_test_add_func ("/gx-list/filter-in-place-free", test_filter_in_place_free);
  g_test_add_func ("/gx-list/every", test_every);
  g_test_add_func ("/gx-list/any", test_any);
  g_test_add_func ("/gx-list/take", test_take);
  g_test_add_func ("/gx-list/take-in-place", test_take_in_place);
  g_test_add_func ("/gx-list/skip", test_skip);
  g_test_add_func ("/gx-list/skip-in-place", test_skip_in_place);
  g_test_add_func ("/gx-list/map", test_map);
  g_test_add_func ("/gx-list/map-in-place", test_map_in_place);
  g_test_add_func ("/gx-list/map-in-place-free", test_map_in_place_free);  
  g_test_add_func ("/gx-list/fold", test_fold);
  g_test_add_func ("/gx-list/iota", test_iota);
  g_test_add_func ("/gx-list/sum", test_sum);
  g_test_add_func ("/gx-list/product", test_product);

  return g_test_run ();
}


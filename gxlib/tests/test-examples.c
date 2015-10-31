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
#include <stdlib.h>

static void
example_funcs (void)
{
  const char *str;
  str =  "FOO";
  
  g_assert (gx_identity (str) == str);
  
  g_assert_cmpint (gx_plus (3, 4), ==, 3 + 4);
  g_assert_cmpint (gx_times (8, 7), ==, 8 * 7);
  g_assert_cmpint (gx_max (100, 1000), ==, MAX(100, 1000));
  g_assert_cmpint (gx_min (123456, 54321), ==, MIN(123456, 54321));
}



static void
example_preds (void)
{
  g_assert_cmpuint (gx_is_even (2),==, TRUE);
  g_assert_cmpuint (gx_is_even (3),==, FALSE);

  g_assert_cmpuint (gx_is_odd (2),==, FALSE);
  g_assert_cmpuint (gx_is_odd (3),==, TRUE);

  g_assert_cmpuint (gx_is_prime (13),==, TRUE);
  g_assert_cmpuint (gx_is_prime (52),==, FALSE);

}


static void
example_prod5_5 (void)
{
  GList *lst;
  int    sum;
  
  lst = gx_list_iota (5, 5, 5); // 5, 10, 15, 20, 25
  sum = GPOINTER_TO_INT(gx_list_fold (lst, (GXTernaryFunc)gx_times,
                                      GINT_TO_POINTER(1), NULL, NULL));
  g_assert_cmpint (sum,==, 375000);
  g_list_free (lst);
}


static gboolean div_3_5 (gint n) { return n % 3 == 0 || n % 5 == 0; }

static void
example_filter (void)
{
  GList *lst, *filtered;
  
  lst = gx_list_iota (100, 1, 1); // getting a list 1..100
  filtered = gx_list_filter (lst, (GXPred)div_3_5, NULL);

  g_assert_cmpint (g_list_length (filtered),==, 47); 

  g_list_free (lst);
  g_list_free (filtered);
}


static void
example_take (void)
{
  const char* words[] = { "foo", "bar", "cuux", NULL };
  GList *lst, *lst2;

  lst = gx_strv_to_list ((char**)words, -1);
  g_assert_cmpint (g_list_length (lst), ==, 3);

  lst2 = gx_list_take (lst, 2);
  
  g_assert_cmpint (g_list_length (lst2), ==, 2);
  g_assert_cmpstr (g_list_nth_data (lst2, 1),==,"bar");

  g_list_free (lst);
  g_list_free (lst2);
}

static void
example_map (void)
{
  GList *lst, *upper;
  const char* cities[] = { "Aruba", "Hawaii", "Zanzibar", NULL };
  
  lst = gx_strv_to_list ((gchar**)cities, -1);
  upper = gx_list_map (lst, (GXBinaryFunc)g_ascii_strup, GINT_TO_POINTER(-1));

  g_assert_cmpint (g_list_length(upper),==,3);

  g_assert_cmpstr (g_list_nth_data(upper, 0), ==, "ARUBA");
  g_assert_cmpstr (g_list_nth_data(upper, 1), ==, "HAWAII");
  g_assert_cmpstr (g_list_nth_data(upper, 2), ==, "ZANZIBAR");
  
  g_list_free (lst);
  g_list_free_full (upper, g_free);
}



static void
example_every (void)
{
  GList *lst;
  lst =  gx_list_iota (10, 1, 1);
  g_assert_false (gx_list_every (lst, (GXPred)gx_is_prime, NULL));
  g_list_free (lst);
}



static void
example_any (void)
{
  GList *lst;
  lst =  gx_list_iota (10, 20, 1);
  g_assert_true (gx_list_any (lst, (GXPred)gx_is_prime, NULL));
  g_list_free (lst);
}


static void
example_plus (void)
{
  GList *lst;
  gint   sum;

  // calculate the sum of numbers 1..100. Alternatively, n * (n - 1) / 2...
  lst = gx_list_iota (100, 1, 1);
  sum = GPOINTER_TO_INT (gx_list_fold (lst, (GXTernaryFunc)gx_plus, 0, NULL, NULL));
  g_list_free (lst);
  g_assert_cmpint (sum, ==, 5050);
}


static void
example_iota (void)
{
  GList *lst;

  // get a list of the first 100 even numbers
  lst = gx_list_iota (100, 0, 2);

  g_assert_cmpint (g_list_length(lst),==,100);
  {
    int x;
    GList *cur;
    
    for (x= 0, cur = lst; cur; cur = g_list_next (cur), x += 2)
      g_assert_cmpint (GPOINTER_TO_INT(cur->data), ==, x);
  }

  g_list_free (lst);
}




static void
example_pred (void)
{
  GList *lst;
  // get a list of the prime numbers up 100 (inclusive)
  lst = gx_list_filter_in_place (gx_list_iota (100, 1, 1), (GXPred)gx_is_prime, NULL, NULL);
  // ...
  g_assert_cmpint (g_list_length(lst),==,25);
  /* { */
  /*   GList *cur; */
  /*   for (cur = lst; cur; cur = g_list_next(cur)) */
  /*     g_print ("%d ", GPOINTER_TO_INT(cur->data)); */
  /* } */
  
  g_list_free (lst);
}


static void
example_max (void)
{
  GList *lst, *cur;
  guint  u, max;

  // a list with 100 random numbers 0..99
  for (u = 0, lst = NULL; u != 100; ++u)
    lst = g_list_prepend (lst, GINT_TO_POINTER(g_random_int_range (0, 100)));

  // the highest number; use the first item in the list as 'seed'
  max = GPOINTER_TO_UINT (gx_list_fold (lst, (GXTernaryFunc)gx_max, lst->data, NULL, NULL));

  {
    guint max2;
    max2 = GPOINTER_TO_UINT(lst->data);
    for (cur = lst; cur; cur = g_list_next (cur))
      {
        guint num;
        num = GPOINTER_TO_UINT(cur->data);
        max2 = num > max2 ? num : max2;
      }
    g_assert_cmpuint (max, ==, max2);
  }

  g_list_free (lst);
}


static void
example_primes_100 (void)
{
  gint sum;
  GList *nums;

  // numbers 1..100 (inclusive)
  nums = gx_list_iota (100, 1, 1);
  // filter out the non-primes
  nums = gx_list_filter_in_place (nums, (GXPred)gx_is_prime, NULL, NULL);
  // take the sum
  sum = gx_list_sum (nums);

  g_assert_cmpint (sum, ==, 1060);
  g_list_free (nums);
}


static void
example_primes_prod_20 (void)
{
  gint prod;
  GList *nums;

  // numbers 1..20 (inclusive)
  nums = gx_list_iota (20 /*number*/, 1/*start*/, 1/*step*/);
  // filter out the non-primes
  nums = gx_list_filter_in_place (nums, (GXPred)gx_is_prime, NULL, NULL);
  // take the sum
  prod = gx_list_product (nums);

  g_assert_cmpint (prod, ==, 9699690);
  g_list_free (nums);
}


static char*
chain (const char *s1, const char *s2, const char *sepa)
{
    return s1 ? g_strconcat (s1, sepa, s2, NULL) : g_strdup (s2);
}

static void
example_upper_chain (void)
{
  GList *lst;
  char *str;
  const char *letters[] = { "a", "b", "c", "d", NULL};
  
  lst = gx_strv_to_list ((char**)letters, -1);
  lst = gx_list_map_in_place (lst, (GXBinaryFunc)g_ascii_strup,
                              GINT_TO_POINTER(-1), NULL);

  str = gx_list_fold (lst, (GXTernaryFunc)chain, NULL,
                      (gpointer)":", g_free);
  g_assert_cmpstr (str,==,"A:B:C:D");
  g_free (str);

  g_list_free_full (lst, g_free);
}

static void
example_chain (void)
{
  GList *lst;
  char  *str;
  const char* cities[] = { "Amsterdam", "San Francisco", "Helsinki", NULL };
  
  lst = gx_strv_to_list ((gchar**)cities, -1);
  str = gx_list_fold (lst, (GXTernaryFunc)chain, NULL,
                      (gpointer)"; ", g_free);

  g_assert_cmpstr (str, ==, "Amsterdam; San Francisco; Helsinki");

  g_free (str);
  g_list_free (lst);
}


static gboolean verbose;
static gboolean beep;
static gint     count;

static GOptionEntry main_entries[] =
  {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
    { NULL }
  };

static GOptionEntry add_entries[] =
  {
    { "beep", 'b', 0, G_OPTION_ARG_NONE, &beep, "Beep after adding", NULL },
    { NULL }
  };

static GOptionEntry remove_entries[] =
  {
    { "count", 'c', 0, G_OPTION_ARG_INT, &count, "Number of items to remove", "N" },
    { NULL }
  };


static gboolean
handle_add (const char **rest, gpointer data, GError **err)
{
  // implement the 'add' subcommand
  return TRUE;
}

static gboolean
handle_remove (const char **rest, gpointer data, GError **err)
{
  // implement the 'remove' subcommand
  return TRUE;
}


static int
test_main (int argc, char *argv[])
{
  gboolean                   rv;
  GOptionContext            *o_ctx;
  GXSubCommandOptionContext *sc_ctx;
  GOptionGroup              *og;
  GError                    *err;

  o_ctx = g_option_context_new ("- add or remove items");
  g_option_context_add_main_entries (o_ctx, main_entries, "items");

  sc_ctx = gx_sub_command_option_context_new (o_ctx);
  
  og = g_option_group_new ("add", "the add subcommand", "add", NULL, NULL);
  g_option_group_add_entries (og, add_entries);
  gx_sub_command_option_context_add_group (sc_ctx, "add",
                                           "adding an item",
                                           "longer explanation",
                                           og,
                                           (GXSubCommandFunc)handle_add, NULL);

  og = g_option_group_new ("remove", "the remove subcommand", "remove", NULL, NULL);
  g_option_group_add_entries (og, remove_entries);
  gx_sub_command_option_context_add_group (sc_ctx, "remove",
                                           "adding an item",
                                           "longer explanation",
                                           og,
                                           (GXSubCommandFunc)handle_remove, NULL);
  err = NULL;
  rv = gx_sub_command_option_context_parse (sc_ctx, &argc, &argv, &err);
  if (!rv)
    {
      g_printerr ("error: %s", err ? err->message : "something went wrong");
    }

  rv = gx_sub_command_option_context_execute (sc_ctx, &err);
  if (!rv)
    {
      g_printerr ("error: %s", err ? err->message : "something went wrong");
    }
  
  g_clear_error (&err);
  gx_sub_command_option_context_free (sc_ctx);

  return rv ? 0 : 1;
}


static void
example_sub_command (void)
{
  const char *argv[] = {"items", "--verbose", "add", "--beep"};
  int         argc = G_N_ELEMENTS(argv);

  
  g_assert_cmpint (test_main (argc, (char**)argv), ==, 0);
}

static void
example_map_fold (void)
{
  GList *nums;
  int    greatest;

  /* turn into a list,  shallow copy */
  const char *numstrv[] = { "3", "48", "22", "73", "55" };
  nums = gx_strv_to_list ((char**)numstrv, G_N_ELEMENTS(numstrv)); 

  /* change the list of strings, in-place, in to a list of numbers */
  gx_list_map_in_place (nums, (GXBinaryFunc)atoi, NULL, NULL);

  greatest = GPOINTER_TO_INT(gx_list_fold (nums, (GXTernaryFunc)gx_max,
                                           GINT_TO_POINTER(0), NULL, NULL));
  g_assert_cmpint (greatest,==,73);
  
  g_list_free (nums);
}



int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/example/funcs", example_funcs);
  g_test_add_func ("/example/preds", example_preds);
  g_test_add_func ("/example/prod5-5", example_prod5_5);
  g_test_add_func ("/example/filter", example_filter);
  g_test_add_func ("/example/take", example_take);
  g_test_add_func ("/example/map", example_map);
  g_test_add_func ("/example/every", example_every);
  g_test_add_func ("/example/any", example_any);
  g_test_add_func ("/example/iota", example_iota);
  g_test_add_func ("/example/plus", example_plus);
  g_test_add_func ("/example/pred", example_pred);
  g_test_add_func ("/example/max", example_max);
  g_test_add_func ("/example/primes-100", example_primes_100);
  g_test_add_func ("/example/primes-prod-20", example_primes_prod_20);
  g_test_add_func ("/example/upper-chain", example_upper_chain);
  g_test_add_func ("/example/chain", example_chain);
  g_test_add_func ("/example/sub-command", example_sub_command);
  g_test_add_func ("/example/map-fold", example_map_fold);
 
  return g_test_run ();
}


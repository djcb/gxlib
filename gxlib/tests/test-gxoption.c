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

static gboolean  frobnicate, foo_called;
static int       level;
static char     *color;

static GOptionEntry main_entries[] =
  {
    { "color", 'c', 0, G_OPTION_ARG_STRING, &color,
      "Set the color", "C" },
    { NULL}
  };

static GOptionEntry foo_entries[] =
  {
    { "frobnicate", 'f', 0, G_OPTION_ARG_NONE, &frobnicate,
      "Frobnicate the flux", NULL },
    { NULL}
  };

static GOptionEntry bar_entries[] =
  {
    { "level", 'l', 0, G_OPTION_ARG_INT, &level,
      "Set the level", "L" },
    { NULL}
  };


static gboolean
handle_foo (const char **rest, gpointer data, GError **err)
{
  foo_called = TRUE;
  return TRUE;
}

static gboolean
handle_bar (const char **rest, gpointer data, GError **err)
{
  g_assert_not_reached ();
  return FALSE;
}



static void
test_sub_command (void)
{
  GError                     *err;
  GOptionContext             *ctx;
  GOptionGroup               *og;
  GXSubCommandOptionContext  *mctx;
  const char                **argv;
  gint                        argc;
  gboolean                    rv;
  
  ctx = g_option_context_new ("- test");
  g_option_context_add_main_entries (ctx, main_entries, "test");

  mctx = gx_sub_command_option_context_new (ctx);
  
  og = g_option_group_new ("foo", "the foo subcommand", "help", NULL, NULL);
  g_option_group_add_entries (og, foo_entries);
  gx_sub_command_option_context_add_group (mctx, "foo", og,
                                           (GXSubCommandFunc)handle_foo,
                                           NULL);
  
  og = g_option_group_new ("bar", "the bar subcommand", "help", NULL, NULL);
  g_option_group_add_entries (og, bar_entries);
  gx_sub_command_option_context_add_group (mctx, "bar", og,
                                           (GXSubCommandFunc)handle_bar,
                                           NULL);
  argv = g_new (const gchar*, 5);
  
  argv[0] = "test";
  argv[1] = "--color=blue";
  argv[2] = "foo";
  argv[3] = "--frobnicate";
  argv[4] = NULL;

  argc = 4;
  err  = NULL;
  rv   = gx_sub_command_option_context_parse (mctx, &argc,
                                              (char***)&argv, &err);
  g_assert_no_error (err);
  g_assert_true (rv);
  g_assert_cmpstr (color,==,"blue");
  g_assert_true (frobnicate);
 
  rv = gx_sub_command_option_context_execute (mctx, &err);
  g_assert_no_error (err);
  g_assert (rv);
  g_assert_true (foo_called);

  g_clear_pointer (&color, g_free);
  g_free (argv);

  gx_sub_command_option_context_free (mctx);
}


static void
test_sub_command_error_1 (void)
{
  GError                     *err;
  GOptionContext             *ctx;
  GXSubCommandOptionContext  *mctx;
  const char                **argv;
  gint                        argc;
  gboolean                    rv;
  
  ctx  = g_option_context_new ("- test-error");
  g_option_context_add_main_entries (ctx, main_entries, "test-error");
  mctx = gx_sub_command_option_context_new (ctx);

  argv = g_new (const gchar*, 4);
  
  argv[0] = "test";
  argv[1] = "cuux";
  argv[2] = "--foo=bla";
  argv[3] = NULL;

  argc = 3;
  err  = NULL;
  rv   = gx_sub_command_option_context_parse (mctx, &argc,
                                              (char***)&argv, &err);
  g_assert_error (err, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE);
  g_assert_false (rv);

  g_clear_error (&err);
  g_clear_pointer (&color, g_free);
  
  g_free (argv);
  gx_sub_command_option_context_free (mctx);
}


static void
test_sub_command_error_2 (void)
{
  GError                     *err;
  GOptionContext             *ctx;
  GOptionGroup               *og;
  GXSubCommandOptionContext  *mctx;
  const char                **argv;
  gint                        argc;
  gboolean                    rv;
  
  ctx = g_option_context_new ("- test");
  g_option_context_add_main_entries (ctx, main_entries, "test");

  mctx = gx_sub_command_option_context_new (ctx);
 
  og = g_option_group_new ("bar", "the bar subcommand", "help", NULL, NULL);
  g_option_group_add_entries (og, bar_entries);
  gx_sub_command_option_context_add_group (mctx, "bar", og,
                                           (GXSubCommandFunc)handle_bar,
                                           NULL);
  argv = g_new (const gchar*, 5);
  
  argv[0] = "test";
  argv[1] = "--color=blue";
  argv[2] = "bar";
  argv[3] = "--cuux=foo";
  argv[4] = NULL;

  argc = 4;
  err  = NULL;
  rv   = gx_sub_command_option_context_parse (mctx, &argc, (char***)&argv, &err);
  g_assert (err);
  g_assert_false (rv);
  g_clear_pointer (&color, g_free);

  g_clear_error (&err);
  g_free (argv);
  gx_sub_command_option_context_free (mctx);
}


int
main (int argc, char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/gx-option/sub-command", test_sub_command);
  g_test_add_func ("/gx-option/sub-command-error-1", test_sub_command_error_1);
  g_test_add_func ("/gx-option/sub-command-error-2", test_sub_command_error_2);
  
  return g_test_run ();
}


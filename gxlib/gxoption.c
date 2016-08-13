/*
** Copyright (C) 2015, 2010 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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

#include <glib/gi18n.h>
#include "gxoption.h"

/**
 * SECTION:gxoption
 * @title: Command-line parsing
 * @short_description: parsing command-line parameters
 *
 * #GXSubCommandOptionContext is a wrapper for #GOptionContext to support
 * "sub-commands", that is, command-line tools that offer a number of commands,
 * each with their specific options -- examples include "git", "openssl", "mu".
 *
 * In the example below, we define a program with two sub-commands, "add" and
 * "remove", each with their specific options.
 *
 * |[<!-- language="C" -->
 * static gboolean verbose;
 * static gboolean beep;
 * static gint     count;
 *
 * static GOptionEntry main_entries[] =
 *   {
 *     { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
 *     { NULL }
 *   };
 *
 * static GOptionEntry add_entries[] =
 *   {
 *     { "beep", 'b', 0, G_OPTION_ARG_NONE, &beep, "Beep after adding", NULL },
 *     { NULL }
 *   };
 *
 * static GOptionEntry remove_entries[] =
 *   {
 *     { "count", 'c', 0, G_OPTION_ARG_INT, &count, "Number of items to remove", "N" },
 *     { NULL }
 *   };
 *
 *
n * static gboolean
 * handle_add (const char **rest, gpointer data, GError **err)
 * {
 *   // implement the 'add' subcommand
 *   return TRUE;
 * }
 *
 * static gboolean
 * handle_remove (const char **rest, gpointer data, GError **err)
 * {
 *   // implement the 'remove' subcommand
 *   return TRUE;
 * }
 *
 *
 * int
 * main (int argc, char *argv[])
 * {
 *   gboolean                   rv;
 *   GOptionContext            *o_ctx;
 *   GXSubCommandOptionContext *sc_ctx;
 *   GOptionGroup              *og;
 *   GError                    *err;
 *
 *   o_ctx = g_option_context_new ("- add or remove items");
 *   g_option_context_add_main_entries (o_ctx, main_entries, "items");
 *
 *   sc_ctx = gx_sub_command_option_context_new (o_ctx);
 *
 *   og = g_option_group_new ("add", "the add subcommand", "add", NULL, NULL);
 *   g_option_group_add_entries (og, add_entries);
 *   gx_sub_command_option_context_add_group (sc_ctx, "add", og,
 *                                            (GXSubCommandFunc)handle_add, NULL);
 *
 *   og = g_option_group_new ("remove", "the remove subcommand", "remove", NULL, NULL);
 *   g_option_group_add_entries (og, remove_entries);
 *   gx_sub_command_option_context_add_group (sc_ctx, "remove", og,
 *                                            (GXSubCommandFunc)handle_remove, NULL);
 *   err = NULL;
 *   rv = gx_sub_command_option_context_process (sc_ctx, &argc, &argv, &err);
 *   if (!rv)
 *     {
 *       g_printerr ("error: %s", err ? err->message : "something went wrong");
 *     }
 *
 *   g_clear_error (&err);
 *   gx_sub_command_option_context_free (sc_ctx);
 *
 *   return rv ? 0 : 1;
 * }
 * ]|
 */


struct _GXSubCommandOptionContext
{
  GOptionContext	 *ctx;
  GQueue		 *groups;
  struct _OGroup	 *group;
  char			**rest;
};

struct _OGroup
{
  char			*name;
  char			*oneline;
  char			*description;
  GOptionGroup		*option_group;
  GXSubCommandFunc	 func;
  gpointer		 user_data;
};

typedef struct _OGroup OGroup;

static void
ogroup_free (OGroup *ogroup)
{
  g_free (ogroup->name);
  g_free (ogroup->description);
  g_free (ogroup->oneline);

  if (ogroup->option_group)
    g_option_group_unref (ogroup->option_group);

  g_free (ogroup);
}


/**
 * gx_sub_command_option_context_new:
 * @context:(transfer full): a #GOptionContext with the main (non-subcommand)
 * options
 *
 * Create a new #GXSubCommandOptionContext, which takes ownership of @context
 *
 * Return value:(transfer full): the new #GXSubCommandOptionContext; free with
 * gx_sub_command_context_option_free().
 */
GXSubCommandOptionContext*
gx_sub_command_option_context_new (GOptionContext *context)
{
  GXSubCommandOptionContext *mctx;

  g_return_val_if_fail (context, NULL);

  mctx      = g_new0 (GXSubCommandOptionContext, 1);

  mctx->groups = g_queue_new ();
  mctx->ctx    = context;

  return mctx;
}


/**
 * gx_sub_command_option_context_free:
 * @context: a #GXSubCommandOptionContext instance
 *
 * Free a #GXSubCommandOptionContext instance
 */
void
gx_sub_command_option_context_free (GXSubCommandOptionContext *context)
{
  g_return_if_fail (context);

  g_option_context_free (context->ctx);

  g_queue_free_full (context->groups, (GDestroyNotify)ogroup_free);
  g_strfreev (context->rest);

  g_free (context);
}

static OGroup*
find_ogroup (GQueue *groups, const char *name)
{
  GList *cur;
  for (cur = groups->head; cur; cur = g_list_next(cur))
    {
      OGroup *ogroup;
      ogroup = (OGroup*)cur->data;

      if (g_strcmp0 (ogroup->name, name) == 0)
        return ogroup;
    }

  return NULL;
}


static gboolean
cmd_help (const char **rest, GXSubCommandOptionContext *context, GError **error)
{
  OGroup *ogroup;
  char *help;

  if (!rest || !rest[0])
    {
      GList   *cur;
      g_print ("%s", _("Use help <sub-command> to get specific help, where "
                       "<subcommand> is one of:\n"));
      for (cur = context->groups->head; cur; cur = g_list_next(cur))
        {
          ogroup = (OGroup*)cur->data;
          g_print ("  %-14s %s\n", ogroup->name, ogroup->oneline);
        }
      return TRUE;
    }

  ogroup = find_ogroup (context->groups, rest[0]);
  if (!ogroup)
    {
      g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                   _("Unknown sub-command '%s'"), rest[0]);
      return FALSE;
    }

  g_print (_("%s - "), rest[0]);
  if (ogroup->description)
    g_print ("%s\n\n", ogroup->oneline);

  g_option_context_set_description (context->ctx, "");
  g_option_context_set_summary (context->ctx, ogroup->description);

  if (ogroup->option_group)
    {
      g_option_context_add_group (context->ctx, ogroup->option_group);
      help = g_option_context_get_help (context->ctx, TRUE, ogroup->option_group);
      if (help)
        g_print ("%s", help);

      g_free (help);
    }

  return TRUE;
}

static void
add_help_command (GXSubCommandOptionContext *context)
{
  OGroup *ogroup;

  ogroup               = g_new0 (OGroup, 1);
  ogroup->name         = g_strdup ("help");
  ogroup->oneline      = g_strdup (_("Get help about commands"));
  ogroup->description  = g_strdup (_("Get help about commands"));
  ogroup->option_group = NULL;
  ogroup->func         = (GXSubCommandFunc)cmd_help;
  ogroup->user_data    = context;

  g_queue_push_tail (context->groups, ogroup);

}

/**
 * gx_sub_command_option_context_add_group:
 * @context: a #GXSubCommandOptionContext
 * @sub_command: the name of the sub-command. Should be the same as the name of
 * @option_group.
 * @option_group:(allow-none): the #GOptionGroup for this subcommand or %NULL.
 * @func: (allow-none): a #GXSubCommandFunc function that handles this
 * sub-command
 * @user_data: user pointer passed to @func
 *
 * Add a sub-command with a set of options, and optionally a function to be
 * called for the subcommand.
 */
void
gx_sub_command_option_context_add_group (GXSubCommandOptionContext *context,
                                         const char *group_name,
                                         const char *oneline,
                                         const char *description,
                                         GOptionGroup *option_group,
                                         GXSubCommandFunc func,
                                         gpointer user_data)
{
  OGroup *ogroup;

  g_return_if_fail (context);
  g_return_if_fail (group_name);

  if (context->groups->length == 0)
    add_help_command (context);

  ogroup               = g_new0 (OGroup, 1);
  ogroup->name         = g_strdup (group_name);
  ogroup->oneline      = g_strdup (oneline);
  ogroup->description  = g_strdup (description);
  ogroup->option_group = option_group;
  ogroup->func         = func;
  ogroup->user_data    = user_data;

  /* make sure "help" is always the last item */
  g_queue_insert_before (context->groups, context->groups->tail, ogroup);
}


static void
group_help (GXSubCommandOptionContext *context)
{
  GList *cur;
  const char *s;

  s = g_option_context_get_description (context->ctx);
  if (s)
    g_print ("%s\n", s);

  s = g_option_context_get_summary (context->ctx);
  if (s)
    g_print ("%s\n", s);

  g_print (_("Available sub-commands:\n"));
  for (cur = context->groups->head; cur; cur = g_list_next(cur))
    {
      OGroup *ogroup;
      ogroup = (OGroup*)cur->data;

      g_print ("  %-14s %s\n", ogroup->name,
               ogroup->oneline ? ogroup->oneline : "");
    }
}


/**
 * gx_sub_command_option_context_parse:
 * @context: a #GXSubCommandOptionContext instance
 * @argc: (inout) (allow-none): a pointer to the number of command-line arguments
 * @argv: (inout) (allow-none) (array length=argc): a pointer to the array of
 * command-line arguments
 * @error: (allow-none): receives error information
 *
 * Parses the command-line options, using the main command-line options and the
 * options for appropriate sub-command (if any). If a sub-command function was
 * set with gx_sub_command_option_context_add_group(), that function is invoked
 * for the given group.
 *
 * See g_option_context_parse() for some more details about the parsing.
 *
 * Return value: If a sub-command was recognized and it defined a
 * #GSubCommandFunc, returns the result of the invocation. Otherwise, returns
 * %TRUE if parsing worked, %FALSE otherwise.
 */
gboolean
gx_sub_command_option_context_parse (GXSubCommandOptionContext *context,
                                     gint *argc, gchar ***argv, GError **error)
{
  gint      i;
  gboolean  rv;
  OGroup   *ogroup;

  g_return_val_if_fail (context, FALSE);
  g_return_val_if_fail (argc, FALSE);
  g_return_val_if_fail (argv, FALSE);

  /* find the first non-option parameter */
  for (ogroup = NULL, i = 1; i < *argc; ++i)
    {
      if ((*argv)[i][0] != '-')
        {
          int j;
          ogroup = find_ogroup (context->groups, (*argv)[i]);
          if (!ogroup)
            {
              g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                           _("Unknown sub-command '%s'"), (*argv)[i]);
              return FALSE;
            }

          if (ogroup->option_group)
            g_option_context_add_group (context->ctx,
                                        g_option_group_ref (ogroup->option_group));

          /* remove groupname */
          for (j = i; j < *argc; ++j)
            (*argv)[j] = (*argv)[j + 1];
          (*argc)--;

          break;
        }
    }

  rv = g_option_context_parse (context->ctx, argc, argv, error);
  if (rv)
    {
      context->rest = g_new0 (gchar *, (*argc) + 1 - 1);
      for (i = 1; i < *argc; ++i)
        context->rest[i - 1] = g_strdup ((*argv)[i]);

      context->group = ogroup;
    }

  if (!ogroup)
    group_help (context);

  return rv;
}


/**
 * gx_sub_command_option_context_get_group:
 * @context: a #GXSubCommandOptionContext instance
 *
 *  After a succesful gx_sub_command_context_parse(), return the #GOptionGroup
 *  for the sub-command, or %NULL if there is none.
 *
 *  Return value:(transfer none): the #GOptionGroup or %NULL.
 */
GOptionGroup*
gx_sub_command_option_context_get_group (GXSubCommandOptionContext *context)
{
  g_return_val_if_fail (context, NULL);

  return context->group ? context->group->option_group : NULL;
}


/**
 * gx_sub_command_option_context_execute:
 * @context: a #GXSubCommandOptionContext instance
 * @argc: (inout) (allow-none): a pointer to the number of command-line
 * arguments
 * @argv: (inout) (allow-none) (array length=argc): a pointer to the array of
 * command-line arguments
 * @error: (allow-none): receives error information
 *
 * After a succesful gx_sub_command_option_context_parse(), if a sub-command
 * function was set with gx_sub_command_option_context_add_group(), that
 * function is invoked for the given group.
 *
 * Return value: If a sub-command was recognized and it defined a
 * #GSubCommandFunc, returns the result of the invocation. Otherwise, returns
 * %TRUE.
 */
gboolean
gx_sub_command_option_context_execute (GXSubCommandOptionContext *context, GError **error)
{
  g_return_val_if_fail (context, FALSE);

  if (context->group && context->group->func)
    {
      return context->group->func ((const char**)context->rest,
                                   context->group->user_data, error);
    }

  return TRUE;
}

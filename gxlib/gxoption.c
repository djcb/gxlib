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

#include <glib/gi18n.h>
#include "gxoption.h"

/**
 * SECTION:gxoption
 * @title: Command-line parsing
 * @short_description: parsing command-line parameters
 *
 * A wrapper for #GOptionContext to support "sub-commands", that is,
 * command-line tools that offer a number of commands, each with their specific
 * options -- examples include "git", "openssl", "mu".
 */

struct _GXSubCommandOptionContext
{
  GOptionContext *ctx;
  GHashTable     *groups;
};

struct _OGroup
{
  GOptionGroup      *option_group;
  GXSubCommandFunc  func;
  gpointer           user_data;
};

typedef struct _OGroup OGroup;

static void
ogroup_free (OGroup *ogroup)
{
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
  mctx->ctx = context;
  
  /* g_option_group_unref only arrived in GLib 2.44 */
  mctx->groups = g_hash_table_new_full (g_str_hash, g_str_equal,
                                        g_free,
                                        (GDestroyNotify)ogroup_free);  
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
  g_hash_table_unref (context->groups);
  g_free (context);
}

/**
 * gx_sub_command_option_context_add_group:
 * @context: a #GXSubCommandOptionContext
 * @sub_command: the name of the sub-command. Should be the same as the name of
 * @option_group.
 * @option_group: the #GOptionGroup for this subcommand
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
                                         GOptionGroup *option_group,
                                         GXSubCommandFunc func,
                                         gpointer user_data)
{
  OGroup *ogroup;
  
  g_return_if_fail (context);
  g_return_if_fail (group_name);
  g_return_if_fail (option_group);

  ogroup               = g_new0 (OGroup, 1);
  ogroup->option_group = option_group;
  ogroup->func         = func;
  ogroup->user_data    = user_data;
  
  g_hash_table_insert (context->groups, g_strdup (group_name), ogroup);
}


/**
 * gx_sub_command_option_context_parse:
 * @context: a #GXSubCommandOptionContext instance
 * @argc: (inout) (allow-none): a pointer to the number of command-line arguments
 * @argv: (inout) (allow-none) (array length=argc): a pointer to the array of
 * command-line arguments
 * @err: (allow-none): receives error information
 *
 * Parses the command-line options, using the main command-line options and the
 * options for appropriate sub-command (if any).
 *
 * If a sub-command function was set with
 * gx_sub_command_option_context_add_group(), that function is invoked for the
 * given group.
 * 
 * See @g_option_context_parse for some more details.
 *
 * Return value: If a sub-command was recognized and it defined a
 * #GSubCommandFunc, returns the result of the invocation. Otherwise, returns
 * %TRUE if parsing worked, %FALSE otherwise.
 * 
 */
gboolean
gx_sub_command_option_context_parse (GXSubCommandOptionContext *context,
                                     gint *argc, gchar ***argv,
                                     GError **error)
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
          
          ogroup = g_hash_table_lookup (context->groups, (*argv)[i]);
          if (!ogroup)
            {
              g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                           _("Unknown group %s"), (*argv)[0]);
              return FALSE;
            }

          g_option_context_add_group (context->ctx,
                                      g_option_group_ref (ogroup->option_group));
          
          /* remove groupname */
          for (j = i; j < *argc - 1; ++j)
            (*argv)[j] = (*argv)[i + 1];
          (*argc)--;
        }
    }
  
  rv = g_option_context_parse (context->ctx, argc, argv, error);
  if (rv && ogroup && ogroup->func)
    {
      return ogroup->func (ogroup->user_data, error);
    }
  
  return rv;
}


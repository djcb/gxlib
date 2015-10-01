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


#ifndef __GXOPTION_H__
#define __GXOPTION_H__

#include <gxlib/gxlib.h>

G_BEGIN_DECLS

/**
 * GXSubCommandOptionContext:
 *
 * A #GXSubCommandOptionContext defines which subcommands the command-line
 * parser accepts, and options they takes. It is like a #GOptionContext for
 * subcommand parsing.
 *
 * The struct has only private
 * fields and should not be directly accessed.
 */
struct _GXSubCommandOptionContext;
typedef struct _GXSubCommandOptionContext GXSubCommandOptionContext;

GXSubCommandOptionContext*
gx_sub_command_option_context_new (GOptionContext *option_context)
G_GNUC_WARN_UNUSED_RESULT;

void gx_sub_command_option_context_free (GXSubCommandOptionContext *context);

/**
 * GXSubCommandFunc:
 * @user_data: user-data passed to funciton
 * @err: (allow-none): receives error information
 *
 * Prototype for a callback function for a #GXSubCommandOptionContext group
 *
 * Returns: %TRUE if the function succeeded, %FALSE otherwise.
 */
typedef gboolean (*GXSubCommandFunc) (gpointer user_data, GError **err);


void gx_sub_command_option_context_add_group (GXSubCommandOptionContext *context,
                                              const char *sub_command,
                                              GOptionGroup *option_group,
                                              GXSubCommandFunc func,
                                              gpointer user_data);

gboolean gx_sub_command_option_context_parse (GXSubCommandOptionContext *context,
                                              gint *argc, gchar ***argv,
                                              GError **error);
G_END_DECLS

#endif /* __GXOPTION_H__ */

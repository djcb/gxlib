/* 
** GXLIB - Library of extensions for GLIB
** Copyright (C) 2015 Dirk-Jan C. Binnema
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

#ifndef __GX_LIST_H__
#define __GX_LIST_H__
 
#include <gxlib/gxlib.h>

G_BEGIN_DECLS

GList *gx_list_filter (GList *list, GXPred pred_func, gpointer user_data)
  G_GNUC_WARN_UNUSED_RESULT;

gboolean gx_list_every (GList *list, GXPred pred_func, gpointer user_data);
gboolean gx_list_any (GList *list, GXPred pred_func, gpointer user_data);

GList *gx_list_filter_in_place (GList *list, GXPred pred_func,
                                gpointer user_data, GDestroyNotify free_func)
  G_GNUC_WARN_UNUSED_RESULT;

GList *gx_list_take (GList *list, gsize n)
  G_GNUC_WARN_UNUSED_RESULT;

GList *gx_list_take_in_place (GList *list, gsize n, GDestroyNotify free_func);

GList *gx_list_skip (GList *list, gsize n)
  G_GNUC_WARN_UNUSED_RESULT;

GList *gx_list_skip_in_place (GList *list, gsize n, GDestroyNotify free_func)
  G_GNUC_WARN_UNUSED_RESULT;

GList *gx_list_map (GList *list, GXBinaryFunc map_func, gpointer user_data)
  G_GNUC_WARN_UNUSED_RESULT;

GList *gx_list_map_in_place (GList *list, GXBinaryFunc map_func,
                             gpointer user_data, GDestroyNotify free_func);

gpointer gx_list_fold (GList *list, GXTernaryFunc fold_func, gpointer init,
                       gpointer user_data, GDestroyNotify free_func)
  G_GNUC_WARN_UNUSED_RESULT;

GList *gx_list_iota (gsize count, gint start, gsize step)
   G_GNUC_WARN_UNUSED_RESULT;

gint gx_list_sum (GList *list);
gint gx_list_product (GList *list);


G_END_DECLS

#endif /* __GX_LIST_H__ */

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

#ifndef __GXDIRWATCHER_H__
#define __GXDIRWATCHER_H__

#include <glib-object.h>
#include <gio/gio.h>

/**
 * SECTION:gxdirwatcher
 * @title: Directory scanner and watcher
 * @short_description: scan a number of directories and watch them for changes
 * 
 * An asychronous file system scanner/watcher.
 *
 * NOTE: #GXDirWatcher is experimental and its API and semantics are unstable.
 */

G_BEGIN_DECLS
/*
 * convenience macros
 */
#define GX_TYPE_DIR_WATCHER             (gx_dir_watcher_get_type ())
#define GX_DIR_WATCHER(self)            (G_TYPE_CHECK_INSTANCE_CAST ((self), \
                                     GX_TYPE_DIR_WATCHER,GXDirWatcher))
#define GX_DIR_WATCHER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),   \
				     GX_TYPE_DIR_WATCHER,GXDirWatcherClass))
#define GX_IS_DIR_WATCHER(self)         (G_TYPE_CHECK_INSTANCE_TYPE ((self), \
                                     GX_TYPE_DIR_WATCHER))
#define GX_IS_DIR_WATCHER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),   \
                                     GX_TYPE_DIR_WATCHER))
#define GX_DIR_WATCHER_GET_CLASS(self)  (G_TYPE_INSTANCE_GET_CLASS ((self),  \
                                     GX_TYPE_DIR_WATCHER,GXDirWatcherClass))

typedef struct _GXDirWatcher GXDirWatcher;
typedef struct _GXDirWatcherClass GXDirWatcherClass;
typedef struct _GXDirWatcherPrivate GXDirWatcherPrivate;

/**
 * GXDirWatcher:
 *
 * The #GXDirWatcher structure contains only private data and should be
 * accessed using its public API
 *
 */
struct _GXDirWatcher {
	/*< private > */
	GObject			 parent;
	GXDirWatcherPrivate	*priv;
};

/**
 * GXDirWatcherClass:
 * 
 * Class structure for #GXDirWatcher
 */
struct _GXDirWatcherClass {
	/*< private > */
	GObjectClass parent_class;
	void  (*update) (GXDirWatcher		*watcher,
			 GFileMonitorEvent	 event_type,
			 GFileType		 file_type,
			 const gchar		*path);
};

/**
 * gx_dir_watcher_get_type:
 * 
 * Get the #GType for #GXDirWatcher
 * 
 * Returns: the #GType
 */
GType gx_dir_watcher_get_type (void) G_GNUC_CONST;


/**
 * GXDirWatcherFlags:
 * @GX_DIR_WATCHER_FLAG_NONE: no special flags
 * @GX_DIR_WATCHER_FLAG_MONITOR: install a change-monitor for each
 * 'interesting' (as per matches/ignores in gx_dir_watcher_new()) directory.
 *
 * Flags to influence #GXDirWatcher behavior.
 *
 */
typedef enum {
	GX_DIR_WATCHER_FLAG_NONE		= 0,
	GX_DIR_WATCHER_FLAG_MONITOR		= 1 << 0
} GXDirWatcherFlags;


/**
 * gx_dir_watcher_new:
 * @dirs: a %NULL-terminated array of directories to watch
 * @matches: (allow-none): %NULL terminated array of regexps matching
 * interesting files; see @gx_dir_set_matches for details.
 * @ignores: (allow-none): %NULL terminated array of regexps matching
 * directories to recursively ignore for scanning; see @gx_dir_set_ignores for
 * details.
 * @flags: #GXDirWatcherFlags flags that influence the behavior
 * @err: (allow-none): receives error information
 *
 * Create a new #GXDirWatcher instance.
 *
 * Use gx_dir_watcher_scan() to scan directories, and set up file-change
 * monitors which then give change notifications through the
 * #GXDirWatcher::update signal.
 *
 * Returns: (transfer full): a new #GXDirWatcher instance or %NULL
 * in case of error. Free with g_object_unref ().
 */
GXDirWatcher *gx_dir_watcher_new (const char *const *dirs,
				  const char *const *matches,
				  const char *const *ignores,
				  GXDirWatcherFlags flags,
				  GError **err) G_GNUC_WARN_UNUSED_RESULT;
/**
 * gx_dir_watcher_scan:
 * @self: a #GXDirWatcher instance
 * @cancellable: (allow-none): a #GCancellable with which one can
 * cancel the scan
 * @callback: callback function to call when the scan has terminated
 * @user_data: data passed to @callback
 *
 * Asynchronously run a file-system scan over the directories set with
 * gx_dir_watcher_new(). For each file and directroy found, an
 * #GXDirWatcher::update signal is emitted, with @G_FILE_MONITOR_EVENT_CREATED
 * (since it is create from the scanner's point of view).
 *
 * If @self was created with @GX_DIR_WATCHER_FLAG_MONITOR (in
 * gx_dir_watcher_new()), later changes would e.g. trigger
 * @GX_FILE_MONITOR_EVENT_CHANGED.
 *
 * Use gx_dir_watcher_scan_finish() to get the result.
 *
 * Note that signals may be emitted from a different thread during the scan.
 */
void gx_dir_watcher_scan (GXDirWatcher *self,
			  GCancellable *cancellable,
			  GAsyncReadyCallback callback,
			  gpointer user_data);

/**
 * gx_dir_watcher_scan_finish:
 * @self: a #GXDirWatcher instance
 * @res: a #GAsyncResult as received in gx_dir_watcher_scan()'s callback
 * @err: (allow-none): receives error information
 *
 * Get the results from the asynchronous gx_dir_watcher_scan().
 *
 * Returns: %TRUE if the scan finished successfully, %FALSE otherwise.
 */
gboolean gx_dir_watcher_scan_finish (GXDirWatcher *self,
				     GAsyncResult *res, GError **err);
G_END_DECLS
#endif				/* __GXDIRWATCHER_H__ */

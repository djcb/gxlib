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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif				/*HAVE_CONFIG_H */

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* hopefully, this should get us a sane PATH_MAX */
#include <limits.h>
/* not all systems provide PATH_MAX in limits.h */
#ifndef PATH_MAX
#include <sys/param.h>
#ifndef PATH_MAX
#define PATH_MAX MAXPATHLEN
#endif				/*!PATH_MAX */
#endif				/*PATH_MAX */

#include "gxdirwatcher.h"

struct _GXDirWatcherPrivate {
	GHashTable	 *monitors;
	char		**dirs;
	GCancellable	 *cancellable;
	GList		 *matches;	/* list of regexps for files to watch */
	char		**matchesv;
	GList		 *ignores;	/* list of regexps for dirs to ignore */
	char		**ignoresv;

	GXDirWatcherFlags flags;

	GMutex lock;
};

G_DEFINE_TYPE_WITH_PRIVATE(GXDirWatcher, gx_dir_watcher, G_TYPE_OBJECT);

static void gx_dir_watcher_finalize(GObject * obj);

static gboolean verify_readable_dir(const char *path, GError **err);
static gboolean set_matches (GXDirWatcher *self, const char *const *rxs,
			     GError **err);
static gboolean set_ignores (GXDirWatcher *self, const char *const *rxs,
			     GError **err);

typedef enum {
	SIG0 = 0,
	SIG_UPDATE,
	/*  ...other sigs... */
	SIG_NUM
} GXDirWatcherSigs;

static guint SIGS[SIG_NUM] = { 0 };

typedef enum {
	PROP0 = 0,
	PROP_DIRS,
	PROP_MATCHES,
	PROP_IGNORES,
	PROP_SCANNING,
	PROP_FLAGS,
	/*  ...other props... */
	PROP_NUM
} GXDirWatcherProps;

static GParamSpec *PROPS[PROP_NUM] = { NULL, };

static void
set_property (GObject *obj, guint prop_id, const GValue *val,
	      GParamSpec *pspec)
{
	GXDirWatcher *self;

	self = GX_DIR_WATCHER(obj);

	switch (prop_id) {
	case PROP_DIRS:
		g_strfreev(self->priv->dirs);
		self->priv->dirs = g_value_dup_boxed(val);
		break;
	case PROP_MATCHES: {
		GError *err;
		const char* const* rxs;
		err = NULL;
		rxs = (const char* const*)g_value_get_boxed(val);
		if (!set_matches (self, rxs, &err)) {
			g_critical ("failed to set matches: %s",
				    err ? err->message :
				    "something went wrong");
			g_clear_error (&err);
			break;
		}
		g_strfreev(self->priv->matchesv);
		self->priv->matchesv = g_value_dup_boxed(val);
	} break;
	case PROP_IGNORES: {
		GError *err;
		const char* const* rxs;
		err = NULL;
		rxs = (const char* const*)g_value_get_boxed(val);
		if (!set_ignores (self, rxs, &err)) {
			g_critical ("failed to set ignores: %s",
				    err ? err->message :
				    "something went wrong");
			g_clear_error (&err);
			break;
		}
		g_strfreev(self->priv->ignoresv);
		self->priv->ignoresv = g_value_dup_boxed(val);
	} break;
	case PROP_FLAGS:
		self->priv->flags = g_value_get_uint (val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void
get_property (GObject *obj, guint prop_id, GValue *val, GParamSpec *pspec)
{
	GXDirWatcher *self;

	self = GX_DIR_WATCHER(obj);

	switch (prop_id) {
	case PROP_DIRS:
		g_value_set_boxed (val, self->priv->dirs);
		break;
	case PROP_MATCHES:
		g_value_set_boxed (val, self->priv->matchesv);
		break;
	case PROP_IGNORES:
		g_value_set_boxed (val, self->priv->ignoresv);
		break;
	case PROP_SCANNING:
		g_value_set_boolean (val, self->priv->cancellable ?
				     TRUE : FALSE);
		break;
	case PROP_FLAGS:
		g_value_set_uint (val, self->priv->flags);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
}

static void
gx_dir_watcher_class_init (GXDirWatcherClass *klass)
{
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gx_dir_watcher_finalize;
	gobject_class->set_property = set_property;
	gobject_class->get_property = get_property;

	/**
	 * GXDirWatcher::update:
	 * @watcher: the #GXDirWatcher emitting the signal
	 * @event_type: the event that happened
	 * @file_type: the type of file
	 * @path: the full path to the file
	 *
	 * Signal emitted when something is found or changed in the filesystem.
	 *
	 * During scanning, there is a @G_FILE_MONITOR_EVENT_CREATED for each
	 * file and directory (since it is new from the scanner's perspective).
	 *
	 * Note that during scanning, the signal may be emitted from a different
	 * thread. For that reason, signal-handlers must not be connected or
	 * disconnected during scanning.
	 */
	SIGS[SIG_UPDATE] =
	    g_signal_new ("update",
			  G_TYPE_FROM_CLASS(gobject_class),
			  G_SIGNAL_RUN_LAST,
			  G_STRUCT_OFFSET(GXDirWatcherClass, update),
			  NULL, NULL,
			  NULL,
			  G_TYPE_NONE, 3,
			  G_TYPE_FILE_MONITOR_EVENT,
			  G_TYPE_FILE_TYPE,
			  G_TYPE_STRING);

	/**
	 * GXDirWatcher:dirs
	 *
	 * The dirs property for this object.
	 */
	PROPS[PROP_DIRS] =
	    g_param_spec_boxed ("dirs", "Directories",
			       "Root directories to watch",
			       G_TYPE_STRV,
			       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
			       G_PARAM_CONSTRUCT_ONLY);
	/**
	 * GXDirWatcher:monitor
	 *
	 * Whether this object is currently scanning
	 */
	PROPS[PROP_SCANNING] =
		g_param_spec_boolean ("scanning", "Scanning",
				     "Is there an ongoing scanning operation?",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

	/**
	 * GXDirWatcher:matches
	 *
	 * Array of regular expressions (compatible with #GRegex) for the
	 * files/directories we're interested in.
	 *
	 * This is useful when we're only interested in certain file types.
	 */
	PROPS[PROP_MATCHES] =
		g_param_spec_boxed ("matches", "Match",
				   "Regexps for interesting files",
				    G_TYPE_STRV,
				    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	/**
	 * GXDirWatcher:ignores
	 *
	 * Array of regular expressions (compatible with #GRegex) for
	 * file/directories we're not interested in. When this matches a
	 * directory name, the directory is recursively ignored.
	 */
	PROPS[PROP_IGNORES] =
	    g_param_spec_boxed ("ignores", "Ignore",
				"Regexps for files/Directories to ignore",
				G_TYPE_STRV,
				G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	/**
	 * GXDirWatcher:flags
	 *
	 * Bitmask of #GXDirWatcherFlags that influence behavior.
	 */
	PROPS[PROP_FLAGS] =
		g_param_spec_uint (
			"flags", "Flags",
			"GXDirWatcherFlags that influence behavior",
			GX_DIR_WATCHER_FLAG_NONE, G_MAXUINT,
			GX_DIR_WATCHER_FLAG_NONE,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
			G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(gobject_class, PROP_NUM, PROPS);
}

static void
gx_dir_watcher_init(GXDirWatcher *self)
{
	self->priv = gx_dir_watcher_get_instance_private(self);

	g_mutex_init (&self->priv->lock);

	self->priv->monitors =
	    g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
				  (GDestroyNotify) g_object_unref);
}

static void
gx_dir_watcher_finalize (GObject * obj)
{
	GXDirWatcher *self;

	self = GX_DIR_WATCHER (obj);

	if (self->priv->cancellable) {
		g_cancellable_cancel (self->priv->cancellable);
		g_clear_object (&self->priv->cancellable);
	}

	g_strfreev (self->priv->dirs);

	if (self->priv->monitors)
		g_hash_table_unref (self->priv->monitors);

	g_list_free_full (self->priv->matches, (GDestroyNotify)g_regex_unref);
	g_list_free_full (self->priv->ignores, (GDestroyNotify)g_regex_unref);

	g_strfreev (self->priv->matchesv);
	g_strfreev (self->priv->ignoresv);

	g_mutex_clear (&self->priv->lock);

	G_OBJECT_CLASS (gx_dir_watcher_parent_class)->finalize (obj);
}

static gboolean
matches_any (GXDirWatcher *self, GList *rxs, const char *path)
{
	gboolean rv;

	g_mutex_lock (&self->priv->lock);

	for (rv = FALSE; rxs; rxs = g_list_next (rxs))
		if ((rv = g_regex_match ((GRegex *) rxs->data, path, 0, NULL)))
			break;

	g_mutex_unlock (&self->priv->lock);

	return rv;
}

static gboolean
ignored_path (GXDirWatcher *self, const char *path)
{
	if (!self->priv->ignores)
		return FALSE;	/* ignore none */

	return matches_any (self, self->priv->ignores, path);
}

static gboolean
matched_path (GXDirWatcher *self, const char *path)
{
	if (!self->priv->matches)
		return TRUE;	/* match all */

	return matches_any (self, self->priv->matches, path);
}

static gboolean install_monitor_maybe (GXDirWatcher *self, const char *path,
				       GError **err);

static void
on_dir_changed (GFileMonitor *mon, GFile *file, GFile *other_file,
	       GFileMonitorEvent ev_type, GXDirWatcher *self)
{
	GFileType ftype;
	GError *err;
	char *path;

	if (ev_type != G_FILE_MONITOR_EVENT_CREATED &&
	    ev_type != G_FILE_MONITOR_EVENT_DELETED &&
	    ev_type != G_FILE_MONITOR_EVENT_CHANGED)
		return;		/* not interesting */

	err = NULL;
	path = g_file_get_path (file);

	/* filter-out uninteresting changes */
	if (!matched_path (self, path) || ignored_path (self, path))
		goto leave;

	/* in the case G_FILE_MONITOR_EVENT_DELETED, ftype will always be
	 * G_FILE_TYPE_UNKNOWN... */
	ftype = g_file_query_file_type (file, G_FILE_QUERY_INFO_NONE, NULL);

	switch (ev_type) {

	case G_FILE_MONITOR_EVENT_CREATED:
		if (ftype != G_FILE_TYPE_DIRECTORY)
			break;
		if (!install_monitor_maybe (self, path, &err))
			g_warning ("failed to install "
				   "monitor on %s: %s", path,
				   err ? err->message :
				   "something went wrong");
		break;
	case G_FILE_MONITOR_EVENT_DELETED:
		g_hash_table_remove (self->priv->monitors, path);

	case G_FILE_MONITOR_EVENT_CHANGED:
	case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
	case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
	case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
	case G_FILE_MONITOR_EVENT_UNMOUNTED:
	case G_FILE_MONITOR_EVENT_MOVED:
	default:
		break;		/* ignore */
	}

	g_signal_emit (self, SIGS[SIG_UPDATE], 0, ev_type, ftype, path);

 leave:
	g_free (path);
	g_clear_error (&err);
}

static gboolean
install_monitor_maybe (GXDirWatcher *self, const char *path, GError **err)
{
	GFile *file;
	GFileInfo *info;
	GFileMonitor *mon;
	gboolean rv;

	/* only install monitor if we were initiated with the right flags */
	if (!(self->priv->flags & GX_DIR_WATCHER_FLAG_MONITOR))
		return TRUE;

	rv = FALSE;
	info = NULL;

	if (g_hash_table_contains (self->priv->monitors, path))
		return TRUE;	/* already */

	file = g_file_new_for_path (path);
	if (!g_file_query_exists (file, NULL)) {
		g_set_error (err, G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "not found");
		goto leave;
	}

	if (!verify_readable_dir (path, err))
		goto leave;

	mon = g_file_monitor_directory (file, G_FILE_MONITOR_NONE, NULL, err);
	if (!mon)
		goto leave;

	g_signal_connect (mon, "changed", G_CALLBACK (on_dir_changed), self);
	g_hash_table_insert (self->priv->monitors, g_strdup (path), mon);

	rv = TRUE;

 leave:
	g_clear_object (&file);
	g_clear_object (&info);

	return rv;
}

GXDirWatcher*
gx_dir_watcher_new (const char *const *dirs,
		    const char *const *matches,
		    const char *const *ignores,
		    GXDirWatcherFlags flags,
		    GError **err)
{
	GXDirWatcher *self;

	g_return_val_if_fail (dirs, NULL);

	self = GX_DIR_WATCHER (g_object_new (GX_TYPE_DIR_WATCHER,
					     "dirs", dirs,
					     "flags", flags,
					     NULL));

	if (!set_matches (self, matches, err) ||
	    !set_ignores (self, ignores, err)) {
		g_object_unref (G_OBJECT (self));
		return NULL;
	}

	return self;
}

static GList*
get_rx_list (const char *const *rxs, GError **err)
{
	GList *lst;

	for (lst = NULL; rxs && *rxs; ++rxs) {
		GRegex *rx;
		rx = g_regex_new (*rxs, G_REGEX_OPTIMIZE, 0, err);
		if (!rx) {
			g_list_free_full (lst, (GDestroyNotify) g_regex_unref);
			return NULL;
		}
		lst = g_list_prepend (lst, rx);
	}

	return lst;
}

static char**
get_rx_array (GList *lst)
{
	guint u;
	char **strv;

	strv = g_new0(char *, g_list_length (lst) + 1);

	for (u = 0; lst; lst = g_list_next (lst), ++u)
		strv[u] = g_strdup (g_regex_get_pattern ((GRegex *) lst->data));

	return strv;
}

static gboolean
set_matches (GXDirWatcher *self, const char *const *rxs, GError **err)
{
	GList *lst;

	lst = NULL;
	if (rxs && *rxs && !(lst = get_rx_list (rxs, err)))
		return FALSE;

	/* LOCK */ g_mutex_lock (&self->priv->lock);

	g_list_free_full (self->priv->matches, (GDestroyNotify)g_regex_unref);
	self->priv->matches = g_list_reverse (lst);
	g_strfreev (self->priv->matchesv);
	if (rxs && *rxs)
		self->priv->matchesv = get_rx_array (self->priv->matches);
	else
		self->priv->matchesv = NULL;

	/* UNLOCK*/ g_mutex_unlock (&self->priv->lock);

	return TRUE;
}


static gboolean
set_ignores (GXDirWatcher *self, const char *const *rxs, GError **err)
{
	GList *lst;

	lst = NULL;
	if (rxs && *rxs && !(lst = get_rx_list (rxs, err)))
		return FALSE;

	/* LOCK */ g_mutex_lock (&self->priv->lock);

	g_list_free_full (self->priv->ignores, (GDestroyNotify) g_regex_unref);
	self->priv->ignores = g_list_reverse (lst);
	g_strfreev (self->priv->ignoresv);

	if (rxs && *rxs)
		self->priv->ignoresv = get_rx_array (self->priv->ignores);
	else
		self->priv->ignoresv = NULL;

	/* UNLOCK */ g_mutex_unlock (&self->priv->lock);

	return TRUE;
}


static gboolean
verify_readable_dir (const char *path, GError **err)
{
	int		mode;
	struct stat	statbuf;

	if (!path)
		return FALSE;

	if (G_UNLIKELY(strlen (path) > PATH_MAX)) {
		g_set_error (err, G_IO_ERROR,
			    G_IO_ERROR_FILENAME_TOO_LONG, "path too long");
		return FALSE;
	}

	mode = F_OK | R_OK;
	if (access (path, mode) != 0) {
		g_set_error (err, G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED,
			    "not readable: %s", strerror (errno));
		return FALSE;
	}

	if (G_UNLIKELY(stat (path, &statbuf) != 0)) {
		g_set_error (err, G_IO_ERROR, G_IO_ERROR_FAILED,
			    "not stat'able: %s", strerror (errno));
		return FALSE;
	}

	if (!S_ISDIR (statbuf.st_mode)) {
		g_set_error (err, G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY,
			    "not a directory");
		return FALSE;
	}

	return TRUE;
}

static const size_t DIRENT_ALLOC_SIZE =
offsetof (struct dirent, d_name) + PATH_MAX;

static struct dirent *
dirent_new (void)
{
	return (struct dirent *)g_slice_alloc (DIRENT_ALLOC_SIZE);
}

static void
dirent_destroy (struct dirent *entry)
{
	g_slice_free1(DIRENT_ALLOC_SIZE, entry);
}

#ifdef HAVE_STRUCT_DIRENT_D_INO
static int
dirent_cmp (struct dirent *d1, struct dirent *d2)
{
	/* we do it his way instead of a simple d1->d_ino - d2->d_ino
	 * because this way, we don't need 64-bit numbers for the
	 * actual sorting */
	if (d1->d_ino < d2->d_ino)
		return -1;
	else if (d1->d_ino > d2->d_ino)
		return 1;
	else
		return 0;
}
#endif	/*HAVE_STRUCT_DIRENT_D_INO */

/* On Linux (and some BSDs), we have entry->d_type, but some file
 * systems (XFS, ReiserFS) do not support it, and set it DT_UNKNOWN.
 * On other OSs, notably Solaris, entry->d_type is not present at all.
 * For these cases, we use lstat (in get_dtype) as a slower fallback,
 * and return it in the d_type parameter
 */
#ifdef HAVE_STRUCT_DIRENT_D_TYPE
#define GET_DTYPE (DE,FP)						   \
	((DE)->d_type == DT_UNKNOWN ? mu_util_get_dtype_with_lstat ((FP)) : \
	 (DE)->d_type)
#else
#define GET_DTYPE (DE,FP)			                           \
	mu_util_get_dtype_with_lstat ((FP))
#endif				/*HAVE_STRUCT_DIRENT_D_TYPE */

static guchar
get_d_type (struct dirent *dentry, const char *path, GError **err)
{
	struct stat statbuf;

#ifdef HAVE_STRUCT_DIRENT_D_TYPE

	/* On Linux (and some BSDs), we have entry->d_type, but some
	 * file systems (XFS, ReiserFS) do not support it, and set it
	 * to DT_UNKNOWN.  On other OSs, notably Solaris,
	 * entry->d_type is not present at all.  For these cases, we
	 * use lstat (in get_dtype) as a slower fallback, and return
	 * it in the d_type parameter
	 */
	if (dentry->d_type != DT_UNKNOWN)
		return dentry->d_type;
#endif				/* HAVE_STRUCT_DIRENT_D_TYPE */

	if (lstat (path, &statbuf) != 0) {
		g_set_error (err, G_IO_ERROR, G_IO_ERROR_FAILED,
			    "stat failed on %s: %s", path, strerror (errno));
		return DT_UNKNOWN;
	}

	/* we only care about dirs, regular files and links */
	if (S_ISREG (statbuf.st_mode))
		return DT_REG;
	else if (S_ISDIR (statbuf.st_mode))
		return DT_DIR;
	else if (S_ISLNK (statbuf.st_mode))
		return DT_LNK;

	return DT_UNKNOWN;
}

static gboolean process_dir (GXDirWatcher *self, const char *path,
			     GTask * task);

static gboolean
process_dentry (GXDirWatcher *self, const char *path,
		struct dirent *entry, GTask *task)
{
	char		 fullpath[PATH_MAX + 1];
	unsigned char	 d_type;
	size_t		 plen, dlen;
	GError		*err;

	plen = strlen (path);
	dlen = strlen (entry->d_name);

	if (G_UNLIKELY (g_task_return_error_if_cancelled (task)))
		return FALSE;

	if (G_UNLIKELY (plen + dlen + 1 > PATH_MAX)) {
		g_task_return_new_error (task, G_IO_ERROR,
					 G_IO_ERROR_FILENAME_TOO_LONG,
					 "path too long");
		goto finished_task;
	}

	if (dlen <= 2 &&	/* ignore '.' and '..' */
	    (entry->d_name[1] == '.' || entry->d_name[1] == '\0') &&
	    entry->d_name[0] == '.')
		return TRUE;

	/* we try hard to do all the string copying on the stack */
	strcpy (fullpath, path);
	fullpath[plen] = G_DIR_SEPARATOR;
	strcpy (fullpath + plen + 1, entry->d_name);

	err    = NULL;
	d_type = get_d_type (entry, fullpath, &err);

	switch (d_type) {
	case DT_REG:
		if (matched_path (self, fullpath)) /* only interesting files */
			g_signal_emit (self, SIGS[SIG_UPDATE], 0,
				       G_FILE_MONITOR_EVENT_CREATED,
				       G_FILE_TYPE_REGULAR, fullpath);
		break;
	case DT_DIR:
		if (!process_dir (self, fullpath, task))
			return FALSE;
		break;
	default:
		if (err) {
			g_task_return_error (task, err);
			goto finished_task;
		}
		break;
	}

	return TRUE;

 finished_task:
	return FALSE;
}

static gboolean
process_dentries (GXDirWatcher *self, DIR *dir, const char *path, GTask *task)
{
	gboolean	 rv;
	GList		*lst, *cur;

	lst = NULL;

	for (;;) {
		int res;
		struct dirent *entry, *res_entry;

		if (G_UNLIKELY(g_task_return_error_if_cancelled (task)))
			return FALSE;

		entry = dirent_new ();
		res = readdir_r (dir, entry, &res_entry);

		/* error? */
		if (G_UNLIKELY (res != 0)) {
			dirent_destroy (entry);
			g_task_return_new_error (task, G_IO_ERROR,
						G_IO_ERROR_FAILED,
						"error scanning dir: %s",
						strerror (res));
			return FALSE;
		}

		/* last direntry reached? */
		if (!res_entry) {
			dirent_destroy (entry);
			break;
		}

		/* add to our list of entries; sort them later */
		lst = g_list_prepend (lst, entry);
	}

#if HAVE_STRUCT_DIRENT_D_INO
	/* sort by inode if possible; this makes things much faster on
	 * extfs2,3,4 */
	lst = g_list_sort (lst, (GCompareFunc)dirent_cmp);
#endif /*HAVE_STRUCT_DIRENT_D_INO */

	for (rv = TRUE, cur = lst; cur; cur = g_list_next (cur)) {
		rv = process_dentry (self, path, (struct dirent *)cur->data,
				    task);
		if (!rv)
			break;
	}

	g_list_free_full (lst, (GDestroyNotify) dirent_destroy);

	return rv;
}

static gboolean
process_dir (GXDirWatcher *self, const char *path, GTask *task)
{
	DIR *dir;
	gboolean rv;
	GError *err;

	if (G_UNLIKELY(g_task_return_error_if_cancelled (task)))
		return FALSE;

	if (ignored_path (self, path))
		return TRUE;

	if (G_UNLIKELY(!(dir = opendir (path)))) {
		g_task_return_new_error (task, G_IO_ERROR, G_IO_ERROR_FAILED,
					"cannot access %s: %s",
					 path, strerror (errno));
		return FALSE;
	}

	g_signal_emit (self, SIGS[SIG_UPDATE], 0,
		       G_FILE_MONITOR_EVENT_CREATED,
		       G_FILE_TYPE_DIRECTORY, path);

	err = NULL;
	if (G_UNLIKELY(!install_monitor_maybe (self, path, &err))) {
		g_task_return_error (task, err);
		return FALSE;
	}

	rv = process_dentries (self, dir, path, task);
	closedir (dir);

	return rv;
}

static void
scan_thread (GTask *task, GXDirWatcher *self, gpointer task_data,
	     GCancellable *cancellable)
{
	char **dir;
	gboolean rv;

	for (rv = TRUE, dir = self->priv->dirs; dir && *dir; ++dir)
		if (!(rv = process_dir (self, *dir, task)))
			break;

	g_task_return_boolean (task, rv);
	g_clear_object (&self->priv->cancellable);

	g_object_unref (task);
}

void
gx_dir_watcher_scan (GXDirWatcher *self, GCancellable *cancellable,
		    GAsyncReadyCallback callback, gpointer user_data)
{
	GTask *task;

	g_return_if_fail (GX_IS_DIR_WATCHER (self));
	g_return_if_fail (callback);

	if (self->priv->cancellable) {
		g_task_report_new_error (self, callback, user_data, NULL,
					 G_IO_ERROR, G_IO_ERROR_BUSY,
					 "Already scanning");
		return;
	}

	if (cancellable)
		self->priv->cancellable	= g_object_ref (cancellable);
	else
		self->priv->cancellable = g_cancellable_new ();

	g_object_notify_by_pspec (G_OBJECT (self), PROPS[PROP_SCANNING]);

	task = g_task_new (self, self->priv->cancellable, callback, user_data);
	g_task_run_in_thread (task, (GTaskThreadFunc)scan_thread);
}

gboolean
gx_dir_watcher_scan_finish (GXDirWatcher *self, GAsyncResult *res,
			   GError **err)
{
	g_return_val_if_fail (GX_IS_DIR_WATCHER (self), FALSE);
	g_return_val_if_fail (G_IS_TASK (res), FALSE);

	/* if we're still scanning, nothing changed (would only happen when user
	   calls scan while we were already scanning) */
	if (!self->priv->cancellable)
		g_object_notify_by_pspec (G_OBJECT (self),
					  PROPS[PROP_SCANNING]);

	return g_task_propagate_boolean (G_TASK (res), err);
}

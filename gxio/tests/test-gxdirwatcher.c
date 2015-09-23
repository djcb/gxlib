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

#include <gxio/gxio.h>
#include <string.h>

typedef struct {
	GMainLoop	 *loop;
	guint		  seen;
	char		**files;
	gboolean	  scanned;
	gulong		  sig_id;
} TestCase;

static TestCase*
test_case_new (const char **files)
{
	TestCase *tcase;

	tcase = g_new0(TestCase, 1);
	tcase->files = (GStrv) files;
	tcase->loop = g_main_loop_new (NULL, TRUE);

	return tcase;
}

static void
test_case_destroy (TestCase * tcase)
{
	g_main_loop_unref (tcase->loop);
	g_free (tcase);
}

static void
on_scanning_already (GXDirWatcher *watcher, GAsyncResult *res)
{
	gboolean rv;
	GError *err;

	err = NULL;
	rv = gx_dir_watcher_scan_finish (watcher, res, &err);
	g_assert_error (err, G_IO_ERROR, G_IO_ERROR_BUSY);
	g_assert_false (rv);
	g_clear_error (&err);
}

static void
on_update (GXDirWatcher *watcher, GFileMonitorEvent event,
	   GFileType ftype, const char *path, TestCase *tcase)
{
	gboolean	  match;
	char		**elm;

	g_assert (GX_IS_DIR_WATCHER (watcher));
	g_assert (path);

	if (event != G_FILE_MONITOR_EVENT_CREATED)
		return;
	if (ftype != G_FILE_TYPE_REGULAR)
		return;

	g_debug ("%s", G_STRLOC);
	
	for (match = FALSE, elm = tcase->files; elm && *elm && !match; ++elm) {
		match = g_str_has_suffix (path, *elm);
		g_debug ("%s; %s", path, *elm);
	}

	if (!match)
		g_debug ("no match: %s", path);

	g_assert (match);
	++tcase->seen;
}

static void
on_scanned (GXDirWatcher *watcher, GAsyncResult *res, TestCase *tcase)
{
	gboolean rv;
	GError *err;

	err = NULL;
	rv = gx_dir_watcher_scan_finish (watcher, res, &err);
	g_assert_no_error (err);
	g_assert (rv);

	if (tcase->sig_id != 0) {
		g_signal_handler_disconnect (watcher, tcase->sig_id);
		tcase->sig_id = 0;
	}

	tcase->scanned = TRUE;
	g_assert_cmpuint (g_strv_length ((char **)tcase->files),
			  ==, tcase->seen);
	g_main_loop_quit (tcase->loop);
}


static gboolean
log_handler (const gchar *log_domain, GLogLevelFlags log_level,
	     const gchar *message, const gchar *exp)
{
	g_assert ((log_level & G_LOG_LEVEL_CRITICAL) ||
		  (log_level & G_LOG_LEVEL_WARNING));

	return FALSE;
}


static void
test_props (void)
{
	char		**xdirs;
	GError		 *err;
	GXDirWatcher	 *watcher;
	TestCase	 *tcase;
	gboolean	  scanning;
	const char *ignores[] = { "(", NULL };
	const char *matches[] = { ")", NULL };
	const char *dirs[]  = { TESTTREE1, NULL };
	const char *files[] = {
		"/tree1/file1",
		"/tree1/file2",
		"/tree1/dir1/file4.foo",
		"/tree1/dir1/file5.bar",
		"/tree1/dir2/file6.foo",
		"/tree1/dir2/file6.bar",
		NULL
	};

	err	= NULL;
	tcase	= test_case_new ((const char **)files);
	watcher = gx_dir_watcher_new ((const char *const *)dirs, NULL, NULL,
				      GX_DIR_WATCHER_FLAG_NONE, &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	xdirs = NULL;
	g_object_get (watcher, "dirs", &xdirs, NULL);
	g_assert (xdirs);
	g_assert_cmpstr (xdirs[0],==,dirs[0]);
	g_strfreev (xdirs);
		
	g_signal_connect (watcher, "update", G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			    tcase);
	g_object_get (watcher, "scanning", &scanning, NULL);
	g_assert_true (scanning);
	g_main_loop_run (tcase->loop);

	g_test_log_set_fatal_handler (
		(GTestLogFatalFunc)log_handler, (gpointer)"failed");
	g_object_set (watcher,
		      "matches", matches,
		      "ignores", ignores,
		      NULL);

	g_object_set (watcher,
		      "foo", matches,
		      "bar", ignores,
		      NULL);

	g_object_get (watcher,
		      "foo", &matches,
		      "bar", &matches,
		      NULL);

	g_log_set_default_handler (g_log_default_handler, NULL);
		
	g_object_unref (watcher);
	test_case_destroy (tcase);
}


static void
test_no_match_no_ignore (void)
{
	GError		 *err;
	GXDirWatcher	 *watcher;
	TestCase	 *tcase;
	const char *dirs[]  = { TESTTREE1, NULL };
	const char *files[] = {
		"/tree1/file1",
		"/tree1/file2",
		"/tree1/dir1/file4.foo",
		"/tree1/dir1/file5.bar",
		"/tree1/dir2/file6.foo",
		"/tree1/dir2/file6.bar",
		NULL
	};

	err	= NULL;
	tcase	= test_case_new ((const char **)files);
	watcher = gx_dir_watcher_new ((const char *const *)dirs, NULL, NULL,
				      GX_DIR_WATCHER_FLAG_NONE, &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	g_signal_connect (watcher, "update", G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			    tcase);
	gx_dir_watcher_scan (watcher, NULL,
			    (GAsyncReadyCallback) on_scanning_already, NULL);

	g_main_loop_run (tcase->loop);

	g_object_unref (watcher);
	test_case_destroy (tcase);
}

static void
test_match_only (void)
{
	GError *err;
	GXDirWatcher *watcher;
	TestCase *tcase;
	const char *dirs[] = { TESTTREE1, NULL };
	const char *matches[] = { "\\.foo$", NULL };
	char **ms, **is;
	const char *files[] = {
		"/tree1/dir1/file4.foo",
		"/tree1/dir2/file6.foo",
		NULL
	};

	err = NULL;
	tcase = test_case_new ((const char **)files);
	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				     (const char *const *)matches,
				      NULL, GX_DIR_WATCHER_FLAG_NONE, &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	g_object_get (watcher,
		      "matches", &ms,
		      "ignores", &is,
		      NULL);
	g_assert (ms);
	g_assert_cmpstr (ms[0], ==, matches[0]);
	g_strfreev (ms);
	g_assert (!is);

	g_signal_connect (watcher, "update", G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			    tcase);

	g_main_loop_run (tcase->loop);

	g_object_unref (watcher);
	test_case_destroy (tcase);
}

static void
test_ignore_only (void)
{
	GError *err;
	GXDirWatcher *watcher;
	TestCase *tcase;
	const char *dirs[] = { TESTTREE1, NULL };
	const char *ignores[] = { "dir1", NULL };
	char **ms, **is;
	const char *files[] = {
		"/tree1/file1",
		"/tree1/file2",
		"/tree1/dir2/file6.foo",
		"/tree1/dir2/file6.bar",
		NULL
	};

	err = NULL;
	tcase = test_case_new ((const char **)files);
	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				      NULL,
				      (const char *const *)ignores,
				      GX_DIR_WATCHER_FLAG_NONE, &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	g_object_get (watcher,
		      "matches", &ms,
		      "ignores", &is,
		      NULL);
	g_assert (!ms);
	g_assert_cmpstr (is[0], ==, ignores[0]);
	g_strfreev (is);

	g_signal_connect (watcher, "update", G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			    tcase);

	g_main_loop_run (tcase->loop);
	g_object_unref (watcher);
	test_case_destroy (tcase);
}

static void
test_match_and_ignore (void)
{
	GError *err;
	GXDirWatcher *watcher;
	TestCase *tcase;
	const char *dirs[] = { TESTTREE1, NULL };
	const char *ignores[] = { "dir2", NULL };
	const char *matches[] = { "\\.bar$", NULL };
	const char *files[] = {
		"/tree1/dir1/file5.bar",
		NULL
	};

	err = NULL;
	tcase = test_case_new ((const char **)files);
	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				      (const char *const *)matches,
				      (const char *const *)ignores,
				      GX_DIR_WATCHER_FLAG_NONE,
				      &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	g_signal_connect (watcher, "update", G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			    tcase);

	g_main_loop_run (tcase->loop);

	g_object_unref (watcher);
	test_case_destroy (tcase);
}

static void
test_set_matches (void)
{
	GError		*err;
	GXDirWatcher	*watcher;
	TestCase	*tcase;
	const char *dirs[]    = { TESTTREE1, NULL };
	const char *matches[] = { "\\.bar$", NULL };
	const char *files1[]  = {
		"/tree1/file1",
		"/tree1/file2",
		"/tree1/dir1/file4.foo",
		"/tree1/dir1/file5.bar",
		"/tree1/dir2/file6.foo",
		"/tree1/dir2/file6.bar",
		NULL
	};
	const char *files2[]  = {
		"/tree1/dir1/file5.bar",
		"/tree1/dir2/file6.bar",
		NULL
	};

	err	= NULL;
	tcase	= test_case_new ((const char **)files1);
	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				      NULL, NULL,
				      GX_DIR_WATCHER_FLAG_NONE, &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	tcase->sig_id = g_signal_connect (watcher, "update",
					 G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback)on_scanned,
			     tcase);
	g_main_loop_run (tcase->loop);

	g_object_set (watcher, "matches", matches, NULL);
	test_case_destroy (tcase);
	
	tcase = test_case_new ((const char **)files2);
	tcase->sig_id  = g_signal_connect (watcher, "update",
					  G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			     tcase);
	g_main_loop_run (tcase->loop);

	g_object_unref (watcher);
	test_case_destroy (tcase);

}

static void
test_set_ignores (void)
{
	GError		*err;
	GXDirWatcher	*watcher;
	TestCase	*tcase;
	const char *dirs[]    = { TESTTREE1, NULL };
	const char *ignores[] = { "dir1", NULL };
	const char *files1[]  = {
		"/tree1/file1",
		"/tree1/file2",
		"/tree1/dir1/file4.foo",
		"/tree1/dir1/file5.bar",
		"/tree1/dir2/file6.foo",
		"/tree1/dir2/file6.bar",
		NULL
	};
	const char *files2[]  = {
		"/tree1/file1",
		"/tree1/file2",
		"/tree1/dir2/file6.foo",
		"/tree1/dir2/file6.bar",
		NULL
	};

	err = NULL;
	tcase = test_case_new ((const char **)files1);
	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				      NULL, NULL,
				      GX_DIR_WATCHER_FLAG_NONE, &err);
	g_assert_no_error (err);
	g_assert (GX_IS_DIR_WATCHER (watcher));

	tcase->sig_id = g_signal_connect (watcher, "update",
					 G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback) on_scanned,
			    tcase);
	g_main_loop_run (tcase->loop);

	g_object_set (watcher, "ignores", ignores, NULL);
	test_case_destroy (tcase);
	
	tcase = test_case_new ((const char **)files2);
	tcase->sig_id =  g_signal_connect (watcher, "update",
					  G_CALLBACK (on_update), tcase);
	gx_dir_watcher_scan (watcher, NULL, (GAsyncReadyCallback)on_scanned,
			    tcase);
	g_main_loop_run (tcase->loop);
	g_object_unref (watcher);

	test_case_destroy (tcase);
}

static void
test_bad_regexps (void)
{
	GError *err;
	GXDirWatcher *watcher;
	const char *dirs[] = { TESTTREE1, NULL };
	const char *ignores[] = { "(", NULL };
	const char *matches[] = { ")", NULL };

	err = NULL;
	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				      (const char *const *)matches,
				      NULL, GX_DIR_WATCHER_FLAG_NONE,
				      &err);
	g_assert (!watcher);
	g_assert (err);
	g_clear_error (&err);

	watcher = gx_dir_watcher_new ((const char *const *)dirs,
				      NULL, (const char *const *)ignores,
				      GX_DIR_WATCHER_FLAG_NONE,
				      &err);
	g_assert (!watcher);
	g_assert (err);
	g_clear_error (&err);
}



/* static void */
/* test_updates (void) */
/* { */
/* 	GError		*err; */
/* 	GXDirWatcher	*watcher; */
/* 	char *dirs[]	      = { NULL, NULL }; */
/* 	const char *ignores[] = { "(", NULL }; */
/* 	const char *matches[] = { ")", NULL }; */
	
/* 	err	= NULL; */
/* 	dirs[0] = g_dir_make_tmp ("test-XXXXXX", NULL); */
/* 	g_assert (dirs[0]); */
/* 	watcher = gx_dir_watcher_new ((const char *const *)dirs, */
/* 				      (const char *const *)matches, */
/* 				      NULL, &err); */
/* 	g_assert (!watcher); */
/* 	g_assert (err); */
/* 	g_free (dirs[0]); */
/* 	g_clear_error (&err); */

/* 	watcher = gx_dir_watcher_new ((const char *const *)dirs, */
/* 				      NULL, (const char *const *)ignores, */
/* 				      &err); */
/* 	g_assert (!watcher); */
/* 	g_assert (err); */
/* 	g_clear_error (&err); */
/* } */



int
main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/gx-dir-watcher/props", test_props);
	g_test_add_func ("/gx-dir-watcher/no-match-no-ignore",
			test_no_match_no_ignore);
	g_test_add_func ("/gx-dir-watcher/match-only", test_match_only);
	g_test_add_func ("/gx-dir-watcher/ignore-only", test_ignore_only);
	g_test_add_func ("/gx-dir-watcher/match-and-ignore",
			test_match_and_ignore);
	g_test_add_func ("/gx-dir-watcher/set-matches", test_set_matches);
	g_test_add_func ("/gx-dir-watcher/set-ignores", test_set_ignores);
	g_test_add_func ("/gx-dir-watcher/bad-regexps", test_bad_regexps);

	return g_test_run ();
}

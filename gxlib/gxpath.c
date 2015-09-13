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

#define _XOPEN_SOURCE 500

#include <config.h>

#include <limits.h>
#include <stdlib.h>

#ifdef HAVE_WORDEXP_H
#include <wordexp.h>
#endif /*HAVE_WORDEXP_H*/

#include "gxpath.h"

/**
 * SECTION:gxpath
 * @title: File system paths
 * @short_description: helper functions for dealing with file system paths.
 *
 * Helper functions for making dealing with filenames and pathnames a bit more
 * convenient.
 */

static char*
do_wordexp (const char *path)
{
#ifndef HAVE_WORDEXP_H
  /* E.g. OpenBSD does not have wordexp.h, so we ignore it */
  return g_strdup (path)
#endif /*HAVE_WORDEXP_H*/
    
  wordexp_t wexp;
  char *dir;

  if (wordexp (path, &wexp, 0) != 0)
    return NULL;
  
  /* we just pick the first one */
  dir = g_strdup (wexp.we_wordv[0]);
  wordfree (&wexp);

  return dir;
}

/**
 * gx_path_resolve:
 * @file_name: a filename
 *
 * Perform shell-like expansion on @file_name, and resolve relative paths. The
 * latter only works for existing files. Wrapper around the library functions
 * `realpath()` and `wordexp()` (where available). If there are multiple
 * expansions, we pick the first one.
 *
 * Return value:(transfer full): the expanded/resolved path; free with g_free().
 */
char*
gx_path_resolve (const char *path)
{
  char *dir;
  char resolved[PATH_MAX + 1];

  g_return_val_if_fail (path, NULL);

  dir = do_wordexp (path);
  if (!dir)
    return NULL; /* error */

  /* now resolve any symlinks, .. etc. */
  if (!realpath (dir, resolved))
    return dir;
  else
    return g_strdup(resolved);
}

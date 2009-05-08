/* GOSM - the Gtk OpenStreetMap Tool
 *
 * Copyright (C) 2009  Sebastian Kuerten
 *
 * This file is part of Gosm.
 *
 * Gosm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gosm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gosm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "uri.h"

char * get_abs_uri(char * filename)
{
	if (filename == NULL) return NULL;
	char * abs = realpath(filename, NULL);
	if (abs == NULL) return NULL;
	char * uri_prefix = "file://";
	char * uri = malloc(sizeof(char) * (strlen(uri_prefix) + strlen(abs) + 1));
	uri[0] = '\0';
	strcat(uri, uri_prefix);
	strcat(uri, abs);
	free(abs);
	return uri;
}


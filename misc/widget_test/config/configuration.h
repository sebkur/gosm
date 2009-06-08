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

#include <glib.h>

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

typedef struct _Configuration Configuration;
typedef struct _ConfEntry ConfEntry;

struct _Configuration{
	ConfEntry * entries;
	int count;
};

struct _ConfEntry{
	char * name;
	int type;
	gpointer data_str;
	gpointer data;
};

enum {
        TYPE_BOOLEAN,
        TYPE_INT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_COLOR,
	TYPE_DIR,
	TYPE_IP
};
#endif

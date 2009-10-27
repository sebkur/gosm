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

#include <stdio.h>
#include <stdlib.h>

#include <glib.h>
#include <unistd.h>

#define GOSM_TYPE_CUSTOM_NAME		(custom_name_get_type ())
#define GOSM_CUSTOM_NAME(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_CUSTOM_NAME, CustomName))
#define GOSM_CUSTOM_NAME_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_CUSTOM_NAME, CustomNameClass))
#define GOSM_IS_CUSTOM_NAME(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_CUSTOM_NAME))
#define GOSM_IS_CUSTOM_NAME_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_CUSTOM_NAME))
#define GOSM_CUSTOM_NAME_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_CUSTOM_NAME, CustomNameClass))

typedef struct _CustomName		CustomName;
typedef struct _CustomNameClass		CustomNameClass;

struct _CustomName
{
	GObject parent;
	/* public things? */
};

struct _CustomNameClass
{
	GObjectClass parent_class;

	//void (* function_name) (CustomName *custom_name);
};

GObject * custom_name_new();

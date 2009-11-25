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

#ifndef _SXDL_FONT_H_
#define _SXDL_FONT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_base.h"

#define GOSM_TYPE_SXDL_FONT           (sxdl_font_get_type ())
#define GOSM_SXDL_FONT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_FONT, SxdlFont))
#define GOSM_SXDL_FONT_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_FONT, SxdlFontClass))
#define GOSM_IS_SXDL_FONT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_FONT))
#define GOSM_IS_SXDL_FONT_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_FONT))
#define GOSM_SXDL_FONT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_FONT, SxdlFontClass))

typedef struct _SxdlFont        SxdlFont;
typedef struct _SxdlFontClass   SxdlFontClass;

struct _SxdlFont
{
	SxdlBase parent;

	char * text;
	int size;
};

struct _SxdlFontClass
{
	SxdlBaseClass parent_class;

	//void (* function_name) (SxdlFont *sxdl_font);
};

SxdlFont * sxdl_font_new(int size);

void sxdl_font_set_text(SxdlFont * sxdl_font, char * text);

#endif /* _SXDL_FONT_H_ */

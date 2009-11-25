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

#ifndef _SXDL_BREAK_H_
#define _SXDL_BREAK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_base.h"

#define GOSM_TYPE_SXDL_BREAK           (sxdl_break_get_type ())
#define GOSM_SXDL_BREAK(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_BREAK, SxdlBreak))
#define GOSM_SXDL_BREAK_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_BREAK, SxdlBreakClass))
#define GOSM_IS_SXDL_BREAK(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_BREAK))
#define GOSM_IS_SXDL_BREAK_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_BREAK))
#define GOSM_SXDL_BREAK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_BREAK, SxdlBreakClass))

typedef struct _SxdlBreak        SxdlBreak;
typedef struct _SxdlBreakClass   SxdlBreakClass;

struct _SxdlBreak
{
	SxdlBase parent;
};

struct _SxdlBreakClass
{
	SxdlBaseClass parent_class;

	//void (* function_name) (SxdlBreak *sxdl_break);
};

SxdlBreak * sxdl_break_new();

#endif /* _SXDL_BREAK_H_ */

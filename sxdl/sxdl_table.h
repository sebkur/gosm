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

#ifndef _SXDL_TABLE_H_
#define _SXDL_TABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_SXDL_TABLE           (sxdl_table_get_type ())
#define GOSM_SXDL_TABLE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_TABLE, SxdlTable))
#define GOSM_SXDL_TABLE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_TABLE, SxdlTableClass))
#define GOSM_IS_SXDL_TABLE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_TABLE))
#define GOSM_IS_SXDL_TABLE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_TABLE))
#define GOSM_SXDL_TABLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_TABLE, SxdlTableClass))

typedef enum{
	SXDL_TABLE_VALIGN_TOP,
	SXDL_TABLE_VALIGN_BOTTOM,
	SXDL_TABLE_VALIGN_CENTER
} SxdlTableValign;

typedef enum{
	SXDL_TABLE_HALIGN_LEFT,
	SXDL_TABLE_HALIGN_RIGHT,
	SXDL_TABLE_HALIGN_CENTER
} SxdlTableHalign;

typedef struct _SxdlTable        SxdlTable;
typedef struct _SxdlTableClass   SxdlTableClass;

#include "sxdl_base.h"
#include "sxdl_table_row.h"

struct _SxdlTable
{
	SxdlBase parent;

	GArray * rows;
	int border;
	double border_r;
	double border_g;
	double border_b;
};

struct _SxdlTableClass
{
	SxdlBaseClass parent_class;

	//void (* function_name) (SxdlTable *sxdl_table);
};

SxdlTable * sxdl_table_new(int border, double r, double g, double b);

void sxdl_table_add_row(SxdlTable * table, SxdlTableRow * table_row);

#endif /* _SXDL_TABLE_H_ */

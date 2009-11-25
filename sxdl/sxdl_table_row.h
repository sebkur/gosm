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

#ifndef _SXDL_TABLE_ROW_H_
#define _SXDL_TABLE_ROW_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_base.h"
#include "sxdl_table_cell.h"

#define GOSM_TYPE_SXDL_TABLE_ROW           (sxdl_table_row_get_type ())
#define GOSM_SXDL_TABLE_ROW(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_TABLE_ROW, SxdlTableRow))
#define GOSM_SXDL_TABLE_ROW_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_TABLE_ROW, SxdlTableRowClass))
#define GOSM_IS_SXDL_TABLE_ROW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_TABLE_ROW))
#define GOSM_IS_SXDL_TABLE_ROW_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_TABLE_ROW))
#define GOSM_SXDL_TABLE_ROW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_TABLE_ROW, SxdlTableRowClass))

typedef struct _SxdlTableRow        SxdlTableRow;
typedef struct _SxdlTableRowClass   SxdlTableRowClass;

struct _SxdlTableRow
{
	SxdlBase parent;

	GArray * cells;
};

struct _SxdlTableRowClass
{
	SxdlBaseClass parent_class;

	//void (* function_name) (SxdlTableRow *sxdl_table_row);
};

SxdlTableRow * sxdl_table_row_new();

void sxdl_table_row_add_cell(SxdlTableRow * table_row, SxdlTableCell * table_cell);
int sxdl_table_row_get_n_cells(SxdlTableRow * table_row);

#endif /* _SXDL_TABLE_ROW_H_ */

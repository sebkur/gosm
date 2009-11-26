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

#ifndef _SXDL_TABLE_CELL_H_
#define _SXDL_TABLE_CELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define GOSM_TYPE_SXDL_TABLE_CELL           (sxdl_table_cell_get_type ())
#define GOSM_SXDL_TABLE_CELL(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOSM_TYPE_SXDL_TABLE_CELL, SxdlTableCell))
#define GOSM_SXDL_TABLE_CELL_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GOSM_TYPE_SXDL_TABLE_CELL, SxdlTableCellClass))
#define GOSM_IS_SXDL_TABLE_CELL(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOSM_TYPE_SXDL_TABLE_CELL))
#define GOSM_IS_SXDL_TABLE_CELL_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GOSM_TYPE_SXDL_TABLE_CELL))
#define GOSM_SXDL_TABLE_CELL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOSM_TYPE_SXDL_TABLE_CELL, SxdlTableCellClass))

typedef struct _SxdlTableCell        SxdlTableCell;
typedef struct _SxdlTableCellClass   SxdlTableCellClass;

#include "sxdl_base.h"
#include "sxdl_container.h"
#include "sxdl_table.h"

struct _SxdlTableCell
{
	SxdlBase parent;

	SxdlContainer * content;
	SxdlTableValign valign;
	SxdlTableHalign halign;
};

struct _SxdlTableCellClass
{
	SxdlBaseClass parent_class;

	//void (* function_name) (SxdlTableCell *sxdl_table_cell);
};

SxdlTableCell * sxdl_table_cell_new();
SxdlTableCell * sxdl_table_cell_new_with_align(SxdlTableValign valign, SxdlTableHalign halign);

void sxdl_table_cell_add_container(SxdlTableCell * cell, SxdlContainer * content);

#endif /* _SXDL_TABLE_CELL_H_ */

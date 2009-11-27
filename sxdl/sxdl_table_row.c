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
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_table_row.h"

G_DEFINE_TYPE (SxdlTableRow, sxdl_table_row, GOSM_TYPE_SXDL_BASE);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_table_row_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_table_row_signals[SIGNAL_NAME_n], 0);

void sxdl_table_row_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip);
void sxdl_table_row_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);

SxdlTableRow * sxdl_table_row_new()
{
	SxdlTableRow * sxdl_table_row = g_object_new(GOSM_TYPE_SXDL_TABLE_ROW, NULL);
	sxdl_table_row -> cells = g_array_new(FALSE, FALSE, sizeof(SxdlTableCell*));
	return sxdl_table_row;
}

static void sxdl_table_row_class_init(SxdlTableRowClass *class)
{
	SxdlBaseClass * sxdl_base_class = GOSM_SXDL_BASE_CLASS(class);
	sxdl_base_class -> render = sxdl_table_row_render;
	sxdl_base_class -> get_size = sxdl_table_row_get_size;
        /*sxdl_table_row_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlTableRowClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_table_row_init(SxdlTableRow *sxdl_table_row)
{
}

void sxdl_table_row_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip)
{
	SxdlTableRow * sxdl_table_row = (SxdlTableRow*)sxdl_base;
}

void sxdl_table_row_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
{
	SxdlTableRow * sxdl_table_row = (SxdlTableRow*)sxdl_base;
}

void sxdl_table_row_add_cell(SxdlTableRow * table_row, SxdlTableCell * table_cell)
{
	g_array_append_val(table_row -> cells, table_cell);
}

int sxdl_table_row_get_n_cells(SxdlTableRow * table_row)
{
	return table_row -> cells -> len;
}

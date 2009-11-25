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

#include "sxdl_table_cell.h"

G_DEFINE_TYPE (SxdlTableCell, sxdl_table_cell, GOSM_TYPE_SXDL_BASE);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_table_cell_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_table_cell_signals[SIGNAL_NAME_n], 0);

void sxdl_table_cell_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_table_cell_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);

SxdlTableCell * sxdl_table_cell_new()
{
	SxdlTableCell * sxdl_table_cell = g_object_new(GOSM_TYPE_SXDL_TABLE_CELL, NULL);
	return sxdl_table_cell;
}

static void sxdl_table_cell_class_init(SxdlTableCellClass *class)
{
	SxdlBaseClass * sxdl_base_class = GOSM_SXDL_BASE_CLASS(class);
	sxdl_base_class -> render = sxdl_table_cell_render;
	sxdl_base_class -> get_size = sxdl_table_cell_get_size;
        /*sxdl_table_cell_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlTableCellClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_table_cell_init(SxdlTableCell *sxdl_table_cell)
{
}

void sxdl_table_cell_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
{
	SxdlTableCell * cell = (SxdlTableCell*)sxdl_base;
	sxdl_container_render(cell -> content, widget, x, y, width_proposed, height_proposed, used_width, used_height);
}

void sxdl_table_cell_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed, int * used_width, int * used_height)
{
	SxdlTableCell * cell = (SxdlTableCell*)sxdl_base;
	sxdl_container_get_size(cell -> content, widget, width_proposed, height_proposed, used_width, used_height);
}

void sxdl_table_cell_add_container(SxdlTableCell * cell, SxdlContainer * content)
{
	cell -> content = content;
}

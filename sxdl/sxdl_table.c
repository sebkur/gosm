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
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "sxdl_table.h"

G_DEFINE_TYPE (SxdlTable, sxdl_table, GOSM_TYPE_SXDL_BASE);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint sxdl_table_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_table_signals[SIGNAL_NAME_n], 0);

void sxdl_table_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip);
void sxdl_table_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_table_get_sizes(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int ** s_cols, int ** s_rows);

SxdlTable * sxdl_table_new(int border, double r, double g, double b)
{
	SxdlTable * sxdl_table = g_object_new(GOSM_TYPE_SXDL_TABLE, NULL);
	sxdl_table -> rows = g_array_new(FALSE, FALSE, sizeof(SxdlTableRow*));
	sxdl_table -> border = border;
	sxdl_table -> border_r = r;
	sxdl_table -> border_g = g;
	sxdl_table -> border_b = b;
	return sxdl_table;
}

static void sxdl_table_class_init(SxdlTableClass *class)
{
	SxdlBaseClass * sxdl_base_class = GOSM_SXDL_BASE_CLASS(class);
	sxdl_base_class -> render = sxdl_table_render;
	sxdl_base_class -> get_size = sxdl_table_get_size;
        /*sxdl_table_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlTableClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void sxdl_table_init(SxdlTable *sxdl_table)
{
}

int sxdl_table_get_n_rows(SxdlTable * sxdl_table)
{
	return sxdl_table -> rows -> len;
}

int sxdl_table_get_n_columns(SxdlTable * sxdl_table)
{
	int cols = 0;
	int n;
	for (n = 0; n < sxdl_table_get_n_rows(sxdl_table); n++){
		SxdlTableRow * row = g_array_index(sxdl_table -> rows, SxdlTableRow*, n);
		int x = sxdl_table_row_get_n_cells(row);
		cols = x > cols ? x : cols;
	}
	return cols;
}

void sxdl_table_layout(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, gboolean actually_paint, Clip * clip)
{
	SxdlTable * table = (SxdlTable*)sxdl_base;
	int n_rows = sxdl_table_get_n_rows(table);
	int n_cols = sxdl_table_get_n_columns(table);
	int * size_min_cols, * size_min_cols_rows, * size_max_cols, * size_max_cols_rows, c, r, w, h;
	sxdl_table_get_sizes(sxdl_base, widget, 0, 0,   &size_min_cols, &size_min_cols_rows);
	sxdl_table_get_sizes(sxdl_base, widget, -1, -1, &size_max_cols, &size_max_cols_rows);
	int min_width = 0;
	int max_width = 0;
	for (c = 0; c < n_cols; c++){
		min_width += size_min_cols[c];
		max_width += size_max_cols[c];
	}
	int too_much = max_width - width_proposed;
	int play = max_width - min_width;
	too_much = too_much > 0 ? too_much : 0;
	int size_cols[n_cols];
	int start_cols[n_cols];
	int size_rows[n_rows];
	int start_rows[n_rows];
	start_cols[0] = 0;
	start_rows[0] = 0;
	for (c = 0; c < n_cols; c++){
		double factor = 0;
		if (play != 0) factor = ((double)too_much / (double)play);
		size_cols[c] = (int)(size_max_cols[c] -
			(size_max_cols[c] - size_min_cols[c]) * factor);
		if (c != 0){
			start_cols[c] = start_cols[c-1] + size_cols[c-1];
		}
	}
	int total_w = 0, total_h = 0, row_h;
	for (r = 0; r < n_rows; r++){
		SxdlTableRow * row = g_array_index(table -> rows, SxdlTableRow*, r);
		row_h = 0;
		for (c = 0; c < row -> cells -> len; c++){
			SxdlTableCell * cell = g_array_index(row -> cells, SxdlTableCell*, c);
			sxdl_base_get_size(GOSM_SXDL_BASE(cell), widget, size_cols[c], 0, &w, &h);
			row_h = h > row_h ? h : row_h;
		}
		size_rows[r] = row_h;
		if (r != 0){
			start_rows[r] = start_rows[r-1] + size_rows[r-1];
		}
		if (actually_paint){
			for (c = 0; c < row -> cells -> len; c++){
				SxdlTableCell * cell = g_array_index(row -> cells, SxdlTableCell*, c);
				sxdl_base_get_size(GOSM_SXDL_BASE(cell), widget, size_cols[c], 0, &w, &h);
				int ypos = y + total_h;
				int xpos = x + start_cols[c];
				switch(cell -> valign){
					case(SXDL_TABLE_VALIGN_TOP):{
						break;
					}
					case(SXDL_TABLE_VALIGN_BOTTOM):{
						ypos += (row_h - h);
						break;
					}
					case(SXDL_TABLE_VALIGN_CENTER):{
						ypos += (row_h - h) / 2;
						break;
					}
				}
				switch(cell -> halign){
					case(SXDL_TABLE_HALIGN_LEFT):{
						break;
					}
					case(SXDL_TABLE_HALIGN_RIGHT):{
						xpos += (size_cols[c] - w);
						break;
					}
					case(SXDL_TABLE_HALIGN_CENTER):{
						xpos += (size_cols[c] - w) / 2;
						break;
					}
				}
				sxdl_base_render(GOSM_SXDL_BASE(cell), widget, 
					xpos, ypos, size_cols[c], 0, &w, &h, clip);
			}
		}
		total_h += row_h;
	}
	int width = start_cols[n_cols - 1] + size_cols[n_cols - 1];
	if (actually_paint && table -> border > 0){
		cairo_t * cr_lines = gdk_cairo_create(widget -> window);
		cairo_set_line_width(cr_lines, table -> border);
		for (r = 0; r <= n_rows; r++){
			double ry = y;
			if (table -> border % 2 == 1) ry += 0.5;
			if (r == n_rows){
				ry += start_rows[r-1] + size_rows[r-1];
			}else{
				ry += start_rows[r];
			}
			cairo_move_to(cr_lines, x, ry);
			cairo_line_to(cr_lines, x + width, ry);
		}
		for (c = 0; c <= n_cols; c++){
			double cx = x;
			if (table -> border % 2 == 1) cx += 0.5;
			if (c == n_cols){
				cx += start_cols[c-1] + size_cols[c-1];
			}else{
				cx += start_cols[c];
			}
			cairo_move_to(cr_lines, cx, y);
			cairo_line_to(cr_lines, cx, y + total_h);
		}
		cairo_pattern_t * pat_lines = cairo_pattern_create_rgb(
			table -> border_r, table -> border_g, table -> border_b);
		cairo_set_source(cr_lines, pat_lines);
		cairo_stroke(cr_lines);
	}
	*used_width = width;
	*used_height = total_h;
	//printf("table height %d\n", total_h);
}

void sxdl_table_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height, Clip * clip)
{
	sxdl_table_layout(sxdl_base, widget, x, y, width_proposed, height_proposed, used_width, used_height, TRUE, clip);
}

void sxdl_table_get_sizes(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int ** s_cols, int ** s_rows)
{
	SxdlTable * table = (SxdlTable*)sxdl_base;
	int n_rows = sxdl_table_get_n_rows(table);
	int n_cols = sxdl_table_get_n_columns(table);
	int size_rows[n_rows];
	int size_cols[n_cols];
	int c, r, c_w, c_h;
	for (c = 0; c < n_cols; c++){
		size_cols[c] = 0;
	}
	for (r = 0; r < n_rows; r++){
		size_rows[r] = 0;
	}
	for (r = 0; r < n_rows; r++){
		SxdlTableRow * row = g_array_index(table -> rows, SxdlTableRow*, r);
		for (c = 0; c < row -> cells -> len; c++){
			SxdlTableCell * cell = g_array_index(row -> cells, SxdlTableCell*, c);
			sxdl_base_get_size(GOSM_SXDL_BASE(cell), widget, width_proposed, height_proposed, &c_w, &c_h);
			size_cols[c] = c_w > size_cols[c] ? c_w : size_cols[c];
			size_rows[r] = c_h > size_rows[r] ? c_h : size_rows[r];
		}
	}
	*s_cols = malloc(sizeof(int) * n_cols);
	*s_rows = malloc(sizeof(int) * n_rows); //TODO: memory leak
	for (c = 0; c < n_cols; c++){
		(*s_cols)[c] = size_cols[c];
	}
	for (r = 0; r < n_rows; r++){
		(*s_rows)[r] = size_rows[r];
	}
}

void sxdl_table_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed, int * used_width, int * used_height)
{
	if (width_proposed == 0 || width_proposed == -1){
		SxdlTable * table = (SxdlTable*)sxdl_base;
		int * size_cols, * size_rows, c, r;
		sxdl_table_get_sizes(sxdl_base, widget, width_proposed, height_proposed, &size_cols, &size_rows);
		int n_rows = sxdl_table_get_n_rows(table);
		int n_cols = sxdl_table_get_n_columns(table);
		*used_width = 0; *used_height = 0;
		for (c = 0; c < n_cols; c++){
			*used_width += size_cols[c];
		}
		for (r = 0; r < n_rows; r++){
			*used_height += size_rows[r];
		}
	}else{
		sxdl_table_layout(sxdl_base, widget, 0, 0, width_proposed, height_proposed, used_width, used_height, FALSE, NULL);
	}
}

void sxdl_table_add_row(SxdlTable * table, SxdlTableRow * table_row)
{
	g_array_append_val(table -> rows, table_row);
}


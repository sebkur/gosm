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
	int * used_width, int * used_height);
void sxdl_table_get_size(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int * used_width, int * used_height);
void sxdl_table_get_sizes(SxdlBase * sxdl_base, GtkWidget * widget, int width_proposed, int height_proposed,
	int ** s_cols, int ** s_rows);

SxdlTable * sxdl_table_new()
{
	SxdlTable * sxdl_table = g_object_new(GOSM_TYPE_SXDL_TABLE, NULL);
	sxdl_table -> rows = g_array_new(FALSE, FALSE, sizeof(SxdlTableRow*));
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

void sxdl_table_render(SxdlBase * sxdl_base, GtkWidget * widget, int x, int y, int width_proposed, int height_proposed,
	int * used_width, int * used_height)
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
	start_cols[0] = 0;
	for (c = 0; c < n_cols; c++){
		size_cols[c] = (int)(size_max_cols[c] -
			(size_max_cols[c] - size_min_cols[c]) * ((double)too_much / (double)play));
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
		for (c = 0; c < row -> cells -> len; c++){
			SxdlTableCell * cell = g_array_index(row -> cells, SxdlTableCell*, c);
			sxdl_base_get_size(GOSM_SXDL_BASE(cell), widget, size_cols[c], 0, &w, &h);
			//TODO: use valign-property
			sxdl_base_render(GOSM_SXDL_BASE(cell), widget, 
				x + start_cols[c], y + total_h + (row_h - h) / 2, size_cols[c], 0, &w, &h);
			//row_h = h > row_h ? h : row_h;
		}
		total_h += row_h;
	}
	*used_width = start_cols[n_cols] + size_cols[n_cols];
	*used_height = total_h;
	printf("table height %d\n", total_h);
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
		//TODO: duplicate code. this is the render code
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
		start_cols[0] = 0;
		for (c = 0; c < n_cols; c++){
			size_cols[c] = (int)(size_max_cols[c] -
				(size_max_cols[c] - size_min_cols[c]) * ((double)too_much / (double)play));
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
			for (c = 0; c < row -> cells -> len; c++){
				SxdlTableCell * cell = g_array_index(row -> cells, SxdlTableCell*, c);
				sxdl_base_get_size(GOSM_SXDL_BASE(cell), widget, size_cols[c], 0, &w, &h);
			}
			total_h += row_h;
		}
		*used_width = start_cols[n_cols] + size_cols[n_cols];
		*used_height = total_h;
	}
}

void sxdl_table_add_row(SxdlTable * table, SxdlTableRow * table_row)
{
	g_array_append_val(table -> rows, table_row);
}


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
#include <expat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>

#include "sxdl_widget.h"
#include "sxdl_break.h"
#include "sxdl_container.h"
#include "sxdl_font.h"
#include "sxdl_image.h"
#include "sxdl_table_cell.h"
#include "sxdl_table.h"
#include "sxdl_table_row.h"

G_DEFINE_TYPE (SxdlWidget, sxdl_widget, GTK_TYPE_DRAWING_AREA);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

enum {
	TAG_FONT,
	TAG_IMG,
	TAG_TABLE,
	TAG_TR,
	TAG_TD,
	TAG_BR,
	TAG_OTHER
};

//static guint sxdl_widget_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, sxdl_widget_signals[SIGNAL_NAME_n], 0);

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event);
void parse_file(SxdlWidget * sxdl, char * filename);
static gboolean sxdl_widget_set_scroll_adjustments(GtkWidget * widget, GtkAdjustment * hadj, GtkAdjustment * vadj);
static void sxdl_widget_adjustment_changed(GtkAdjustment * adj, SxdlWidget * sxdl);
void sxdl_widget_update_adjustments(SxdlWidget * sxdl);

SxdlWidget * sxdl_widget_new()
{
	SxdlWidget * sxdl_widget = g_object_new(GOSM_TYPE_SXDL_WIDGET, NULL);
	sxdl_widget -> offset_h = 0;
	sxdl_widget -> offset_v = 0;
	g_signal_connect(
		G_OBJECT(sxdl_widget), "set-scroll-adjustments",
		G_CALLBACK(sxdl_widget_set_scroll_adjustments), NULL);
	return sxdl_widget;
}

static gboolean sxdl_widget_set_scroll_adjustments(GtkWidget * widget, GtkAdjustment * hadj, GtkAdjustment * vadj)
{
	SxdlWidget * sxdl = GOSM_SXDL_WIDGET(widget);
	sxdl -> hadj = hadj;
	sxdl -> vadj = vadj;
	gtk_adjustment_set_step_increment(hadj, 10);
	gtk_adjustment_set_step_increment(vadj, 10);
	g_signal_connect (
		G_OBJECT(vadj), "value-changed",
		G_CALLBACK(sxdl_widget_adjustment_changed), sxdl);
	g_signal_connect (
		G_OBJECT(hadj), "value-changed",
		G_CALLBACK(sxdl_widget_adjustment_changed), sxdl);
	return TRUE;
}

static void sxdl_widget_adjustment_changed(GtkAdjustment * adj, SxdlWidget * sxdl)
{
	double val = gtk_adjustment_get_value(adj);
	if (adj == sxdl -> vadj){
		sxdl -> offset_v = -val;
		gtk_widget_queue_draw(sxdl);
	}
	if (adj == sxdl -> hadj){
		sxdl -> offset_h = -val;
		gtk_widget_queue_draw(sxdl);
	}
}

void sxdl_widget_update_adjustments(SxdlWidget * sxdl)
{
	gtk_adjustment_set_page_size(sxdl -> vadj, sxdl -> height_visible);
	gtk_adjustment_set_upper(sxdl -> vadj, sxdl -> height_total);
	gtk_adjustment_set_page_size(sxdl -> hadj, sxdl -> width_visible);
	gtk_adjustment_set_upper(sxdl -> hadj, sxdl -> width_total);
}

void sxdl_widget_set_base_path(SxdlWidget * sxdl_widget, char * uri)
{
	char * abs;
	if (g_str_has_prefix(uri, "file://")){
		abs = &uri[7];
	}else{
		abs = uri;
	}
	sxdl_widget -> full_path = g_strdup(abs);
	char * dn = dirname(abs);
	sxdl_widget -> base_path = g_strconcat(dn, "/", NULL);
}

void sxdl_widget_set_uri(SxdlWidget * sxdl_widget, char * uri)
{
	sxdl_widget -> stack = g_array_new(FALSE, FALSE, sizeof(SxdlWidget*));
	sxdl_widget -> document = sxdl_container_new();
	g_array_append_val(sxdl_widget -> stack, sxdl_widget -> document);
	sxdl_widget_set_base_path(sxdl_widget, uri);
	parse_file(sxdl_widget, sxdl_widget -> full_path);
}

static void sxdl_widget_class_init(SxdlWidgetClass *class)
{
	GObjectClass *object_class;
	GtkWidgetClass *widget_class;
	object_class = (GObjectClass*)class;
	widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> expose_event = expose_cb;
        /*sxdl_widget_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (SxdlWidgetClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
	widget_class->set_scroll_adjustments_signal = g_signal_new (
		"set-scroll-adjustments",
		G_TYPE_FROM_CLASS (object_class),
		G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		G_STRUCT_OFFSET (GtkTreeViewClass, set_scroll_adjustments),
		NULL, NULL,
		gtk_marshal_VOID__POINTER_POINTER,
		G_TYPE_NONE, 2,
		GTK_TYPE_ADJUSTMENT,
		GTK_TYPE_ADJUSTMENT);
}

static void sxdl_widget_init(SxdlWidget *sxdl_widget)
{
}

void read_hex_colour_int(char * hex, int * r, int * g, int * b)
{
	*r = 0; *g = 0; *b = 0;
	if (strlen(hex) != 7) return;
	char hash[2]; strncpy(hash, hex, 1); hash[1] = '\0';
	char pr[3]; strncpy(pr, &hex[1], 2); pr[2] = '\0';
	char pg[3]; strncpy(pg, &hex[3], 2); pg[2] = '\0';
	char pb[3]; strncpy(pb, &hex[5], 2); pb[2] = '\0';
	*r = strtol(pr, (char **) NULL, 16);
	*g = strtol(pg, (char **) NULL, 16);
	*b = strtol(pb, (char **) NULL, 16);
}

void read_hex_colour_double(char * hex, double * r, double * g, double * b)
{
	int ir, ig, ib;
	read_hex_colour_int(hex, &ir, &ig, &ib);
	*r = ((double)ir)/((double)255);
	*g = ((double)ig)/((double)255);
	*b = ((double)ib)/((double)255);
}

const char * sxdl_widget_get_value(const XML_Char ** atts, char * search_key)
{
	const XML_Char ** ptr = atts;
	while(*ptr != NULL){
		const XML_Char * key = *ptr;
		const XML_Char * value = *(ptr + 1);
		if (strcmp(key, search_key) == 0){
			return value;
		}
		ptr = ptr + 2;
	}
	return NULL;
}

static void XMLCALL sxdl_widget_StartElementCallback(
						void * userData,
						const XML_Char * name,
						const XML_Char ** atts)
{
	SxdlWidget * sxdl = (SxdlWidget*)userData;
	//printf("start %s\n", name);
	      if (strcmp(name, "font") == 0){
		sxdl -> tag = TAG_FONT;
		SxdlFont * e = sxdl_font_new(10);
		g_array_append_val(sxdl -> stack, e);
	}else if (strcmp(name, "br") == 0){
		sxdl -> tag = TAG_BR;
		SxdlBreak * e = sxdl_break_new();
		g_array_append_val(sxdl -> stack, e);
	}else if (strcmp(name, "img") == 0){
		sxdl -> tag = TAG_IMG;
		SxdlImage * e = sxdl_image_new(sxdl -> base_path, sxdl_widget_get_value(atts, "src"));
		g_array_append_val(sxdl -> stack, e);
	}else if (strcmp(name, "table") == 0){
		int border = 0;
		char * border_s = sxdl_widget_get_value(atts, "border");
		if (border_s != NULL) border = atoi(border_s);
		double r = 0, g = 0, b = 0;
		char * border_colour_s = sxdl_widget_get_value(atts, "bordercolor");
		if (border_colour_s != NULL) read_hex_colour_double(border_colour_s, &r, &g, &b);
		//printf("%e %e %e\n", r, g, b);
		sxdl -> tag = TAG_TABLE;
		SxdlTable * e = sxdl_table_new(border, r, g, b);
		g_array_append_val(sxdl -> stack, e);
	}else if (strcmp(name, "tr") == 0){
		sxdl -> tag = TAG_TR;
		SxdlTableRow * e = sxdl_table_row_new();
		g_array_append_val(sxdl -> stack, e);
	}else if (strcmp(name, "td") == 0){
		sxdl -> tag = TAG_TD;
		SxdlTableValign valign;
		SxdlTableHalign halign;
		char * valign_s = sxdl_widget_get_value(atts, "valign");
		char * halign_s = sxdl_widget_get_value(atts, "halign");
		if (valign_s == NULL){
			valign = SXDL_TABLE_VALIGN_TOP;
		}else if (strcmp(valign_s, "bottom") == 0){
			valign = SXDL_TABLE_VALIGN_BOTTOM;
		}else if (strcmp(valign_s, "center") == 0){
			valign = SXDL_TABLE_VALIGN_CENTER;
		}else{
			valign = SXDL_TABLE_VALIGN_TOP;
		}
		if (halign_s == NULL){
			halign = SXDL_TABLE_HALIGN_LEFT;
		}else if (strcmp(halign_s, "right") == 0){
			halign = SXDL_TABLE_HALIGN_RIGHT;
		}else if (strcmp(halign_s, "center") == 0){
			halign = SXDL_TABLE_HALIGN_CENTER;
		}else{
			halign = SXDL_TABLE_HALIGN_LEFT;
		}
		SxdlTableCell * e = sxdl_table_cell_new_with_align(valign, halign);
		SxdlContainer * c = sxdl_container_new();
		sxdl_table_cell_add_container(e, c);
		g_array_append_val(sxdl -> stack, e);
		g_array_append_val(sxdl -> stack, c);
	}else{
		sxdl -> tag = TAG_OTHER;
	}
}

static void XMLCALL sxdl_widget_EndElementCallback(
					void * userData,
					const XML_Char * name)
{
	SxdlWidget * sxdl = (SxdlWidget*)userData;
	SxdlBase * e = g_array_index(sxdl -> stack, SxdlBase*, sxdl -> stack -> len - 1);
	      if (strcmp(name, "font") == 0){
		SxdlFont * f = GOSM_SXDL_FONT(e);
		sxdl_font_set_text(f, sxdl -> text);
		SxdlContainer * c = g_array_index(sxdl -> stack, SxdlContainer*, sxdl -> stack -> len - 2);
		sxdl_container_add(c, GOSM_SXDL_BASE(e));
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		sxdl -> tag = TAG_OTHER;
	}else if (strcmp(name, "br") == 0){
		SxdlContainer * c = g_array_index(sxdl -> stack, SxdlContainer*, sxdl -> stack -> len - 2);
		sxdl_container_add(c, GOSM_SXDL_BASE(e));
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		sxdl -> tag = TAG_OTHER;
	}else if (strcmp(name, "img") == 0){
		SxdlContainer * c = g_array_index(sxdl -> stack, SxdlContainer*, sxdl -> stack -> len - 2);
		sxdl_container_add(c, GOSM_SXDL_BASE(e));
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		sxdl -> tag = TAG_OTHER;
	}else if (strcmp(name, "table") == 0){
		//printf("TABLE\n");
		SxdlContainer * c = g_array_index(sxdl -> stack, SxdlContainer*, sxdl -> stack -> len - 2);
		sxdl_container_add(c, GOSM_SXDL_BASE(e));
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		sxdl -> tag = TAG_OTHER;
	}else if (strcmp(name, "tr") == 0){
		//printf("TR\n");
		SxdlTable * c = g_array_index(sxdl -> stack, SxdlTable*, sxdl -> stack -> len - 2);
		sxdl_table_add_row(c, GOSM_SXDL_TABLE_ROW(e));
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		sxdl -> tag = TAG_TABLE;
	}else if (strcmp(name, "td") == 0){
		//printf("TD\n");
		SxdlTableRow * r = g_array_index(sxdl -> stack, SxdlTableRow*, sxdl -> stack -> len - 3);
		SxdlTableCell * c = g_array_index(sxdl -> stack, SxdlTableCell*, sxdl -> stack -> len - 2);
		sxdl_table_row_add_cell(r, GOSM_SXDL_TABLE_CELL(c));
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		g_array_remove_index(sxdl -> stack, sxdl -> stack -> len - 1);
		sxdl -> tag = TAG_TR;
	}else{
	}
}

static void XMLCALL sxdl_widget_CharacterDataCallback(
					void * userData,
					const XML_Char * s,
					int len)
{
	SxdlWidget * sxdl = (SxdlWidget*)userData;
	char * st = malloc(sizeof(char) * (len+1));
	strncpy(st, s, len);
	st[len] = '\0';
	sxdl -> text = st;
}

void parse_file(SxdlWidget * sxdl, char * filename)
{
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, sxdl);
	XML_SetStartElementHandler(parser, sxdl_widget_StartElementCallback);
	XML_SetEndElementHandler(parser, sxdl_widget_EndElementCallback);
	XML_SetCharacterDataHandler(parser, sxdl_widget_CharacterDataCallback);

	struct stat sb;
	int s = stat(filename, &sb);
	if (s < 0){
		return;
	}
	int filesize = sb.st_size;
	int f = open(filename, O_RDONLY);
	if (f == 0) {
		return;
	}
	ssize_t size;
	int n = 1000;
	char buf[n];
	do{
		size = read(f, buf, n);
		XML_Parse(parser, buf, size, 0);
	}while(size > 0);
	XML_Parse(parser, NULL, 0, 1);
}

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event)
{
	SxdlWidget * sxdl = (SxdlWidget*)widget;
	sxdl -> event = event;
	int width  = widget -> allocation.width;
	int height = widget -> allocation.height;
	/* bg */
	cairo_t * cr = gdk_cairo_create(widget -> window);
	cairo_pattern_t * pat = cairo_pattern_create_rgba(1.0, 1.0, 1.0, 1.0);
	cairo_set_source(cr, pat);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	if (sxdl -> height_total + sxdl -> offset_v < sxdl -> height_visible && sxdl -> offset_v != 0){
		sxdl -> offset_v = sxdl -> height_visible - sxdl -> height_total;
		gtk_adjustment_set_value(sxdl -> vadj, -sxdl -> offset_v);
		gtk_adjustment_value_changed(sxdl -> vadj);
	}
	if (sxdl -> width_total + sxdl -> offset_h < sxdl -> width_visible && sxdl -> offset_h != 0){
		sxdl -> offset_h = sxdl -> width_visible - sxdl -> width_total;
		gtk_adjustment_set_value(sxdl -> hadj, -sxdl -> offset_h);
		gtk_adjustment_value_changed(sxdl -> hadj);
	}

	int min_w, min_w_h, max_w, max_w_h, w, h;
	sxdl_base_get_size(GOSM_SXDL_BASE(sxdl -> document), widget, 0, 0, &min_w, &min_w_h);
	sxdl_base_get_size(GOSM_SXDL_BASE(sxdl -> document), widget, -1, -1, &max_w, &max_w_h);
	//printf("%dx%d %dx%d\n", min_w, min_w_h, max_w, max_w_h);
	int layout_w = min_w > width ? min_w : width;
	sxdl_base_render(GOSM_SXDL_BASE(sxdl -> document), widget, 
		sxdl -> offset_h, sxdl -> offset_v, layout_w, -1, &w, &h);

	gboolean adjust = FALSE;
	if (sxdl -> height_visible != height){
		sxdl -> height_total = h;
		sxdl -> height_visible = height;
		if (height >= h){
			sxdl -> offset_v = 0;
			gtk_widget_queue_draw(sxdl);
		}
		adjust = TRUE;
	}
	if (sxdl -> width_visible != width){
		sxdl -> width_total = w;
		sxdl -> width_visible = width;
		if (width >= w){
			sxdl -> offset_h = 0;
			gtk_widget_queue_draw(sxdl);
		}
		adjust = TRUE;
	}
	if (adjust){
		sxdl_widget_update_adjustments(sxdl);
	}
	
	return FALSE;
}

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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <cairo/cairo.h>

#include <sys/time.h>
#include <string.h>

#include "map_area.h"
#include "map_types.h"
#include "tool.h"
#include "tilemath.h"
#include "tile_cache.h"
#include "tile_manager.h"

#define MATCHES(a,b)	(a & b) == b
#define TILESIZE	256
#define CACHE_SIZE	164

#define DIRECTION_TOP_LEFT	1
#define DIRECTION_TOP		2
#define DIRECTION_TOP_RIGHT	3
#define DIRECTION_LEFT		4
#define DIRECTION_RIGHT		5
#define DIRECTION_DOWN_LEFT	6
#define DIRECTION_DOWN		7
#define DIRECTION_DOWN_RIGHT	8

G_DEFINE_TYPE (MapArea, map_area, GTK_TYPE_DRAWING_AREA);

enum
{
	MAP_BEEN_MOVED,
	MAP_PATH_CHANGED,
	LAST_SIGNAL
};

static guint map_area_signals[LAST_SIGNAL] = { 0 };
// TODO: put in object

Point point_drag;
GdkPixmap *no_pic;

struct timeval t1, t2;

gboolean map_moved = TRUE;

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event);
static gboolean mouse_button_cb(GtkWidget *widget, GdkEventButton *event);
static gboolean mouse_motion_cb(GtkWidget *widget, GdkEventMotion *event);
static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);

GtkWidget * map_area_new()
{
	return g_object_new(GOSM_TYPE_MAP_AREA, NULL);
}

static void map_area_class_init(MapAreaClass *class)
{
	GtkWidgetClass *widget_class;
	widget_class = GTK_WIDGET_CLASS(class);
	widget_class -> expose_event = expose_cb;

	map_area_signals[MAP_BEEN_MOVED] = g_signal_new(
		"map-been-moved",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_been_moved),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);

	map_area_signals[MAP_PATH_CHANGED] = g_signal_new(
		"map-path-changed",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET (MapAreaClass, map_been_moved),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0);
}

void map_area_set_cache_directory(MapArea * map_area, char * directory)
{
	if (map_area -> cache_dir != NULL){
		free(map_area -> cache_dir);
	}
	map_area -> cache_dir = malloc(sizeof(char) * strlen(directory));
	strcpy(map_area -> cache_dir, directory);
	tile_manager_set_cache_directory(map_area -> tile_manager, directory);
}

void map_area_set_network_state(MapArea *map_area, gboolean state)
{
	tile_manager_set_network_state(map_area -> tile_manager, state);
	// TODO change state of tileloader
}

static void tile_loaded_cb(TileManager * tile_manager, MapArea *map_area)
{
	printf("received a repaint signal\n");
	map_moved = TRUE;
	gdk_threads_enter();
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
	gdk_threads_leave();
}

static void map_area_init(MapArea *map_area)
{
	// TODO: this initialization is useless!
	map_area -> map_position.width  	= 1280;
	map_area -> map_position.height 	= 600;
	map_area -> map_position.zoom 		= 11;
	double x 				= lon_to_x(13.411, 11) - 500/256.0;
	double y 				= lat_to_y(52.523, 11) - 300/256.0;
	map_area -> map_position.tile_top 	= (int)y;
	map_area -> map_position.tile_left 	= (int)x;
	map_area -> map_position.tile_offset_y 	= ((int)(y * 256.0)) % 256;
	map_area -> map_position.tile_offset_x 	= ((int)(x * 256.0)) % 256;
	map_area -> map_position.tile_count_x 	= 2;
	map_area -> map_position.tile_count_y 	= 2;

	map_area -> action_state		= -1;
	map_area -> show_grid			= FALSE;
	map_area -> show_font			= FALSE;
	map_area -> show_selection		= TRUE;
	map_area -> snap_selection		= TRUE;
	// end useless part

	map_area -> selection.x1 		= 0;
	map_area -> selection.x2 		= 0;
	map_area -> selection.y1 		= 0;
	map_area -> selection.y2 		= 0;

	map_area -> path			= g_list_alloc();

	g_signal_connect(G_OBJECT(map_area), "motion_notify_event",	G_CALLBACK(mouse_motion_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "button_press_event", 	G_CALLBACK(mouse_button_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "button_release_event",	G_CALLBACK(mouse_button_cb),	NULL);
	g_signal_connect(G_OBJECT(map_area), "key_press_event",		G_CALLBACK(key_press_cb),	NULL);

	// don't use GDK_KEY_PRESS, old versions of gtk don't realize motion-notifys anymore
	gtk_widget_set_events(GTK_WIDGET(map_area), gtk_widget_get_events(GTK_WIDGET(map_area)) |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_EXPOSURE_MASK );
	map_area -> tile_manager = GOSM_TILE_MANAGER(tile_manager_new());
	g_signal_connect(G_OBJECT(map_area -> tile_manager), "tile_loaded_from_disk", G_CALLBACK(tile_loaded_cb), map_area);
}

int map_area_position_get_center_x(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	return map_position -> tile_left * 256 + map_position -> tile_offset_x + map_position -> width/2;
}

int map_area_position_get_center_y(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	return map_position -> tile_top * 256 + map_position -> tile_offset_y + map_position -> height/2;
}

double map_area_position_get_center_lon(MapArea *map_area)
{
	return x_to_lon(map_area_position_get_center_x(map_area) / 256.0, map_area -> map_position.zoom);
}

double map_area_position_get_center_lat(MapArea *map_area)
{
	return y_to_lat(map_area_position_get_center_y(map_area) / 256.0, map_area -> map_position.zoom);
}

int map_area_lon_to_area_x(MapArea *map_area, double lon)
{
	MapPosition *map_position = &(map_area -> map_position);
	double x = lon_to_x(lon, map_position -> zoom);
	return (x - map_position -> tile_left - map_position -> tile_offset_x / 256.0) * 256.0;
}

int map_area_lat_to_area_y(MapArea *map_area, double lat)
{
	MapPosition *map_position = &(map_area -> map_position);
	double y = lat_to_y(lat, map_position -> zoom);
	return (y - map_position -> tile_top - map_position -> tile_offset_y / 256.0) * 256.0;
}

double map_area_x_to_lon(MapArea *map_area, int x)
{
	MapPosition *map_position = &(map_area -> map_position);
	double pos = map_position -> tile_left + map_position -> tile_offset_x / 256.0 + x / 256.0;
	return x_to_lon(pos, map_position -> zoom);
}

double map_area_y_to_lat(MapArea *map_area, int y)
{
	MapPosition *map_position = &(map_area -> map_position);
	double pos = map_position -> tile_top + map_position -> tile_offset_y / 256.0 + y / 256.0;
	return y_to_lat(pos, map_position -> zoom);
}

void map_area_repaint(MapArea *map_area)
{
	map_area -> need_repaint = TRUE;
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

void map_area_goto_lon_lat_zoom(MapArea *map_area, double lon, double lat, int zoom)
{
	map_area -> map_position.lon = lon;
	map_area -> map_position.lat = lat;
	map_area -> map_position.zoom = zoom;
	printf("%f %f %d\n", lon, lat, zoom);
	double x = lon_to_x(lon, zoom) - map_area -> width/2/256.0;
	double y = lat_to_y(lat, zoom) - map_area -> height/2/256.0;
	map_area -> map_position.tile_top       = (int)y;
	map_area -> map_position.tile_left      = (int)x;
	map_area -> map_position.tile_offset_y  = ((int)(y * 256.0)) % 256;
	map_area -> map_position.tile_offset_x  = ((int)(x * 256.0)) % 256;
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_BEEN_MOVED], 0);
}

void map_has_moved(GtkWidget *widget)
{
	MapArea *map_area = GOSM_MAP_AREA(widget);
	map_moved = TRUE;
	if (map_area -> snap_selection){
		map_area -> selection.x1 = map_area_lon_to_area_x(map_area, map_area -> selection.lon1);
		map_area -> selection.x2 = map_area_lon_to_area_x(map_area, map_area -> selection.lon2);
		map_area -> selection.y1 = map_area_lat_to_area_y(map_area, map_area -> selection.lat1);
		map_area -> selection.y2 = map_area_lat_to_area_y(map_area, map_area -> selection.lat2);
	}else{
		map_area -> selection.lon1 = map_area_x_to_lon(map_area, map_area -> selection.x1),
		map_area -> selection.lon2 = map_area_x_to_lon(map_area, map_area -> selection.x2),
		map_area -> selection.lat1 = map_area_y_to_lat(map_area, map_area -> selection.y1),
		map_area -> selection.lat2 = map_area_y_to_lat(map_area, map_area -> selection.y2);
	}
	map_area -> map_position.lon = map_area_x_to_lon(map_area, map_area -> map_position.width / 2);
	map_area -> map_position.lat = map_area_y_to_lat(map_area, map_area -> map_position.height / 2);
	g_signal_emit (widget, map_area_signals[MAP_BEEN_MOVED], 0);
}

void map_area_zoom_in(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	map_position -> zoom += 1;
	int middle_x_old = map_position -> tile_left * 256 + map_position -> tile_offset_x + map_position -> width/2;
	int corner_x_new = middle_x_old * 2 - map_position -> width/2;
	map_position -> tile_offset_x = corner_x_new % 256;
	map_position -> tile_left = corner_x_new / 256;
	int middle_y_old = map_position -> tile_top * 256 + map_position -> tile_offset_y + map_position -> height/2;
	int corner_y_new = middle_y_old * 2 - map_position -> height/2;
	map_position -> tile_offset_y = corner_y_new % 256;
	map_position -> tile_top = corner_y_new / 256;

	map_has_moved(GTK_WIDGET(map_area));
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

void map_area_zoom_out(MapArea *map_area)
{
	MapPosition *map_position = &(map_area -> map_position);
	map_position -> zoom -= 1;
	int middle_x_old = map_position -> tile_left * 256 + map_position -> tile_offset_x + map_position -> width/2;
	int corner_x_new = middle_x_old / 2.0 - map_position -> width/2;
	map_position -> tile_offset_x = corner_x_new % 256;
	map_position -> tile_left = corner_x_new / 256;
	int middle_y_old = map_position -> tile_top * 256 + map_position -> tile_offset_y + map_position -> height/2;
	int corner_y_new = middle_y_old / 2.0 - map_position -> height/2;
	map_position -> tile_offset_y = corner_y_new % 256;
	map_position -> tile_top = corner_y_new / 256;
	
	map_has_moved(GTK_WIDGET(map_area));
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

void path_add_point(MapArea *map_area, double lon, double lat)
{
	LonLatPair * llp = malloc(sizeof(LonLatPair));
	llp -> lon = lon;
	llp -> lat = lat;
	map_area -> path = g_list_append(map_area -> path, llp);
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_PATH_CHANGED], 0);
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

void map_area_path_remove_point(MapArea *map_area)
{
	if (g_list_length(map_area -> path) > 1){
		GList * last = g_list_last(map_area -> path);
		free(last->data);
		map_area -> path = g_list_remove_link(map_area -> path, last);
		g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_PATH_CHANGED], 0);
		gtk_widget_queue_draw(GTK_WIDGET(map_area));
	}
}

void map_area_path_clear(MapArea *map_area)
{
	
	GList * node = map_area -> path -> next;
	while(node != NULL){
		free(node -> data);
		map_area -> path = g_list_remove_link(map_area -> path, node);
		node = map_area -> path -> next;
	}
	g_signal_emit (GTK_WIDGET(map_area), map_area_signals[MAP_PATH_CHANGED], 0);
	gtk_widget_queue_draw(GTK_WIDGET(map_area));
}

static gboolean mouse_button_cb(GtkWidget *widget, GdkEventButton *event)
{
	MapArea *map_area = GOSM_MAP_AREA(widget);
	gtk_widget_grab_focus(widget);
	double lon = map_area_x_to_lon(map_area, (int) event -> x);
	double lat = map_area_y_to_lat(map_area, (int) event -> y);
	printf("button %f %f\n", lon, lat);
	point_drag.x = (int) event -> x;
	point_drag.y = (int) event -> y;
	if (event -> type == GDK_2BUTTON_PRESS){
		if (event -> button == 1){
			map_area_zoom_in(map_area);
		}
		if (event -> button == 3){
			map_area_zoom_out(map_area);
		}
	}
	if (event -> type == GDK_BUTTON_PRESS && map_area -> action_state == 2){
		path_add_point(map_area, lon, lat);
	}
}

static gboolean mouse_motion_cb(GtkWidget *widget, GdkEventMotion *event)
{
	//if (action_state == 2){
		//printf("motion\n");
	//}
	MapArea *map_area = GOSM_MAP_AREA(widget);
	MapPosition *map_position = &(map_area -> map_position);
	if (map_area -> action_state == 0){
		if (MATCHES(event->state, GDK_BUTTON1_MASK)){
			//printf("motion %d\n", event->state);
			//printf("x: %d, y: %d\n", (int)event->x, (int)event->y);
			int diff_x = ((int) event -> x) - point_drag.x;
			int diff_y = ((int) event -> y) - point_drag.y;
			point_drag.x = (int) event -> x;
			point_drag.y = (int) event -> y;
			//printf("motion %d %d\n", diff_x, diff_y);
			map_position -> tile_offset_x -= diff_x;
			map_position -> tile_offset_y -= diff_y;
			while (map_position -> tile_offset_x >= TILESIZE){
				map_position -> tile_offset_x -= TILESIZE;
				map_position -> tile_left += 1;
			}
			while (map_position -> tile_offset_x < 0){
				map_position -> tile_offset_x += TILESIZE;
				map_position -> tile_left -= 1;
			}
			while (map_position -> tile_offset_y >= TILESIZE){
				map_position -> tile_offset_y -= TILESIZE;
				map_position -> tile_top += 1;
			}
			while (map_position -> tile_offset_y < 0){
				map_position -> tile_offset_y += TILESIZE;
				map_position -> tile_top -= 1;
			}
			//printf("offset now: %d\n", map_position -> tile_offset_x);
			map_has_moved(widget);
			gtk_widget_queue_draw(widget);
		}
	}
	if (map_area -> action_state == 1){
		if (MATCHES(event->state, GDK_BUTTON1_MASK)){
			if (point_drag.x > (int) event -> x){	
				map_area -> selection.x1 = (int) event -> x;
				map_area -> selection.x2 = point_drag.x;
			}else{	
				map_area -> selection.x2 = (int) event -> x;
				map_area -> selection.x1 = point_drag.x;
			}
			if (point_drag.y > (int) event -> y){
				map_area -> selection.y1 = (int) event -> y;
				map_area -> selection.y2 = point_drag.y;
			}else{
				map_area -> selection.y2 = (int) event -> y;
				map_area -> selection.y1 = point_drag.y;
			}
			map_area -> selection.lon1 = map_area_x_to_lon(map_area, map_area -> selection.x1),
			map_area -> selection.lon2 = map_area_x_to_lon(map_area, map_area -> selection.x2),
			map_area -> selection.lat1 = map_area_y_to_lat(map_area, map_area -> selection.y1),
			map_area -> selection.lat2 = map_area_y_to_lat(map_area, map_area -> selection.y2);

			map_has_moved(widget);
			gtk_widget_queue_draw(widget);
		}
	}
}

static gboolean button_move_function(GtkWidget *widget, gpointer direction)
{
	MapArea *map_area = GOSM_MAP_AREA(widget);
	MapPosition * map_position = &(map_area -> map_position);
	int dir = GPOINTER_TO_INT(direction);
	switch(dir){
		case DIRECTION_LEFT:
		case DIRECTION_TOP_LEFT:
		case DIRECTION_DOWN_LEFT:
			map_position -> tile_left -= 1; break;
		case DIRECTION_RIGHT:
		case DIRECTION_TOP_RIGHT:
		case DIRECTION_DOWN_RIGHT:
			map_position -> tile_left += 1; break;
	}
	switch(dir){
		case DIRECTION_TOP:
		case DIRECTION_TOP_LEFT:
		case DIRECTION_TOP_RIGHT:
			map_position -> tile_top -= 1; break;
		case DIRECTION_DOWN:
		case DIRECTION_DOWN_LEFT:
		case DIRECTION_DOWN_RIGHT:
			map_position -> tile_top += 1; break;
	}
	map_has_moved(widget);
	gtk_widget_queue_draw(widget);
}

void move(GtkWidget *widget, int direction)
{
	button_move_function(widget, GINT_TO_POINTER(direction));
}

static gboolean expose_cb(GtkWidget *widget, GdkEventExpose *event)
{
	MapArea *map_area = GOSM_MAP_AREA(widget);
	MapPosition *map_position = &(map_area -> map_position);

	gboolean recreate = FALSE;
	// create Pixmap-Buffer if first time of drawing
	if (map_area -> pixmap == NULL
			|| widget -> allocation.width != map_area -> width
			|| widget -> allocation.height != map_area -> height){
		recreate = TRUE;
		map_area -> width = widget->allocation.width;
		map_area -> height = widget->allocation.height;
		map_position -> width = map_area -> width;
		map_position -> height = map_area -> height;

		// capsle
		double x = lon_to_x(map_position -> lon, map_position -> zoom) - map_area -> width/2/256.0;
		double y = lat_to_y(map_position -> lat, map_position -> zoom) - map_area -> height/2/256.0;
		map_area -> map_position.tile_top 	= (int)y;
		map_area -> map_position.tile_left 	= (int)x;
		map_area -> map_position.tile_offset_y 	= ((int)(y * 256.0)) % 256;
		map_area -> map_position.tile_offset_x 	= ((int)(x * 256.0)) % 256;	
		// capsle

		if (map_area -> pixmap != NULL) g_object_unref(map_area -> pixmap);
		map_area -> pixmap = gdk_pixmap_new(widget->window,
				widget->allocation.width,
				widget->allocation.height, -1);
		map_has_moved(GTK_WIDGET(map_area));
	}
	int width = map_area -> width;
	int height = map_area -> height;
	int width_1 = width - TILESIZE +  map_position -> tile_offset_x;
	int height_1 = height - TILESIZE +  map_position -> tile_offset_y;
	width_1 = width_1 > 0 ? width_1 : 0;
	height_1 = height_1 > 0 ? height_1 :0;
	map_position -> tile_count_x = width_1 / 256 + 2;
	map_position -> tile_count_y = height_1 / 256 + 2;

	//printf("tiles: %d\n", map_position -> tile_count_x * map_position -> tile_count_y);
	if (map_moved){
		map_moved = FALSE;
		recreate = TRUE;
	}
	if (map_area -> need_repaint){
		map_area -> need_repaint = FALSE;
		recreate = TRUE;
	}
	if(recreate){
		gdk_draw_rectangle(map_area -> pixmap,
				widget->style->white_gc,
				TRUE, 0, 0, 
				widget -> allocation.width,
				widget -> allocation.height);

		char path[100];
		int x, y = 0; int t_x, t_y; int c;
		for (x = 0; x < map_position -> tile_count_x; x++){
		  for(y = 0; y < map_position -> tile_count_y; y++){
			GdkPixbuf *pixbuf;
			GdkPixmap *pixmap;
			t_x = map_position -> tile_left + x;
			t_y = map_position -> tile_top  + y;

			gboolean in_cache = FALSE;

			//gpointer pointer = cache_find_tile(tile_cache, map_position -> zoom, t_x, t_y);
			gpointer pointer = tile_manager_request_tile(map_area -> tile_manager, t_x, t_y, map_position -> zoom); 
			if (pointer != NULL){
				in_cache = TRUE;
				pixmap = GDK_PIXMAP(pointer);
			}
			if (!in_cache){
				printf("CACHE MISS\n");
				if (no_pic == NULL){
					no_pic = gdk_pixmap_new(NULL,
						256, 256, 24);
					GdkVisual * visual = gdk_visual_get_best_with_depth(24);
					GdkColormap * colormap = gdk_colormap_new(visual, TRUE);
					gdk_drawable_set_colormap(no_pic, colormap);
					gdk_draw_rectangle(no_pic,
						widget->style->white_gc,
						TRUE, 0, 0, TILESIZE, TILESIZE);
				}
				pixmap = no_pic;
			}
			gdk_draw_drawable(map_area -> pixmap, widget->style->black_gc, pixmap,
				0, 0,
				TILESIZE * x - map_position -> tile_offset_x,
				TILESIZE * y - map_position -> tile_offset_y,
				TILESIZE, TILESIZE);
			// if DRAW_GRID
			if (map_area -> show_grid){
				gdk_draw_rectangle(map_area -> pixmap, widget->style->black_gc,
					FALSE,
					TILESIZE * x - map_position -> tile_offset_x,
					TILESIZE * y - map_position -> tile_offset_y,
					256, 256);
			}
			// if DRAW_TILE_NAMES
			if (map_area -> show_font){
				char text[20];
				sprintf(text, "%d_%d_%d", map_position -> zoom, t_x, t_y);
				GdkFont *font = gdk_font_load("-*-terminal-*-*-*-*-14-*-*-*-*-*-*-*");
				gdk_draw_text(map_area -> pixmap, font, widget->style->black_gc,
					TILESIZE * x - map_position -> tile_offset_x + 10,
					TILESIZE * y - map_position -> tile_offset_y + 20,
					text, strlen(text));
			}
		  } // for y
		} // for x
	} /* endif recreate */
	//gettimeofday(&t1, NULL);
	gdk_draw_drawable(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		map_area -> pixmap,
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
	//gettimeofday(&t2, NULL);
	// DRAW SELECTION
	if (map_area -> show_selection){
		// fill
		cairo_t * cr_sel = gdk_cairo_create(widget->window);
		cairo_pattern_t * pat_sel = cairo_pattern_create_rgba(1.0, 1.0, 0.5, 0.4);
		cairo_set_source(cr_sel, pat_sel);
		cairo_rectangle(cr_sel,
			map_area -> selection.x1,
			map_area -> selection.y1, 
			map_area -> selection.x2 - map_area -> selection.x1,
			map_area -> selection.y2 - map_area -> selection.y1);
		cairo_fill(cr_sel);
		// outline
		pat_sel = cairo_pattern_create_rgba(1.0, 1.0, 0.75, 1.0);
		cairo_set_source(cr_sel, pat_sel);
		cairo_rectangle(cr_sel,
			map_area -> selection.x1,
			map_area -> selection.y1, 
			map_area -> selection.x2 - map_area -> selection.x1,
			map_area -> selection.y2 - map_area -> selection.y1);
		cairo_set_line_width(cr_sel, 1.0);
		cairo_stroke(cr_sel);
	}
	// DRAW PATH
	int last_x, last_y; gboolean first = TRUE;
	GList * node = map_area -> path -> next;
	GdkGC *gc = gdk_gc_new(widget->window);
	cairo_t * cr_lines = gdk_cairo_create(widget->window);
	cairo_set_line_width(cr_lines, 1.5);
	cairo_t * cr_arcs = gdk_cairo_create(widget->window);
	while(node != NULL){
		LonLatPair * ll = (LonLatPair*) node -> data;
		int x = map_area_lon_to_area_x(map_area, ll -> lon);
		int y = map_area_lat_to_area_y(map_area, ll -> lat);
		if (first){
			first = FALSE;
			cairo_move_to(cr_lines, x, y);
		}else{
			cairo_line_to(cr_lines, x, y);
		}
		cairo_move_to(cr_arcs, x, y);
		cairo_arc(cr_arcs, x, y, 3, 0.0, 2 * 3.1415927);
		node = node -> next;
	}
	cairo_pattern_t * pat_lines = cairo_pattern_create_rgb(0.55, 0.15, 0.15);
	cairo_set_source(cr_lines, pat_lines);
	cairo_stroke(cr_lines);
	cairo_pattern_t * pat_arcs = cairo_pattern_create_rgb(0.85, 0.25, 0.25);
	cairo_set_source(cr_arcs, pat_arcs);
	cairo_fill(cr_arcs);
	//gettimeofday(&t2, NULL);
	//printf("time %d\n", time_diff(&t1, &t2));
	return TRUE;
}

static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event)
{
	//printf("%d\n", event->keyval); //6536x
	switch (event->keyval){
		case 65361: move(widget, DIRECTION_LEFT); break;
		case 65362: move(widget, DIRECTION_TOP); break;
		case 65363: move(widget, DIRECTION_RIGHT); break;
		case 65364: move(widget, DIRECTION_DOWN); break;
		case 65365: map_area_zoom_in(GOSM_MAP_AREA(widget)); break;
		case 65366: map_area_zoom_out(GOSM_MAP_AREA(widget)); break;
	}
	return FALSE;
}


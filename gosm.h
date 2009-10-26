#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include "tiles/tilesets.h"
#include "poi/poi_set.h"
#include "poi/poi_manager.h"
#include "poi/poi_selector.h"
#include "config/config_widget.h"

gboolean	check_for_cache_directory(char * fn);
static gboolean apply_new_config(Config * config, gpointer data);
static gboolean close_cb(GtkWidget *widget);
void		area_set_cursor(int id);
static gboolean action_select_cb(GtkWidget *widget, gpointer cursor_id_p);
static gboolean button_font_cb(GtkWidget *widget);
static gboolean button_grid_cb(GtkWidget *widget);
static gboolean button_map_controls_cb(GtkWidget *widget);
       void 	toggle_side_bar();
       void 	toggle_menu_bar();
       void 	toggle_tool_bar();
       void 	toggle_status_bar();
       void 	toggle_map_controls();
       void 	toggle_side_bar();
       void 	toggle_side_bar_left();
static void 	button_savemaptype_cb();
static gboolean button_side_bar_cb(GtkWidget *widget);
static gboolean button_side_bar_left_cb(GtkWidget *widget);
static gboolean button_zoom_cb(GtkWidget *widget, gpointer direction);
static gboolean button_move_cb(GtkWidget *widget, gpointer direction);
static gboolean combo_tiles_cb(GtkWidget *widget);
static gboolean menubar_fullscreen_cb(GtkWidget *widget);
static gboolean window_event_cb(GtkWidget *window, GdkEventWindowState *event);
static gboolean map_area_map_cb(GtkWidget *widget, GdkEventConfigure *event);
static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
static gboolean selection_download_cb(GtkWidget *widget);
static gboolean selection_trash_cb(GtkWidget *widget);
static gboolean selection_trash_adv_cb(GtkWidget *widget);
static gboolean selection_export_cb(GtkWidget *widget);
static gboolean selection_xml_cb(GtkWidget *widget);
static gboolean selection_clipboard_cb(GtkWidget *widget);
static gboolean selection_check_snap_cb(GtkWidget *widget);
static gboolean selection_check_visible_cb(GtkWidget *widget);
static gboolean atlas_values_changed_cb(GtkWidget *widget);
static gboolean atlas_template_cb(GtkWidget *widget);
static gboolean atlas_download_cb(GtkWidget *widget);
static gboolean atlas_export_cb(GtkWidget *widget);
static gboolean atlas_export_pdf_cb(GtkWidget *widget);
static gboolean distance_remove_last_cb(GtkWidget *widget);
static gboolean distance_clear_cb(GtkWidget *widget);
static gboolean map_moved_cb(GtkWidget *widget);
static gboolean map_zoom_cb(GtkWidget *widget);
static gboolean map_tileset_cb(GtkWidget *widget);
static void	set_legend(Tileset tileset, int zoom);
static gboolean map_selection_changed_cb(GtkWidget *widget);
static gboolean map_path_cb(GtkWidget *widget);
static gboolean map_node_cb(GtkWidget *widget, gpointer llt_p);
static gboolean show_manual_cb(GtkWidget *widget);
static gboolean show_about_cb(GtkWidget *widget, gpointer nump);
static void 	set_button_network();
static gboolean button_network_cb(GtkWidget *widget);
static gboolean preferences_confirm_cb(GtkWidget * widget, ConfigWidget * config_widget);
static gboolean preferences_cancel_cb(GtkWidget * widget, GtkWindow *window);
static gboolean show_preferences_cb(GtkWidget *widget);
static gboolean focus_redirect_cb(GtkWidget *widget, GdkEventButton *event);
static gboolean legend_click_cb(GtkWidget *widget, GdkEventButton *event);
static gboolean namefinder_city_cb(GtkWidget *widget);
static gboolean namefinder_country_cb(GtkWidget *widget);
static gboolean poi_manager_layer_cb(PoiSelector * poi_selector, int index);
static gboolean poi_manager_colour_cb(PoiManager * poi_manager, int index);
static gboolean poi_manager_source_cb(PoiManager * poi_manager, int index);
static gboolean poi_manager_parse_start_cb(PoiManager * poi_manager, int index);
static gboolean poi_manager_parse_end_cb(PoiManager * poi_manager, int index);
static gboolean poi_manager_api_start_cb(PoiManager * poi_manager);
static gboolean poi_manager_api_end_cb(PoiManager * poi_manager, int index);
static gboolean exit_cb(GtkWidget *widget);
       void     chdir_to_bin(char * arg0);
       void     add_pois(PoiSet * poi_set, char * key, char * value);
       void	read_osm_file();

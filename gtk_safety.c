#include <glib.h>
#include <glib-object.h>

// run this command in vim to produce the include lines
// :r !grep GOSM_TYPE `find . -name "*.h" | grep -v svn | grep -v misc`| grep get_type | sed -e 's|\./\([^:]*\)[^G]*\([A-Z_]*\).*|\1;\2|' | awk -F ';' '{print "\#include \""$1"\""}'
#include "namefinder/namefinder_countries.h"
#include "namefinder/namefinder_cities.h"
#include "map_navigator.h"
#include "tiles/tile_manager.h"
#include "tiles/tile_loader.h"
#include "manual/manual.h"
#include "poi/poi_source_selector.h"
#include "poi/poi_selector.h"
#include "poi/named_poi_set.h"
#include "poi/osm_reader.h"
#include "poi/poi_source_load_progress.h"
#include "poi/cell_renderer_colour.h"
#include "poi/poi_set.h"
#include "poi/poi_manager.h"
#include "poi/poi_tool.h"
#include "poi/styled_poi_set.h"
#include "atlas/atlas_tool.h"
#include "map_area.h"
#include "wizzard/wizzard_download.h"
#include "wizzard/atlas_template_dialog.h"
#include "wizzard/wizzard_export.h"
#include "wizzard/wizzard_atlas_pdf.h"
#include "wizzard/wizzard_atlas_pdf_window.h"
#include "wizzard/tile_download_window.h"
#include "wizzard/wizzard_atlas_sequence_window.h"
#include "wizzard/wizzard_delete_tiles.h"
#include "wizzard/select_use_window.h"
#include "wizzard/select_export_window.h"
#include "wizzard/wizzard_atlas_sequence.h"
#include "wizzard/wizzard_delete_tiles_window.h"
#include "distance/distance_tool.h"
#include "imageglue/pdf_generator.h"
#include "imageglue/image_glue.h"
#include "select/select_tool.h"
#include "about/about.h"
#include "config/color_button.h"
#include "config/color_box.h"
#include "config/config_widget.h"
// end include lines

void make_gtk_types_safe()
{
	volatile GType dummy;
	// run this command in vim to produce the dummy lines
	// :r !grep GOSM_TYPE `find . -name "*.h" | grep -v svn | grep -v misc`| grep get_type | sed -e 's|\./\([^:]*\)[^G]*\([A-Z_]*\).*|\1;\2|' | awk -F ';' '{print "\tdummy = "$2";"}'
	dummy = GOSM_TYPE_TILE_MANAGER;
	dummy = GOSM_TYPE_MAP_NAVIGATOR;
	dummy = GOSM_TYPE_CONFIG_WIDGET;
	dummy = GOSM_TYPE_MAP_AREA;
	dummy = GOSM_TYPE_ATLAS_TOOL;
	dummy = GOSM_TYPE_DISTANCE_TOOL;
	dummy = GOSM_TYPE_TILE_LOADER;
	dummy = GOSM_TYPE_SELECT_TOOL;
	dummy = GOSM_TYPE_MANUAL_DIALOG;
	dummy = GOSM_TYPE_ABOUT_DIALOG;
	dummy = GOSM_TYPE_COLOR_BOX;
	dummy = GOSM_TYPE_COLOR_BUTTON;
	dummy = GOSM_TYPE_PDF_GENERATOR;
	dummy = GOSM_TYPE_IMAGE_GLUE;
	dummy = GOSM_TYPE_SELECT_USE_WINDOW;
	dummy = GOSM_TYPE_WIZZARD_DOWNLOAD;
	dummy = GOSM_TYPE_ATLAS_TEMPLATE_DIALOG;
	dummy = GOSM_TYPE_WIZZARD_DELETE_TILES;
	dummy = GOSM_TYPE_TILE_DOWNLOAD_WINDOW;
	dummy = GOSM_TYPE_WIZZARD_ATLAS_PDF;
	dummy = GOSM_TYPE_SELECT_EXPORT_WINDOW;
	dummy = GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE;
	dummy = GOSM_TYPE_WIZZARD_DELETE_TILES_WINDOW;
	dummy = GOSM_TYPE_WIZZARD_ATLAS_PDF_WINDOW;
	dummy = GOSM_TYPE_WIZZARD_ATLAS_SEQUENCE_WINDOW;
	dummy = GOSM_TYPE_WIZZARD_EXPORT;
	dummy = GOSM_TYPE_CELL_RENDERER_COLOUR;
	dummy = GOSM_TYPE_POI_SELECTOR;
	dummy = GOSM_TYPE_STYLED_POI_SET;
	dummy = GOSM_TYPE_POI_TOOL;
	dummy = GOSM_TYPE_POI_MANAGER;
	dummy = GOSM_TYPE_OSM_READER;
	dummy = GOSM_TYPE_POI_SOURCE_SELECTOR;
	dummy = GOSM_TYPE_NAMED_POI_SET;
	dummy = GOSM_TYPE_POI_SET;
	dummy = GOSM_TYPE_POI_SOURCE_LOAD_PROGRESS;
	dummy = GOSM_TYPE_NAMEFINDER_CITIES;
	dummy = GOSM_TYPE_NAMEFINDER_COUNTRIES;
	// end dummy lines
}

#include "glib.h"

#include "map_area.h"
#include "map_navigator.h"
#include "poi/poi_tool.h"

void make_gtk_types_safe()
{
	volatile GType dummy;
	dummy = GOSM_TYPE_MAP_AREA;
	dummy = GOSM_TYPE_MAP_NAVIGATOR;
	dummy = GOSM_TYPE_POI_TOOL;
}

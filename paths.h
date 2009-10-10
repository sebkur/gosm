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

#ifndef _PATHS_H_
#define _PATHS_H_ 

//#define BUILD = 0 // LOCAL
//#define BUILD = 1 // DEBIAN

#if BUILD == 0 

#define GOSM_ICON_DIR "icons/"
#define GOSM_MANUAL_DIR "manual/"
#define GOSM_ABOUT_DIR "about/"
#define GOSM_NAMEFINDER_DIR "namefinder/"
#define GOSM_LEGEND_DIR "legend/"
#define GOSM_IMAGEGLUE_DIR "imageglue/"
#define GOSM_POI_DIR "poi/"

#else

#define GOSM_ICON_DIR "/usr/share/gosm/icons/"
#define GOSM_MANUAL_DIR "/usr/share/gosm/manual/"
#define GOSM_ABOUT_DIR "/usr/share/gosm/about/"
#define GOSM_NAMEFINDER_DIR "/usr/share/gosm/namefinder/"
#define GOSM_LEGEND_DIR "/usr/share/gosm/legend/"
#define GOSM_IMAGEGLUE_DIR "/usr/lib/gosm/imageglue/"
#define GOSM_POI_DIR "/usr/share/gosm/poi/"

#endif

#endif 

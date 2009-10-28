#ifndef _TILESETS_H_
#define _TILESETS_H_

/****************************************************************************************************
* the tileset currently available
* TODO: the urls should be encapsulated here?
* TODO: make tileset configurable through config-file for easily adding different tilesets
*		-> tile-numbering etc. should be configurable, too
****************************************************************************************************/
typedef enum {
	TILESET_MAPNIK,
	TILESET_OSMARENDER,
	TILESET_CYCLE,
	TILESET_LAST
} Tileset;

#endif

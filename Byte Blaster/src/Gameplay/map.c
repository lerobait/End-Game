/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/util.h"
#include "map.h"

extern Stage stage;

static void loadMapTiles(void);
static void loadMapData(char *filename);

static AtlasImage *tiles[MAX_TILES];

void initMap(void)
{
	loadMapTiles();

	loadMapData("data/stages/1.map");
}

void drawMap(void)
{
	int x, y, n;

	for (x = 0; x < MAP_WIDTH; x++)
	{
		for (y = 0; y < MAP_HEIGHT; y++)
		{
			n = stage.map[x][y];

			if (n > 0)
			{
				blitAtlasImage(tiles[n], x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, 0, SDL_FLIP_NONE);
			}
		}
	}
}

int isInsideMap(int x, int y)
{
	return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
}

static void loadMapTiles(void)
{
	char filename[MAX_FILENAME_LENGTH];
	int	 i;

	for (i = 1; i < MAX_TILES; i++)
	{
		sprintf(filename, "gfx/tiles/%d.png", i);

		tiles[i] = getAtlasImage(filename, 0);
	}
}

static void loadMapData(char *filename)
{
	int	  x, y;
	char *data, *p;

	data = readFile(filename);

	p = data;

	for (y = 0; y < MAP_HEIGHT; y++)
	{
		for (x = 0; x < MAP_WIDTH; x++)
		{
			stage.map[x][y] = atoi(p);

			do
			{
				p++;
			} while (isdigit(*p));
		}
	}

	free(data);
}

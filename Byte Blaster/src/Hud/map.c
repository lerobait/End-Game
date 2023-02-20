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
	int x, y, n, x1, x2, y1, y2, mx, my;

	x1 = (stage.camera.x % MAP_TILE_SIZE) * -1;
	x2 = x1 + MAP_RENDER_WIDTH * MAP_TILE_SIZE + (x1 == 0 ? 0 : MAP_TILE_SIZE);

	y1 = (stage.camera.y % MAP_TILE_SIZE) * -1;
	y2 = y1 + MAP_RENDER_HEIGHT * MAP_TILE_SIZE + (y1 == 0 ? 0 : MAP_TILE_SIZE);

	mx = stage.camera.x / MAP_TILE_SIZE;
	my = stage.camera.y / MAP_TILE_SIZE;

	for (y = y1; y < y2; y += MAP_TILE_SIZE)
	{
		for (x = x1; x < x2; x += MAP_TILE_SIZE)
		{
			if (isInsideMap(mx, my))
			{
				n = stage.map[mx][my];

				if (n > 0)
				{
					blitAtlasImage(tiles[n], x, y, 0, SDL_FLIP_NONE);
				}
			}

			mx++;
		}

		mx = stage.camera.x / MAP_TILE_SIZE;

		my++;
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

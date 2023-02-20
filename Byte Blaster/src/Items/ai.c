/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/map.h"
#include "ai.h"

extern Stage stage;

static int traceMap(int x, int y);

int canSeePlayer(int x, int y)
{
	return traceMap(x, y);
}

static int traceMap(int x, int y)
{
	int x1, y1, x2, y2, dx, dy, sx, sy, err, e2;

	x1 = x / MAP_TILE_SIZE;
	y1 = y / MAP_TILE_SIZE;
	x2 = (stage.player->x + (stage.player->texture->rect.w / 2)) / MAP_TILE_SIZE;
	y2 = (stage.player->y + (stage.player->texture->rect.h / 2)) / MAP_TILE_SIZE;

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	sx = (x1 < x2) ? 1 : -1;
	sy = (y1 < y2) ? 1 : -1;
	err = dx - dy;

	while (1)
	{
		e2 = 2 * err;

		if (e2 > -dy)
		{
			err -= dy;
			x1 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y1 += sy;
		}

		if (x1 == x2 && y1 == y2)
		{
			return 1;
		}

		if (!isInsideMap(x1, y1) || stage.map[x1][y1] != 0)
		{
			return 0;
		}
	}

	return 0;
}

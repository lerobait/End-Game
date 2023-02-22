/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/map.h"
#include "../game/quadtree.h"
#include "../system/util.h"
#include "ai.h"

extern Stage stage;

static int traceMap(Entity *e);
static int traceEntities(Entity *e);

int canSeePlayer(Entity *e)
{
	return traceMap(e) && traceEntities(e);
}

static int traceMap(Entity *e)
{
	int x, y, sx, sy, ex, ey, x1, y1, x2, y2, mx1, my1, mx2, my2;

	sx = (e->x + (e->texture->rect.w / 2));
	sy = e->y;

	ex = (stage.player->x + (stage.player->texture->rect.w / 2));
	ey = stage.player->y;

	x1 = MIN(sx, ex);
	y1 = MIN(sy, ey);
	x2 = MAX(sx, ex);
	y2 = MAX(sy, ey);

	mx1 = x1 / MAP_TILE_SIZE;
	my1 = y1 / MAP_TILE_SIZE;
	mx2 = x2 / MAP_TILE_SIZE;
	my2 = y2 / MAP_TILE_SIZE;

	for (x = mx1; x <= mx2; x++)
	{
		for (y = my1; y <= my2; y++)
		{
			if (isInsideMap(x, y) && stage.map[x][y] != 0 && lineRectCollision(x1, y1, x2, y2, x * MAP_TILE_SIZE, y * MAP_TILE_SIZE, MAP_TILE_SIZE, MAP_TILE_SIZE))
			{
				return 0;
			}
		}
	}

	return 1;
}

static int traceEntities(Entity *e)
{
	int		i, sx, sy, ex, ey, x1, y1, x2, y2;
	Entity *candidates[MAX_QT_CANDIDATES], *other;

	sx = e->x + (e->texture->rect.w / 2);
	sy = e->y;

	ex = stage.player->x + (stage.player->texture->rect.w / 2);
	ey = stage.player->y;

	x1 = MIN(sx, ex);
	y1 = MIN(sy, ey);
	x2 = MAX(sx, ex);
	y2 = MAX(sy, ey);

	getAllEntsWithin(x1, y1, x2, y2, candidates, e);

	for (i = 0, other = candidates[0]; i < MAX_QT_CANDIDATES && other != NULL; other = candidates[++i])
	{
		if ((other->flags & (EF_SOLID | EF_BLOCKS_LOS)) && lineRectCollision(x1, y1, x2, y2, other->x, other->y, other->texture->rect.w, other->texture->rect.h))
		{
			return 0;
		}
	}

	return 1;
}

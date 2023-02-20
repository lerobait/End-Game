/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/map.h"
#include "../system/draw.h"
#include "../system/util.h"
#include "entities.h"

extern App	 app;
extern Stage stage;

static Entity deadHead, *deadTail;

static void move(Entity *e, double dx, double dy);
static void touchOthers(Entity *e);
static void moveToWorldX(Entity *e, double dx);
static void moveToWorldY(Entity *e, double dy);

void initEntities(void)
{
	memset(&stage.entityHead, 0, sizeof(Entity));
	stage.entityTail = &stage.entityHead;

	memset(&deadHead, 0, sizeof(Entity));
	deadTail = &deadHead;
}

void doEntities(void)
{
	Entity *e, *prev;

	prev = &stage.entityHead;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		e->onGround = 0;

		if (!(e->flags & EF_WEIGHTLESS))
		{
			e->dy = MIN(e->dy + (GRAVITY * app.deltaTime), MAX_FALL_SPEED);
		}

		move(e, e->dx, 0);

		move(e, 0, e->dy);

		e->tick(e);

		if (e->touch != NULL)
		{
			touchOthers(e);
		}

		if (e->dead)
		{
			prev->next = e->next;

			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			deadTail->next = e;
			deadTail = e;
			deadTail->next = NULL;

			e = prev;
		}

		prev = e;
	}
}

static void move(Entity *e, double dx, double dy)
{
	if (dx != 0)
	{
		e->x += (dx * app.deltaTime);

		moveToWorldX(e, dx);
	}

	if (dy != 0)
	{
		e->y += (dy * app.deltaTime);

		moveToWorldY(e, dy);
	}

	e->x = MIN(MAX(e->x, 0), (MAP_WIDTH * MAP_TILE_SIZE) - e->texture->rect.w);
	e->y = MIN(MAX(e->y, -e->texture->rect.h), (MAP_WIDTH * MAP_TILE_SIZE) - e->texture->rect.h);
}

static void moveToWorldX(Entity *e, double dx)
{
	int mx, my, adj;

	mx = dx > 0 ? (e->x + e->texture->rect.w) : e->x;
	mx /= MAP_TILE_SIZE;

	for (my = (e->y / MAP_TILE_SIZE); my <= (e->y + e->texture->rect.h - 1) / MAP_TILE_SIZE; my++)
	{
		if (isInsideMap(mx, my) && stage.map[mx][my] != 0)
		{
			adj = dx > 0 ? -(e->texture->rect.w) : MAP_TILE_SIZE;

			e->x = (mx * MAP_TILE_SIZE) + adj;

			e->dx = 0;

			return;
		}
	}
}

static void moveToWorldY(Entity *e, double dy)
{
	int mx, my, adj;

	my = dy > 0 ? (e->y + e->texture->rect.h) : e->y;
	my /= MAP_TILE_SIZE;

	for (mx = e->x / MAP_TILE_SIZE; mx <= (e->x + e->texture->rect.w - 1) / MAP_TILE_SIZE; mx++)
	{
		if (isInsideMap(mx, my) && stage.map[mx][my] != 0)
		{
			adj = dy > 0 ? -(e->texture->rect.h) : MAP_TILE_SIZE;

			e->y = (my * MAP_TILE_SIZE) + adj;

			e->dy = 0;

			e->onGround = dy > 0;

			return;
		}
	}
}

static void touchOthers(Entity *e)
{
	Entity *other;

	for (other = stage.entityHead.next; other != NULL; other = other->next)
	{
		if (e != other && collisionRects(&e->hitbox, &other->hitbox))
		{
			e->touch(e, other);
		}
	}
}

void drawEntities(void)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		e->draw(e);
	}

	if (app.dev.showHitboxes)
	{
		for (e = stage.entityHead.next; e != NULL; e = e->next)
		{
			drawOutlineRect(e->hitbox.x - stage.camera.x, e->hitbox.y - stage.camera.y, e->hitbox.w, e->hitbox.h, 255, 255, 0, 255);
		}
	}
}

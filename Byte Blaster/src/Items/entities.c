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

static void move(Entity *e);
static void moveToWorldX(Entity *e);
static void moveToWorldY(Entity *e);
static void moveToEntities(Entity *e, double dx, double dy);

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

		move(e);

		e->tick(e);

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

static void move(Entity *e)
{
	if (e->dy != 0)
	{
		e->y += (e->dy * app.deltaTime);

		moveToWorldY(e);

		moveToEntities(e, 0, e->dy);
	}

	if (e->dx != 0)
	{
		e->x += (e->dx * app.deltaTime);

		moveToWorldX(e);

		moveToEntities(e, e->dx, 0);
	}

	e->x = MIN(MAX(e->x, 0), (MAP_WIDTH * MAP_TILE_SIZE) - e->texture->rect.w);
	e->y = MIN(MAX(e->y, -e->texture->rect.h), (MAP_WIDTH * MAP_TILE_SIZE) - e->texture->rect.h);
}

static void moveToWorldX(Entity *e)
{
	int mx, my, adj;

	mx = e->dx > 0 ? (e->x + e->texture->rect.w) : e->x;
	mx /= MAP_TILE_SIZE;

	for (my = (e->y / MAP_TILE_SIZE); my <= (e->y + e->texture->rect.h - 1) / MAP_TILE_SIZE; my++)
	{
		if (isInsideMap(mx, my) && stage.map[mx][my] != 0)
		{
			adj = e->dx > 0 ? -(e->texture->rect.w) : MAP_TILE_SIZE;

			e->x = (mx * MAP_TILE_SIZE) + adj;

			e->dx = 0;

			return;
		}
	}
}

static void moveToWorldY(Entity *e)
{
	int mx, my, adj;

	my = e->dy > 0 ? (e->y + e->texture->rect.h) : e->y;
	my /= MAP_TILE_SIZE;

	for (mx = e->x / MAP_TILE_SIZE; mx <= (e->x + e->texture->rect.w - 1) / MAP_TILE_SIZE; mx++)
	{
		if (isInsideMap(mx, my) && stage.map[mx][my] != 0)
		{
			adj = e->dy > 0 ? -(e->texture->rect.h) : MAP_TILE_SIZE;

			e->y = (my * MAP_TILE_SIZE) + adj;

			e->onGround = e->dy > 0;

			e->dy = 0;

			return;
		}
	}
}

static void moveToEntities(Entity *e, double dx, double dy)
{
	Entity *other;
	int		adj, y;

	y = (dy > 0 && dy < 1) ? 1 : 0;

	for (other = stage.entityHead.next; other != NULL; other = other->next)
	{
		if (other != e && collision(e->x, e->y + y, e->texture->rect.w, e->texture->rect.h, other->x, other->y, other->texture->rect.w, other->texture->rect.h))
		{
			if (other->flags & EF_SOLID)
			{
				if (dy != 0)
				{
					adj = dy > 0 ? -e->texture->rect.h : other->texture->rect.h;

					e->y = other->y + adj;

					if (e->dy > 0)
					{
						e->onGround = 1;
					}

					e->dy = 0;
				}

				if (dx != 0)
				{
					adj = dx > 0 ? -e->texture->rect.w : other->texture->rect.w;

					e->x = other->x + adj;

					e->dx = 0;
				}
			}

			if (e->touch != NULL)
			{
				e->touch(e, other);
			}

			if (other->flags & EF_INTERACTIVE && other->touch != NULL)
			{
				other->touch(other, e);
			}
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

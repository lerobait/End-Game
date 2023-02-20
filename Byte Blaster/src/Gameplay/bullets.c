/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/map.h"
#include "../system/draw.h"
#include "../system/util.h"
#include "bullets.h"

extern App	 app;
extern Stage stage;

static void checkCollisions(Bullet *b);
static void checkWorldCollisions(Bullet *b);
static void checkEntityCollisions(Bullet *b);

void initBullets(void)
{
	memset(&stage.bulletHead, 0, sizeof(Bullet));
	stage.bulletTail = &stage.bulletHead;
}

Bullet *spawnBullet(Entity *owner)
{
	Bullet *b;

	b = malloc(sizeof(Bullet));
	memset(b, 0, sizeof(Bullet));
	stage.bulletTail->next = b;
	stage.bulletTail = b;

	b->owner = owner;

	return b;
}

void doBullets(void)
{
	Bullet *b, *prev;

	prev = &stage.bulletHead;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		b->x += (b->dx * app.deltaTime);
		b->y += (b->dy * app.deltaTime);

		b->hitbox.x = b->x;
		b->hitbox.y = b->y;

		b->life -= app.deltaTime;

		checkCollisions(b);

		if (b->life <= 0)
		{
			prev->next = b->next;

			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			free(b);

			b = prev;
		}

		prev = b;
	}
}

static void checkCollisions(Bullet *b)
{
	checkWorldCollisions(b);

	if (b->life != 0)
	{
		checkEntityCollisions(b);
	}
}

static void checkWorldCollisions(Bullet *b)
{
	int mx, my;

	mx = b->hitbox.x + (b->hitbox.w / 2);
	mx /= MAP_TILE_SIZE;

	my = b->hitbox.y + (b->hitbox.h / 2);
	my /= MAP_TILE_SIZE;

	if (!isInsideMap(mx, my) || stage.map[mx][my] != 0)
	{
		b->life = 0;
	}
}

static void checkEntityCollisions(Bullet *b)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (b->owner != e && e->takeDamage != NULL && collisionRects(&b->hitbox, &e->hitbox))
		{
			e->takeDamage(e, b->damage, b->owner);

			b->life = 0;

			return;
		}
	}
}

void drawBullets(void)
{
	Bullet *b;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		blitAtlasImage(b->texture, b->x, b->y, 0, b->dx > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	}
}

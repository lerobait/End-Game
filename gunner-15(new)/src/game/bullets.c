/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/effects.h"
#include "../game/map.h"
#include "../game/quadtree.h"
#include "../system/draw.h"
#include "../system/sound.h"
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

	mx = b->x + (b->texture->rect.w / 2);
	mx /= MAP_TILE_SIZE;

	my = b->y + (b->texture->rect.h / 2);
	my /= MAP_TILE_SIZE;

	if (!isInsideMap(mx, my))
	{
		b->life = 0;
	}
	else if (stage.map[mx][my] != 0)
	{
		b->life = 0;

		addBulletImpactEffect(b);

		playSound(SND_IMPACT, CH_ANY);
	}
}

static void checkEntityCollisions(Bullet *b)
{
	Entity  *e, *candidates[MAX_QT_CANDIDATES];
	int		 i;
	SDL_Rect r;

	r.x = b->x;
	r.y = b->y;
	r.w = b->texture->rect.w;
	r.h = b->texture->rect.h;

	getAllEntsWithin(b->x, b->y, b->texture->rect.w, b->texture->rect.h, candidates, b->owner);

	for (i = 0, e = candidates[0]; i < MAX_QT_CANDIDATES && e != NULL; e = candidates[++i])
	{
		app.dev.collisionChecks++;

		if (((e->flags & EF_SOLID) || e->takeDamage != NULL) && collisionRects(&r, &e->hitbox))
		{
			if (!e->dead && e->takeDamage != NULL)
			{
				e->takeDamage(e, b->damage, b->owner);
			}

			addBulletImpactEffect(b);

			playSound(SND_IMPACT, CH_ANY);

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
		blitAtlasImage(b->texture, b->x - stage.camera.x, b->y - stage.camera.y, 0, b->dx > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	}
}

void clearBullets(void)
{
	Bullet *b;

	while (stage.bulletHead.next != NULL)
	{
		b = stage.bulletHead.next;

		stage.bulletHead.next = b->next;

		free(b);
	}
}

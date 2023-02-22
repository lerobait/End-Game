/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "powerup.h"

extern App	 app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void healthTouch(Entity *self, Entity *other);
static void spreadGunTouch(Entity *self, Entity *other);

static AtlasImage *healthTexture = NULL;
static AtlasImage *spreadGunTexture = NULL;

void initHealth(Entity *e)
{
	if (healthTexture == NULL)
	{
		healthTexture = getAtlasImage("gfx/sprites/health.png", 1);
	}

	e->texture = healthTexture;

	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->tick = tick;
	e->draw = draw;
	e->touch = healthTouch;
}

void initSpreadGun(Entity *e)
{
	if (spreadGunTexture == NULL)
	{
		spreadGunTexture = getAtlasImage("gfx/sprites/spreadGun.png", 1);
	}

	e->texture = spreadGunTexture;

	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->tick = tick;
	e->draw = draw;
	e->touch = spreadGunTouch;
}

static void tick(Entity *self)
{
	self->hitbox.x = self->x;
	self->hitbox.y = self->y;

	if (self->onGround)
	{
		self->dx *= 1.0 - (0.015 * app.deltaTime);
	}
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
}

static void healthTouch(Entity *self, Entity *other)
{
	Gunner *g;

	if (other == stage.player)
	{
		g = (Gunner *)other->data;

		g->life = MIN(g->life + 1, MAX_PLAYER_LIFE);

		self->dead = 1;

		playSound(SND_POWER_UP, CH_ANY);
	}
}

static void spreadGunTouch(Entity *self, Entity *other)
{
	Gunner *g;

	if (other == stage.player)
	{
		g = (Gunner *)other->data;

		g->weaponType = WPN_SPREAD;

		self->dead = 1;

		playSound(SND_POWER_UP, CH_ANY);
	}
}

/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/effects.h"
#include "../game/entityFactory.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "debris.h"

#define NUM_TEXTURES 4

extern App	 app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);

static AtlasImage *textures[NUM_TEXTURES] = {0};

void addDebris(int x, int y)
{
	Debris *d;
	Entity *e;

	d = malloc(sizeof(Debris));
	memset(d, 0, sizeof(Debris));
	d->life = FPS * 2 + (rand() % (int)FPS * 11);

	if (textures[0] == NULL)
	{
		textures[0] = getAtlasImage("gfx/effects/debris1.png", 1);
		textures[1] = getAtlasImage("gfx/effects/debris2.png", 1);
		textures[2] = getAtlasImage("gfx/effects/debris3.png", 1);
		textures[3] = getAtlasImage("gfx/effects/debris4.png", 1);
	}

	e = spawnEntity();
	e->x = x;
	e->y = y;
	e->dx = (1.0 * (rand() % 800 - rand() % 800)) * 0.01;
	e->dy = -(1000.0 + (rand() % 1000)) * 0.01;
	e->texture = textures[rand() % NUM_TEXTURES];
	e->data = d;
	e->flags = EF_BOUNCES;

	e->tick = tick;
	e->draw = draw;
}

static void tick(Entity *self)
{
	Debris *d;

	d = (Debris *)self->data;

	d->life -= app.deltaTime;

	d->smokeTime -= app.deltaTime;

	if (d->life > FPS && d->smokeTime <= 0)
	{
		addSmokeEffect(self->x + (self->texture->rect.w / 2), self->y + (self->texture->rect.h / 2));

		d->smokeTime = 1 + rand() % 5;
	}

	if (self->onGround)
	{
		self->dx *= 1.0 - (0.015 * app.deltaTime);
	}

	if (d->life <= 0)
	{
		self->dead = 1;
	}
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
}

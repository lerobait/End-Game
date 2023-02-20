/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "oilDrum.h"

#define NUM_TEXTURES 3

extern App	 app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void takeDamage(Entity *self, int damage, Entity *attacker);

static AtlasImage *textures[NUM_TEXTURES] = {NULL};

void initOilDrum(Entity *e)
{
	OilDrum *o;

	o = malloc(sizeof(OilDrum));
	memset(o, 0, sizeof(OilDrum));

	o->life = 12;

	if (textures[0] == NULL)
	{
		textures[0] = getAtlasImage("gfx/sprites/greenOilDrum.png", 1);
		textures[1] = getAtlasImage("gfx/sprites/blueOilDrum.png", 1);
		textures[2] = getAtlasImage("gfx/sprites/greyOilDrum.png", 1);
	}

	e->flags = EF_SOLID;

	e->data = o;
	e->texture = textures[rand() % NUM_TEXTURES];
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;
}

static void tick(Entity *self)
{
	OilDrum *o;

	o = (OilDrum *)self->data;

	o->damageTimer = MAX(o->damageTimer - app.deltaTime, 0);

	self->hitbox.x = self->x;
	self->hitbox.y = self->y;
}

static void draw(Entity *self)
{
	OilDrum *o;
	int		 x, y;

	o = (OilDrum *)self->data;

	x = self->x - stage.camera.x;
	y = self->y - stage.camera.y;

	if (o->damageTimer == 0)
	{
		blitAtlasImage(self->texture, x, y, 0, SDL_FLIP_NONE);
	}
	else
	{
		SDL_SetTextureColorMod(self->texture->texture, 255, 32, 32);
		blitAtlasImage(self->texture, x, y, 0, SDL_FLIP_NONE);
		SDL_SetTextureColorMod(self->texture->texture, 255, 255, 255);
	}
}

static void takeDamage(Entity *self, int damage, Entity *attacker)
{
	OilDrum *o;

	o = (OilDrum *)self->data;

	o->damageTimer = 2;

	o->life -= damage;

	if (o->life <= 0)
	{
		self->dead = 1;
	}
}
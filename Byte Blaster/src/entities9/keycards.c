/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "keycards.h"

extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void touch(Entity *self, Entity *other);

static AtlasImage *redTexture = NULL;

static void initKeycard(Entity *e, int type)
{
	Keycard *k;

	k = malloc(sizeof(Keycard));
	memset(k, 0, sizeof(Keycard));

	k->type = type;

	e->data = k;

	e->tick = tick;
	e->draw = draw;
	e->touch = touch;
}

void initRedKeycard(Entity *e)
{
	initKeycard(e, KEYCARD_RED);

	if (redTexture == NULL)
	{
		redTexture = getAtlasImage("gfx/sprites/redKeycard.png", 1);
	}

	e->texture = redTexture;
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;
}

static void tick(Entity *self)
{
	self->hitbox.x = self->x;
	self->hitbox.y = self->y;
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *self, Entity *other)
{
	Gunner  *g;
	Keycard *k;

	if (other == stage.player)
	{
		g = (Gunner *)other->data;
		k = (Keycard *)self->data;

		g->hasKeycard[k->type] = 1;

		self->dead = 1;
	}
}

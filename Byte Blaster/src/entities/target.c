/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "target.h"

#define BOUNCE_HEIGHT (-MAX_FALL_SPEED * 2)

extern App	 app;
extern Stage stage;

static AtlasImage *redTargetTexture = NULL;
static AtlasImage *blueTargetTexture = NULL;

static void tick(Entity *self);
static void draw(Entity *self);
static void takeDamage(Entity *self, int damage, Entity *attacker);

void initTarget(Entity *e)
{
	Target *t;

	t = malloc(sizeof(Target));
	memset(t, 0, sizeof(Target));

	if (redTargetTexture == NULL)
	{
		redTargetTexture = getAtlasImage("gfx/sprites/redTarget.png", 1);
		blueTargetTexture = getAtlasImage("gfx/sprites/blueTarget.png", 1);
	}

	t->life = 3;

	e->data = t;

	if (rand() % 2 == 0)
	{
		e->texture = blueTargetTexture;
		e->dx = 4;
	}
	else
	{
		e->texture = redTargetTexture;
		e->flags = EF_WEIGHTLESS;
	}

	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;
}

static void tick(Entity *self)
{
	Target *t;

	t = (Target *)self->data;

	t->damageTimer = MAX(t->damageTimer - app.deltaTime, 0);

	if (self->x > SCREEN_WIDTH - (self->texture->rect.w + 25))
	{
		self->x = SCREEN_WIDTH - (self->texture->rect.w + 25);
		self->dx = -4;
	}

	if (self->x < 25)
	{
		self->x = 25;
		self->dx = 4;
	}

	if (self->onGround)
	{
		self->dy = BOUNCE_HEIGHT;
	}

	self->hitbox.x = self->x;
	self->hitbox.y = self->y;
}

static void draw(Entity *self)
{
	Target *t;
	int		x, y;

	t = (Target *)self->data;

	x = self->x - stage.camera.x;
	y = self->y - stage.camera.y;

	if (t->damageTimer == 0)
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
	Target *t;

	t = (Target *)self->data;

	t->damageTimer = 2;

	t->life -= damage;

	if (t->life <= 0)
	{
		self->dead = 1;
	}
}

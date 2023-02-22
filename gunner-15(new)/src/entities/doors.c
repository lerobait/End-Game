/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../entities/debris.h"
#include "../game/effects.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "doors.h"

extern App	 app;
extern Stage stage;

static void tick(Entity *self);
static void touchLockedDoor(Entity *self, Entity *other);
static void takeDamage(Entity *self, int amount, Entity *attacker);
static void touchNormalDoor(Entity *self, Entity *other);
static void draw(Entity *self);

static AtlasImage *normalDoorTexture = NULL;
static AtlasImage *weakDoorTexture = NULL;
static AtlasImage *redDoorTexture = NULL;
static AtlasImage *greenDoorTexture = NULL;
static AtlasImage *blueDoorTexture = NULL;

static void initDoor(Entity *e)
{
	Door *d;

	d = malloc(sizeof(Door));
	memset(d, 0, sizeof(Door));

	e->data = d;
	e->flags = EF_WEIGHTLESS + EF_SOLID + EF_INTERACTIVE;
	e->layer = LAYER_BACKGROUND;

	e->tick = tick;
	e->draw = draw;
}

void initNormalDoor(Entity *e)
{
	initDoor(e);

	if (normalDoorTexture == NULL)
	{
		normalDoorTexture = getAtlasImage("gfx/sprites/normalDoor.png", 1);
	}

	e->texture = normalDoorTexture;
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->touch = touchNormalDoor;
}

void initWeakDoor(Entity *e)
{
	initDoor(e);

	if (weakDoorTexture == NULL)
	{
		weakDoorTexture = getAtlasImage("gfx/sprites/weakDoor.png", 1);
	}

	((Door *)e->data)->life = 16;

	e->texture = weakDoorTexture;
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->takeDamage = takeDamage;
}

void initRedDoor(Entity *e)
{
	initDoor(e);

	if (redDoorTexture == NULL)
	{
		redDoorTexture = getAtlasImage("gfx/sprites/redDoor.png", 1);
	}

	((Door *)e->data)->requiredKeycard = KEYCARD_RED;

	e->texture = redDoorTexture;
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->touch = touchLockedDoor;
}

void initGreenDoor(Entity *e)
{
	initDoor(e);

	if (greenDoorTexture == NULL)
	{
		greenDoorTexture = getAtlasImage("gfx/sprites/greenDoor.png", 1);
	}

	((Door *)e->data)->requiredKeycard = KEYCARD_GREEN;

	e->texture = greenDoorTexture;
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->touch = touchLockedDoor;
}

void initBlueDoor(Entity *e)
{
	initDoor(e);

	if (blueDoorTexture == NULL)
	{
		blueDoorTexture = getAtlasImage("gfx/sprites/blueDoor.png", 1);
	}

	((Door *)e->data)->requiredKeycard = KEYCARD_BLUE;

	e->texture = blueDoorTexture;
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->touch = touchLockedDoor;
}

static void tick(Entity *self)
{
	Door *d;

	d = (Door *)self->data;

	if (d->open && self->y > d->ty)
	{
		self->y = MAX(self->y - (app.deltaTime * 3), d->ty);
	}

	d->damageTimer = MAX(d->damageTimer - app.deltaTime, 0);

	d->thinkTime = MAX(d->thinkTime - app.deltaTime, 0);

	self->hitbox.x = self->x;
	self->hitbox.y = self->y;
}

static void draw(Entity *self)
{
	if (((Door *)self->data)->damageTimer == 0)
	{
		blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
	}
	else
	{
		SDL_SetTextureColorMod(self->texture->texture, 255, 32, 32);
		blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
		SDL_SetTextureColorMod(self->texture->texture, 255, 255, 255);
	}
}

static void touchNormalDoor(Entity *self, Entity *other)
{
	Door *d;

	if (other == stage.player)
	{
		d = (Door *)self->data;

		if (!d->open)
		{
			d->ty = (int)self->y - (self->texture->rect.h - 8);
			d->open = 1;

			playSound(SND_DOOR_OPEN, CH_ANY);
		}
	}
}

static void touchLockedDoor(Entity *self, Entity *other)
{
	Door *d;

	if (other == stage.player)
	{
		d = (Door *)self->data;

		if (!d->open)
		{
			if (((Gunner *)other->data)->hasKeycard[d->requiredKeycard])
			{
				d->ty = (int)self->y - (self->texture->rect.h - 8);
				d->open = 1;

				playSound(SND_DOOR_OPEN, CH_ANY);
			}
			else if (d->thinkTime == 0)
			{
				playSound(SND_DOOR_LOCKED, CH_ANY);
			}

			d->thinkTime = 1 + app.deltaTime;
		}
	}
}

static void takeDamage(Entity *self, int amount, Entity *attacker)
{
	Door *d;
	int	  i, x, y;

	if (attacker == stage.player)
	{
		d = (Door *)self->data;

		d->life -= amount;

		playSound(SND_METAL, CH_ANY);

		if (d->life <= 0)
		{
			for (i = 0; i < 50; i++)
			{
				x = self->x + rand() % self->texture->rect.w;
				y = self->y + rand() % self->texture->rect.h;

				addExplosionEffect(x, y, 64);
			}

			for (i = 0; i < 12; i++)
			{
				addDebris(self->x, self->y);
			}

			self->dead = 1;

			playSound(SND_EXPLOSION, CH_ANY);
		}

		d->damageTimer = 2;
	}
}

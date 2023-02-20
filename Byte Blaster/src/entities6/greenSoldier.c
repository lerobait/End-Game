/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/ai.h"
#include "../game/bullets.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "greenSoldier.h"

#define RELOAD_SPEED 14
#define BULLET_SPEED 12

extern App	 app;
extern Stage stage;

static AtlasImage *standTexture = NULL;
static AtlasImage *bulletTexture;

static void tick(Entity *self);
static void draw(Entity *self);
static void takeDamage(Entity *self, int amount, Entity *attacker);
static void lookForPlayer(Entity *self);
static void fireBullet(Entity *self);

void initGreenSoldier(Entity *e)
{
	EnemySoldier *s;

	s = malloc(sizeof(EnemySoldier));
	memset(s, 0, sizeof(EnemySoldier));

	if (standTexture == NULL)
	{
		standTexture = getAtlasImage("gfx/sprites/greenSoldierStand.png", 1);

		bulletTexture = getAtlasImage("gfx/sprites/enemyBullet.png", 1);
	}

	s->life = 5;
	s->thinkTime = FPS * (1 + rand() % 3);

	e->facing = FACING_RIGHT;
	e->texture = standTexture;
	e->data = s;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;
}

static void tick(Entity *self)
{
	EnemySoldier *s;

	s = (EnemySoldier *)self->data;

	s->damageTimer = MAX(s->damageTimer - app.deltaTime, 0);

	s->reload = MAX(s->reload - app.deltaTime, 0);

	if (s->shotsToFire == 0)
	{
		s->thinkTime = MAX(s->thinkTime - app.deltaTime, 0);

		if (s->thinkTime == 0)
		{
			self->facing = rand() % 2 ? FACING_LEFT : FACING_RIGHT;

			s->thinkTime = FPS * (rand() % 2);

			lookForPlayer(self);
		}
	}
	else if (s->reload == 0)
	{
		s->shotsToFire--;

		fireBullet(self);
	}

	self->hitbox.x = self->x + 16;
	self->hitbox.y = self->y;
	self->hitbox.w = 44;
	self->hitbox.h = self->texture->rect.h;
}

static void lookForPlayer(Entity *self)
{
	int			  distX, distY;
	EnemySoldier *s;

	distX = abs(self->x - stage.player->x);
	distY = abs(self->y - stage.player->y);

	if (distX <= SCREEN_WIDTH / 2 && distY <= self->texture->rect.h)
	{
		if ((self->facing == FACING_LEFT && stage.player->x < self->x) || (self->facing == FACING_RIGHT && stage.player->x > self->x))
		{
			if (canSeePlayer(self->x + (self->texture->rect.w / 2), self->y))
			{
				s = (EnemySoldier *)self->data;

				s->shotsToFire = 2 + rand() % 3;

				s->thinkTime = FPS * 2;
			}
		}
	}
}

static void draw(Entity *self)
{
	EnemySoldier *s;
	int			  x, y;

	s = (EnemySoldier *)self->data;

	x = self->x - stage.camera.x;
	y = self->y - stage.camera.y;

	if (s->damageTimer == 0)
	{
		blitAtlasImage(self->texture, x, y, 0, self->facing == FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	}
	else
	{
		SDL_SetTextureColorMod(self->texture->texture, 255, 32, 32);
		blitAtlasImage(self->texture, x, y, 0, self->facing == FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
		SDL_SetTextureColorMod(self->texture->texture, 255, 255, 255);
	}
}

static void takeDamage(Entity *self, int amount, Entity *attacker)
{
	EnemySoldier *s;

	s = (EnemySoldier *)self->data;

	s->life -= amount;

	s->damageTimer = 4;

	if (attacker == stage.player)
	{
		self->facing = stage.player->x < self->x ? FACING_LEFT : FACING_RIGHT;

		s->thinkTime = FPS / 4;
	}

	if (s->life <= 0)
	{
		self->dead = 1;
	}
}

static void fireBullet(Entity *self)
{
	Bullet *b;

	b = spawnBullet(self);
	b->texture = bulletTexture;
	b->hitbox.w = bulletTexture->rect.w;
	b->hitbox.h = bulletTexture->rect.h;
	b->damage = 1;
	b->life = FPS * 2;

	b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x;
	b->y = self->y + 19;
	b->dx = self->facing == FACING_RIGHT ? BULLET_SPEED : -BULLET_SPEED;

	((EnemySoldier *)self->data)->reload = RELOAD_SPEED;
}

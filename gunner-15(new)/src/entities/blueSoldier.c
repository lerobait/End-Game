/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../entities/debris.h"
#include "../entities/powerup.h"
#include "../game/entityFactory.h"
#include "../game/ai.h"
#include "../game/bullets.h"
#include "../game/effects.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "../system/util.h"
#include "blueSoldier.h"

#define RELOAD_SPEED 18
#define BULLET_SPEED 8

extern App	 app;
extern Stage stage;

static AtlasImage *standTexture = NULL;
static AtlasImage *bulletTexture;

static void tick(Entity *self);
static void touch(Entity *self, Entity *other);
static void draw(Entity *self);
static void takeDamage(Entity *self, int amount, Entity *attacker);
static void lookForPlayer(Entity *self);
static void fireBullet(Entity *self);

void initBlueSoldier(Entity *e)
{
	EnemySoldier *s;

	s = malloc(sizeof(EnemySoldier));
	memset(s, 0, sizeof(EnemySoldier));

	if (standTexture == NULL)
	{
		standTexture = getAtlasImage("gfx/sprites/blueSoldierStand.png", 1);

		bulletTexture = getAtlasImage("gfx/sprites/enemyBullet.png", 1);
	}

	s->life = 3;
	s->thinkTime = FPS * (1 + rand() % 3);

	e->facing = FACING_RIGHT;
	e->flags = EF_BLOCKS_LOS;
	e->texture = standTexture;
	e->data = s;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;
	e->touch = touch;
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

			if (!stage.player->dead)
			{
				lookForPlayer(self);
			}
		}
	}
	else if (s->reload == 0)
	{
		s->shotsToFire--;

		self->facing = (stage.player->x > self->x) ? FACING_RIGHT : FACING_LEFT;

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

	if (distX <= SCREEN_WIDTH / 2 && distY <= SCREEN_HEIGHT / 2)
	{
		if ((self->facing == FACING_LEFT && stage.player->x < self->x) || (self->facing == FACING_RIGHT && stage.player->x > self->x))
		{
			if (canSeePlayer(self))
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
        Entity *a;
	EnemySoldier *s;
	int			  i, x, y;

	if (attacker == stage.player)
	{
		s = (EnemySoldier *)self->data;

		s->life -= amount;

		s->damageTimer = 4;

		self->facing = stage.player->x < self->x ? FACING_LEFT : FACING_RIGHT;

		s->thinkTime = FPS / 4;

		playSound(SND_METAL, CH_ANY);

		if (s->life <= 0)
		{
			for (i = 0; i < 32; i++)
			{
				x = self->x + rand() % self->texture->rect.w;
				y = self->y + rand() % self->texture->rect.h;

				addExplosionEffect(x, y, 96);
			}

			for (i = 0; i < 8; i++)
			{
				x = self->x + (self->texture->rect.w / 2);
				y = self->y + (self->texture->rect.h / 2);

				addDebris(x, y);
			}
			a = spawnEntity();
			a->x = self->x + (self->texture->rect.w / 2);
			a->y = self->y + (self->texture->rect.h / 2);
			a->dx = (1.0 * (rand() % 400 - rand() % 400)) * 0.01;
			a->dy = (1000 + (rand() % 700)) * -0.01;
                        initSpreadGun(a);
                        a->flags |= EF_BOUNCES;

			self->dead = 1;

			playSound(SND_EXPLOSION, CH_ANY);
		}
	}
}

static void touch(Entity *self, Entity *other)
{
	if (other == stage.player)
	{
		other->takeDamage(other, 1, self);
	}
}

static void fireBullet(Entity *self)
{
	Bullet *b;

	b = spawnBullet(self);
	b->texture = bulletTexture;
	b->damage = 1;
	b->life = FPS * 2;

	b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x;
	b->y = self->y + 19;

	calcSlope(stage.player->x + (stage.player->texture->rect.w / 2), stage.player->y, b->x, b->y, &b->dx, &b->dy);

	b->dx *= BULLET_SPEED;
	b->dy *= BULLET_SPEED;

	((EnemySoldier *)self->data)->reload = RELOAD_SPEED;

	playSound(SND_ENEMY_SHOOT, CH_ENEMY);
}

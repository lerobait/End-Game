/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../entities/debris.h"
#include "../entities/powerup.h"
#include "../game/ai.h"
#include "../game/bullets.h"
#include "../game/effects.h"
#include "../game/entityFactory.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "../system/util.h"
#include "drone.h"

#define RELOAD_SPEED 14
#define BULLET_SPEED 12

extern App	 app;
extern Stage stage;

static void tick(Entity *self);
static void touch(Entity *self, Entity *other);
static void draw(Entity *self);
static void takeDamage(Entity *self, int amount, Entity *attacker);
static void lookForPlayer(Entity *self);
static void fireBulletX(Entity *self);
static void fireBulletY(Entity *self);

static AtlasImage *bulletTexture;
static AtlasImage *droneTextures[DRONE_MAX] = {0};

void addDrone(int x, int y)
{
	Drone  *d;
	Entity *e;

	d = malloc(sizeof(Drone));
	memset(d, 0, sizeof(Drone));

	if (droneTextures[DRONE_GREY] == NULL)
	{
		droneTextures[DRONE_GREY] = getAtlasImage("gfx/sprites/greyDrone.png", 1);
		droneTextures[DRONE_RED] = getAtlasImage("gfx/sprites/redDrone.png", 1);
		bulletTexture = getAtlasImage("gfx/sprites/enemyBullet.png", 1);
	}

	d->life = 4;
	d->despawnTimer = FPS * 15;
	d->type = rand() % DRONE_MAX;

	e = spawnEntity();

	e->x = x;
	e->y = y;
	e->flags = EF_BLOCKS_LOS + EF_WEIGHTLESS;
	e->texture = droneTextures[d->type];
	e->data = d;

	e->hitbox.w = 24;
	e->hitbox.h = 21;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;
	e->touch = touch;
}

static void tick(Entity *self)
{
	Drone *d;
	int	   x, y;

	d = (Drone *)self->data;

	d->damageTimer = MAX(d->damageTimer - app.deltaTime, 0);

	d->reload = MAX(d->reload - app.deltaTime, 0);

	d->despawnTimer = MAX(d->despawnTimer - app.deltaTime, 0);

	d->engineTimer = MAX(d->engineTimer - app.deltaTime, 0);

	if (d->shotsToFire == 0)
	{
		d->thinkTime = MAX(d->thinkTime - app.deltaTime, 0);

		if (d->thinkTime == 0)
		{
			d->thinkTime = FPS;

			if (!stage.player->dead)
			{
				lookForPlayer(self);
			}
		}
	}
	else if (d->reload == 0)
	{
		d->shotsToFire--;

		if (d->type == DRONE_GREY)
		{
			fireBulletX(self);
		}
		else
		{
			fireBulletY(self);
		}
	}

	if (d->engineTimer <= 0)
	{
		addDroneEngineEffect(self->x + (self->texture->rect.w / 2), self->y + self->texture->rect.h, self->dy);

		d->engineTimer = 1;
	}

	if (d->despawnTimer <= 0)
	{
		x = self->x - stage.camera.x;
		y = self->y - stage.camera.y;

		if (!collision(x, y, self->texture->rect.w, self->texture->rect.h, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			self->dead = 1;
		}
	}
	else
	{
		d->despawnTimer = 1;
	}

	self->hitbox.x = self->x + 12;
	self->hitbox.y = self->y + 3;
}

static void lookForPlayer(Entity *self)
{
	int	   n, distX, distY;
	Drone *d;

	self->dx = self->dy = 0;

	n = rand() % 5;

	if (n < 2)
	{
		calcSlope(stage.player->x, stage.player->y, self->x, self->y, &self->dx, &self->dy);
	}
	else if (n < 4)
	{
		calcSlope(self->x, self->y, stage.player->x, stage.player->y, &self->dx, &self->dy);
	}

	self->dx *= 1 + (rand() % 3);
	self->dy *= 1 + (rand() % 3);

	distX = abs(self->x - stage.player->x);
	distY = abs(self->y - stage.player->y);

	d = (Drone *)self->data;

	if ((d->type == DRONE_GREY && distX <= SCREEN_WIDTH / 2 && distY <= 256) || (d->type == DRONE_RED && distY <= SCREEN_HEIGHT / 2 && distX <= 256))
	{
		if (canSeePlayer(self))
		{
			d->shotsToFire = 4 + rand() % 5;

			d->thinkTime = FPS * 2;
		}
	}
}

static void draw(Entity *self)
{
	Drone *d;
	int	   x, y;

	d = (Drone *)self->data;

	x = self->x - stage.camera.x;
	y = self->y - stage.camera.y;

	if (d->damageTimer == 0)
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

static void takeDamage(Entity *self, int amount, Entity *attacker)
{
	Drone  *d;
	int		i, x, y;

	if (attacker == stage.player)
	{
		d = (Drone *)self->data;

		d->life -= amount;

		d->damageTimer = 4;

		d->thinkTime = FPS / 4;

		playSound(SND_METAL, CH_ANY);

		if (d->life <= 0)
		{
			for (i = 0; i < 16; i++)
			{
				x = self->x + rand() % self->texture->rect.w;
				y = self->y + rand() % self->texture->rect.h;

				addExplosionEffect(x, y, 64);
			}

			for (i = 0; i < 6; i++)
			{
				x = self->x + (self->texture->rect.w / 2);
				y = self->y + (self->texture->rect.h / 2);

				addDebris(x, y);
			}

			
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

static void fireBulletX(Entity *self)
{
	Bullet *b;

	b = spawnBullet(self);
	b->texture = bulletTexture;
	b->damage = 1;
	b->life = FPS * 2;

	b->x = self->x + (self->texture->rect.w / 2) - (b->texture->rect.w / 2);
	b->y = self->y + (self->texture->rect.h / 2) - (b->texture->rect.h / 2);

	b->dx = self->x < stage.player->x ? BULLET_SPEED : -BULLET_SPEED;

	((Drone *)self->data)->reload = RELOAD_SPEED;

	playSound(SND_ENEMY_SHOOT, CH_ENEMY);
}

static void fireBulletY(Entity *self)
{
	Bullet *b;

	b = spawnBullet(self);
	b->texture = bulletTexture;
	b->damage = 1;
	b->life = FPS * 2;

	b->x = self->x + (self->texture->rect.w / 2) - (b->texture->rect.w / 2);
	b->y = self->y + (self->texture->rect.h / 2) - (b->texture->rect.h / 2);

	b->dy = BULLET_SPEED;

	((Drone *)self->data)->reload = RELOAD_SPEED;

	playSound(SND_ENEMY_SHOOT, CH_ENEMY);
}

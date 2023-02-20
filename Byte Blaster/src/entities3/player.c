/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/bullets.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "player.h"

#define NUM_RUN_TEXTURES 6
#define RUN_SPEED		 6
#define JUMP_SPEED		 -15
#define ANIM_TIME		 6
#define RELOAD_SPEED	 8
#define BULLET_SPEED	 24

extern App	 app;
extern Stage stage;

static void tick(Entity *self);
static void handleMovement(Entity *self);
static void handleShoot(Entity *self);
static void updateHitbox(Entity *self);
static void draw(Entity *self);
static void takeDamage(Entity *self, int amount);

static AtlasImage *standTexture = NULL;
static AtlasImage *aimUpTexture;
static AtlasImage *runTextures[NUM_RUN_TEXTURES];
static AtlasImage *aimUpRightTextures[NUM_RUN_TEXTURES];
static AtlasImage *jumpTexture;
static AtlasImage *duckTexture;
static AtlasImage *bulletTexture;

void initPlayer(Entity *e)
{
	int		i;
	char	filename[MAX_FILENAME_LENGTH];
	Gunner *g;

	if (standTexture == NULL)
	{
		for (i = 0; i < NUM_RUN_TEXTURES; i++)
		{
			sprintf(filename, "gfx/sprites/gunnerRun%02d.png", i + 1);
			runTextures[i] = getAtlasImage(filename, 1);

			sprintf(filename, "gfx/sprites/gunnerAimUpRight%02d.png", i + 1);
			aimUpRightTextures[i] = getAtlasImage(filename, 1);
		}

		standTexture = getAtlasImage("gfx/sprites/gunnerStand.png", 1);

		aimUpTexture = getAtlasImage("gfx/sprites/gunnerAimUp.png", 1);

		jumpTexture = getAtlasImage("gfx/sprites/gunnerJump.png", 1);

		duckTexture = getAtlasImage("gfx/sprites/gunnerDuck.png", 1);

		bulletTexture = getAtlasImage("gfx/sprites/playerBullet.png", 1);
	}

	g = malloc(sizeof(Gunner));
	memset(g, 0, sizeof(Gunner));

	g->animTimer = ANIM_TIME;

	e->x = 150;
	e->y = 700;
	e->facing = FACING_RIGHT;
	e->texture = standTexture;
	e->data = g;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;

	stage.player = e;
}

static void tick(Entity *self)
{
	Gunner *g;

	g = (Gunner *)self->data;

	g->immuneTimer = MAX(g->immuneTimer - app.deltaTime, 0);

	handleMovement(self);

	handleShoot(self);

	updateHitbox(self);
}

static void handleMovement(Entity *self)
{
	Gunner *g;

	g = (Gunner *)self->data;

	self->dx = 0;

	g->aimingUp = 0;

	g->ducking = 0;

	if (app.keyboard[SDL_SCANCODE_A])
	{
		self->dx = -RUN_SPEED;

		self->facing = FACING_LEFT;
	}

	if (app.keyboard[SDL_SCANCODE_D])
	{
		self->dx = RUN_SPEED;

		self->facing = FACING_RIGHT;
	}

	if (app.keyboard[SDL_SCANCODE_I] && self->onGround)
	{
		self->dy = JUMP_SPEED;

		self->onGround = 0;
	}

	if (!self->onGround)
	{
		self->texture = jumpTexture;
	}
	else if (app.keyboard[SDL_SCANCODE_S])
	{
		self->texture = duckTexture;

		self->dx = 0;

		g->ducking = 1;

		g->frame = 0;
	}
	else
	{
		if (app.keyboard[SDL_SCANCODE_W])
		{
			g->aimingUp = 1;
		}

		if (self->dx != 0)
		{
			g->animTimer -= app.deltaTime;

			if (g->animTimer <= 0)
			{
				g->frame = (g->frame + 1) % NUM_RUN_TEXTURES;

				g->animTimer = ANIM_TIME;
			}

			if (g->aimingUp)
			{
				self->texture = aimUpRightTextures[g->frame];
			}
			else
			{
				self->texture = runTextures[g->frame];
			}
		}
		else
		{
			g->frame = 0;

			if (g->aimingUp)
			{
				self->texture = aimUpTexture;
			}
			else
			{
				self->texture = standTexture;
			}
		}
	}
}

static void handleShoot(Entity *self)
{
	Bullet *b;
	Gunner *g;

	g = (Gunner *)self->data;

	g->reload = MAX(g->reload - app.deltaTime, 0);

	if (app.keyboard[SDL_SCANCODE_J] && g->reload == 0)
	{
		b = spawnBullet(self);
		b->texture = bulletTexture;
		b->hitbox.w = bulletTexture->rect.w;
		b->hitbox.h = bulletTexture->rect.h;
		b->damage = 1;
		b->life = FPS * 2;

		if (g->ducking)
		{
			b->y = self->y + self->texture->rect.h - 34;
			b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x - b->texture->rect.w;
			b->dx = self->facing == FACING_RIGHT ? BULLET_SPEED : -BULLET_SPEED;
		}
		else if (g->aimingUp)
		{
			b->y = self->y - b->texture->rect.h;
			b->dy = -BULLET_SPEED;

			if (self->dx == 0)
			{
				b->x = self->x + (self->texture->rect.w / 2) - (b->texture->rect.w / 2);
				b->x += self->facing == FACING_RIGHT ? 12 : -12;
			}
			else
			{
				b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x;
				b->dx = self->facing == FACING_RIGHT ? BULLET_SPEED : -BULLET_SPEED;
			}
		}
		else
		{
			b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x;
			b->y = self->y + 19;
			b->dx = self->facing == FACING_RIGHT ? BULLET_SPEED : -BULLET_SPEED;
		}

		g->reload = RELOAD_SPEED;
	}
}

static void updateHitbox(Entity *self)
{
	Gunner *g;

	g = (Gunner *)self->data;

	self->hitbox.w = 44;

	if (!g->ducking)
	{
		self->hitbox.x = self->x + 16;
		self->hitbox.y = self->y;
		self->hitbox.h = self->texture->rect.h;
	}
	else
	{
		self->hitbox.x = self->x + 12;
		self->hitbox.y = self->y + 34;
		self->hitbox.h = 46;
	}
}

static void draw(Entity *self)
{
	Gunner *g;

	g = (Gunner *)self->data;

	if ((int)g->immuneTimer % 5 == 0)
	{
		blitAtlasImage(self->texture, self->x, self->y, 0, self->facing == FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	}
}

static void takeDamage(Entity *self, int amount)
{
	((Gunner *)self->data)->immuneTimer = FPS;
}

/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "player.h"

#define NUM_RUN_TEXTURES 6
#define RUN_SPEED		 6
#define JUMP_SPEED		 -15
#define ANIM_TIME		 6

extern App app;

static void tick(Entity *self);
static void draw(Entity *self);

static AtlasImage *standTexture;
static AtlasImage *runTextures[NUM_RUN_TEXTURES];
static AtlasImage *jumpTexture;
static int		   wasInit = 0;
static int		   frame;
static double	   animTimer;

void initPlayer(Entity *e)
{
	int	 i;
	char filename[MAX_FILENAME_LENGTH];

	if (!wasInit)
	{
		for (i = 0; i < NUM_RUN_TEXTURES; i++)
		{
			sprintf(filename, "gfx/sprites/gunnerRun%02d.png", i + 1);

			runTextures[i] = getAtlasImage(filename, 1);
		}

		standTexture = getAtlasImage("gfx/sprites/gunnerStand.png", 1);

		jumpTexture = getAtlasImage("gfx/sprites/gunnerJump.png", 1);

		wasInit = 1;
	}

	frame = 0;

	animTimer = ANIM_TIME;

	e->x = 450;
	e->y = 300;
	e->facing = FACING_RIGHT;
	e->texture = standTexture;

	e->tick = tick;
	e->draw = draw;
}

static void tick(Entity *self)
{
	self->dx = 0;

	if (self->y == 350)
	{
		self->dy = 0;
	}

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

	if (app.keyboard[SDL_SCANCODE_I] && self->y == 350)
	{
		self->dy = JUMP_SPEED;
	}

	if (self->dy != 0)
	{
		self->texture = jumpTexture;
	}
	else if (self->dx != 0)
	{
		animTimer -= app.deltaTime;

		if (animTimer <= 0)
		{
			frame = (frame + 1) % NUM_RUN_TEXTURES;

			animTimer = ANIM_TIME;
		}

		self->texture = runTextures[frame];
	}
	else
	{
		frame = 0;

		self->texture = standTexture;
	}
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x, self->y, 0, self->facing == FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

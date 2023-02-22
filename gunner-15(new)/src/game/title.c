/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/options.h"
#include "../game/stage.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "../system/text.h"
#include "../system/widgets.h"
#include "title.h"

#define NUM_RUN_TEXTURES 6
#define ANIM_TIME		 6

extern App app;

static void logic(void);
static void draw(void);
static void drawLogo(void);
static void start(void);
static void setupWidgets(void);

static double	   reveal;
static double	   logoY;
static double	   gunnerAnimTimer;
static int		   gunnerFrame;
static double	   gunnerX;
static AtlasImage *gunnerTexture;
static AtlasImage *logoTextures[2] = {0};
static AtlasImage *runTextures[NUM_RUN_TEXTURES];

void initTitle(void)
{
	int	 i;
	char filename[MAX_FILENAME_LENGTH];

	if (logoTextures[0] == NULL)
	{
		logoTextures[0] = getAtlasImage("gfx/title/sdl2.png", 1);

		logoTextures[1] = getAtlasImage("gfx/title/gunner.png", 1);

		for (i = 0; i < NUM_RUN_TEXTURES; i++)
		{
			sprintf(filename, "gfx/sprites/gunnerRun%02d.png", i + 1);

			runTextures[i] = getAtlasImage(filename, 1);
		}

		setupWidgets();

		reveal = 0;

		loadMusic("music/music for the game.mp3");

		playMusic(1);
	}

	logoY = 275;

	gunnerX = -100;
	gunnerAnimTimer = 0;
	gunnerFrame = 0;

	app.activeWidget = getWidget("start", "title");

	app.delegate.logic = logic;

	app.delegate.draw = draw;
}

static void logic(void)
{
	reveal = MIN(reveal + app.deltaTime * 3, 255);

	if (reveal == 255)
	{
		logoY = MAX(logoY - app.deltaTime * 4, 100);

		if (logoY == 100)
		{
			gunnerX = MIN(SCREEN_WIDTH / 2, gunnerX + 8 * app.deltaTime);

			gunnerAnimTimer -= app.deltaTime;

			if (gunnerAnimTimer <= 0)
			{
				gunnerFrame = (gunnerFrame + 1) % NUM_RUN_TEXTURES;

				gunnerAnimTimer = ANIM_TIME;
			}
		}
	}

	gunnerTexture = runTextures[gunnerFrame];

	doWidgets("title");
}

static void draw(void)
{
	drawLogo();

	blitAtlasImage(gunnerTexture, gunnerX, 430, 1, SDL_FLIP_NONE);

	drawWidgets("title");

	app.fontScale = 0.5;

	drawText("Copyright 2021, Parallel Realities. All Rights Reserved.", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 30, 160, 160, 160, TEXT_ALIGN_CENTER, 0);

	app.fontScale = 1.0;
}

static void drawLogo(void)
{
	int x1, x2;

	x2 = logoTextures[0]->rect.w;
	x1 = (SCREEN_WIDTH - (x2 + logoTextures[1]->rect.w)) / 2;
	x2 += x1;

	SDL_SetTextureAlphaMod(logoTextures[0]->texture, reveal);

	blitAtlasImage(logoTextures[0], x1, logoY, 0, SDL_FLIP_NONE);

	blitAtlasImage(logoTextures[1], x2, logoY, 0, SDL_FLIP_NONE);

	SDL_SetTextureAlphaMod(logoTextures[0]->texture, 255);
}

static void start(void)
{
	reveal = 128;

	initStage();
}

static void postOptions(void)
{
	app.activeWidget = getWidget("options", "title");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void options(void)
{
	initOptions(NULL, drawLogo, postOptions);
}

static void quit(void)
{
	exit(0);
}

static void setupWidgets(void)
{
	Widget *w;

	w = getWidget("start", "title");
	w->x = (SCREEN_WIDTH - w->w) / 2;
	w->action = start;

	w = getWidget("options", "title");
	w->x = (SCREEN_WIDTH - w->w) / 2;
	w->action = options;

	w = getWidget("quit", "title");
	w->x = (SCREEN_WIDTH - w->w) / 2;
	w->action = quit;
}

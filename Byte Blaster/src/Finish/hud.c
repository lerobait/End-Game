/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/text.h"
#include "../system/util.h"
#include "hud.h"

extern App	 app;
extern Stage stage;

static void drawPlayerLife(Gunner *g);
static void drawOilDrums(void);
static void drawOilDrumArrow(void);
static void drawKeycards(Gunner *g);
static void drawTime(void);

static AtlasImage *playerLifeBlock = NULL;
static AtlasImage *playerLifeBlockEmpty;
static AtlasImage *playerLifeBlockWarning;
static AtlasImage *keycard[KEYCARD_MAX];
static AtlasImage *keycardEmpty;
static AtlasImage *gunnerIcon;
static AtlasImage *oilDrumIcon;
static AtlasImage *arrow;
static double	   playerLifeWarningTimer;

void initHud(void)
{
	if (playerLifeBlock == NULL)
	{
		playerLifeBlock = getAtlasImage("gfx/hud/playerLifeBlockFull.png", 1);

		playerLifeBlockEmpty = getAtlasImage("gfx/hud/playerLifeBlockEmpty.png", 1);

		playerLifeBlockWarning = getAtlasImage("gfx/hud/playerLifeBlockWarning.png", 1);

		keycard[KEYCARD_RED] = getAtlasImage("gfx/hud/keycardRed.png", 1);

		keycard[KEYCARD_GREEN] = getAtlasImage("gfx/hud/keycardGreen.png", 1);

		keycard[KEYCARD_BLUE] = getAtlasImage("gfx/hud/keycardBlue.png", 1);

		keycardEmpty = getAtlasImage("gfx/hud/keycardEmpty.png", 1);

		gunnerIcon = getAtlasImage("gfx/hud/gunnerIcon.png", 1);

		oilDrumIcon = getAtlasImage("gfx/hud/oilDrumIcon.png", 1);

		arrow = getAtlasImage("gfx/hud/arrow.png", 1);
	}

	playerLifeWarningTimer = 0;
}

void doHud(void)
{
	playerLifeWarningTimer += app.deltaTime;

	if (playerLifeWarningTimer >= FPS)
	{
		playerLifeWarningTimer = 0;
	}
}

void drawHud(void)
{
	Gunner *g;

	g = (Gunner *)stage.player->data;

	drawRect(0, 0, SCREEN_WIDTH, 40, 48, 48, 48, 192);

	drawPlayerLife(g);

	drawOilDrumArrow();

	drawOilDrums();

	drawTime();

	drawKeycards(g);
}

static void drawPlayerLife(Gunner *g)
{
	int	 x, i;
	char text[8];

	AtlasImage *blockTexture;

	x = 20;

	for (i = 0; i < MAX_PLAYER_LIFE; i++)
	{
		blockTexture = playerLifeBlock;

		if (i >= g->life)
		{
			blockTexture = playerLifeBlockEmpty;
		}
		else if (g->life == 1 && playerLifeWarningTimer < (int)FPS / 2)
		{
			blockTexture = playerLifeBlockWarning;
		}

		blitAtlasImage(blockTexture, x, 5, 0, SDL_FLIP_NONE);

		x += blockTexture->rect.w + 15;
	}

	blitAtlasImage(gunnerIcon, 550, 5, 0, SDL_FLIP_NONE);

	app.fontScale = 0.75;

	sprintf(text, "x %d", g->rest);

	drawText(text, 605, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	app.fontScale = 1.0;
}

static void drawOilDrums(void)
{
	char text[8];

	blitAtlasImage(oilDrumIcon, 850, 5, 0, SDL_FLIP_NONE);

	app.fontScale = 0.75;

	sprintf(text, "x %d", stage.numOilDrums);

	drawText(text, 905, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	app.fontScale = 1.0;
}

static void drawOilDrumArrow(void)
{
	if (stage.nearestOilDrum != NULL)
	{
		blitRotated(arrow, 805, 20, getAngle(stage.player->x, stage.player->y, stage.nearestOilDrum->x, stage.nearestOilDrum->y));
	}
}

static void drawTime(void)
{
	char text[16];
	int	 seconds, minutes;

	seconds = (int)(stage.time / FPS);
	minutes = (seconds / 60);

	app.fontScale = 0.75;

	sprintf(text, "Time: %02d:%02d", minutes, seconds % 60);

	drawText(text, 1105, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	app.fontScale = 1.0;
}

static void drawKeycards(Gunner *g)
{
	int i, x;

	x = SCREEN_WIDTH - (keycardEmpty->rect.w + 15);

	for (i = 0; i < KEYCARD_MAX; i++)
	{
		if (g->hasKeycard[i])
		{
			blitAtlasImage(keycard[i], x, 8, 0, SDL_FLIP_NONE);
		}
		else
		{
			blitAtlasImage(keycardEmpty, x, 8, 0, SDL_FLIP_NONE);
		}

		x -= keycardEmpty->rect.w + 15;
	}
}

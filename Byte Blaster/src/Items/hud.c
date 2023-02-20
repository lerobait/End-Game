/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/text.h"
#include "hud.h"

extern App	 app;
extern Stage stage;

static void drawPlayerLife(Gunner *g);
static void drawKeycards(Gunner *g);
static void drawDebugInfo(void);

static AtlasImage *playerLifeBlock = NULL;
static AtlasImage *playerLifeBlockEmpty;
static AtlasImage *playerLifeBlockWarning;
static AtlasImage *keycard[KEYCARD_MAX];
static AtlasImage *keycardEmpty;
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

	drawPlayerLife(g);

	drawKeycards(g);

	if (0)
	{
		drawDebugInfo();
	}
}

static void drawPlayerLife(Gunner *g)
{
	int x, i;

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

		blitAtlasImage(blockTexture, x, 20, 0, SDL_FLIP_NONE);

		x += blockTexture->rect.w + 15;
	}
}

static void drawKeycards(Gunner *g)
{
	int i, x;

	x = 20;

	for (i = 0; i < KEYCARD_MAX; i++)
	{
		if (g->hasKeycard[i])
		{
			blitAtlasImage(keycard[i], x, 70, 0, SDL_FLIP_NONE);
		}
		else
		{
			blitAtlasImage(keycardEmpty, x, 70, 0, SDL_FLIP_NONE);
		}

		x += keycardEmpty->rect.w + 15;
	}
}

static void drawDebugInfo(void)
{
	char text[64];

	app.fontScale = 0.5;

	sprintf(text, "x=%d, y=%d, mx=%d, my=%d", (int)stage.player->x, (int)stage.player->y, (int)stage.player->x / MAP_TILE_SIZE, (int)stage.player->y / MAP_TILE_SIZE);

	drawText(text, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 30, 255, 255, 255, TEXT_ALIGN_RIGHT, 0);

	app.fontScale = 1.0;
}

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

static void drawPlayerLife(void);
static void drawDebugInfo(void);

static AtlasImage *playerLifeBlock = NULL;
static AtlasImage *playerLifeBlockEmpty;
static AtlasImage *playerLifeBlockWarning;
static double	   playerLifeWarningTimer;

void initHud(void)
{
	if (playerLifeBlock == NULL)
	{
		playerLifeBlock = getAtlasImage("gfx/hud/playerLifeBlockFull.png", 1);

		playerLifeBlockEmpty = getAtlasImage("gfx/hud/playerLifeBlockEmpty.png", 1);

		playerLifeBlockWarning = getAtlasImage("gfx/hud/playerLifeBlockWarning.png", 1);
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
	drawPlayerLife();

	if (0)
	{
		drawDebugInfo();
	}
}

static void drawPlayerLife(void)
{
	int			x, i;
	Gunner	   *g;
	AtlasImage *blockTexture;

	x = 20;

	g = (Gunner *)stage.player->data;

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

static void drawDebugInfo(void)
{
	char text[64];

	app.fontScale = 0.5;

	sprintf(text, "x=%d, y=%d", (int)stage.player->x, (int)stage.player->y);

	drawText(text, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 30, 255, 255, 255, TEXT_ALIGN_RIGHT, 0);

	app.fontScale = 1.0;
}

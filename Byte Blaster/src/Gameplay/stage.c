/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/bullets.h"
#include "../game/entities.h"
#include "../game/entityFactory.h"
#include "../game/map.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "stage.h"

extern App	 app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void drawSky(void);
static void addSoldiers(void);

static AtlasImage *skyTexture;
static AtlasImage *moonTexture;

void initStage(void)
{
	memset(&stage, 0, sizeof(Stage));

	skyTexture = getAtlasImage("gfx/stage/sky.png", 1);

	moonTexture = getAtlasImage("gfx/stage/moon.png", 1);

	initMap();

	initEntities();

	initBullets();

	initEntity("player");

	addSoldiers();

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void addSoldiers(void)
{
	Entity *e;

	e = initEntity("greenSoldier");
	e->x = 1500;
	e->y = 750;

	e = initEntity("greenSoldier");
	e->x = 1400;
	e->y = 350;

	e = initEntity("greenSoldier");
	e->x = 200;
	e->y = 550;
}

static void logic(void)
{
	doEntities();

	doBullets();

	if (app.keyboard[SDL_SCANCODE_F1])
	{
		app.dev.showHitboxes = !app.dev.showHitboxes;

		app.keyboard[SDL_SCANCODE_F1] = 0;
	}
}

static void draw(void)
{
	drawSky();

	drawMap();

	drawEntities();

	drawBullets();
}

static void drawSky(void)
{
	int x;

	x = 0;

	while (x < SCREEN_WIDTH)
	{
		blitAtlasImage(skyTexture, x, 0, 0, SDL_FLIP_NONE);

		x += skyTexture->rect.w;
	}

	blitAtlasImage(moonTexture, SCREEN_WIDTH - 160, 20, 0, SDL_FLIP_NONE);
}

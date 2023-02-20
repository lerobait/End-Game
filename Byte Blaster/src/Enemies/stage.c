/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/bullets.h"
#include "../game/entities.h"
#include "../game/entityFactory.h"
#include "../system/draw.h"
#include "stage.h"

extern App	 app;
extern Stage stage;

static void logic(void);
static void draw(void);

void initStage(void)
{
	memset(&stage, 0, sizeof(Stage));

	initEntities();

	initBullets();

	initEntity("player");

	initEntity("greenSoldier");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
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
	drawRect(0, 430, SCREEN_WIDTH, 64, 0, 128, 0, 255);

	drawEntities();

	drawBullets();
}

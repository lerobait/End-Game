/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

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

	initEntity("player");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	doEntities();
}

static void draw(void)
{
	drawRect(0, 430, SCREEN_WIDTH, 64, 0, 128, 0, 255);

	drawEntities();
}

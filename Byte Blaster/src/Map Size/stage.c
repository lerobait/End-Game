/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../game/bullets.h"
#include "../game/entities.h"
#include "../game/entityFactory.h"
#include "../game/map.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/text.h"
#include "../system/util.h"
#include "stage.h"

#define CAMERA_MID_X 36
#define CAMERA_MID_Y 40

extern App	 app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void drawSky(void);
static void updateCamera(void);
static void loadStage(char *filename);
static void drawDebugInfo(void);

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

	loadStage("data/stages/1.json");

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

	updateCamera();
}

static void updateCamera(void)
{
	stage.camera.x = (int)stage.player->x + CAMERA_MID_X;
	stage.camera.y = (int)stage.player->y + CAMERA_MID_Y;

	stage.camera.x -= (SCREEN_WIDTH / 2);
	stage.camera.y -= (SCREEN_HEIGHT / 2);

	stage.camera.x = MIN(MAX(stage.camera.x, 0), (MAP_WIDTH * MAP_TILE_SIZE) - SCREEN_WIDTH);
	stage.camera.y = MIN(MAX(stage.camera.y, 0), (MAP_HEIGHT * MAP_TILE_SIZE) - SCREEN_HEIGHT);
}

static void draw(void)
{
	drawSky();

	drawMap();

	drawEntities();

	drawBullets();

	if (0)
	{
		drawDebugInfo();
	}
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

static void loadStage(char *filename)
{
	cJSON  *root, *node;
	char	 *text, *type;
	int		x, y;
	Entity *e;

	text = readFile(filename);

	root = cJSON_Parse(text);

	for (node = root->child; node != NULL; node = node->next)
	{
		type = cJSON_GetObjectItem(node, "type")->valuestring;
		x = cJSON_GetObjectItem(node, "x")->valueint;
		y = cJSON_GetObjectItem(node, "y")->valueint;

		e = initEntity(type);
		e->x = x;
		e->y = y;
	}

	cJSON_Delete(root);

	free(text);
}

static void drawDebugInfo(void)
{
	char text[64];

	sprintf(text, "x=%d, y=%d", (int)stage.player->x, (int)stage.player->y);

	drawText(text, 25, 25, 255, 255, 255, TEXT_ALIGN_LEFT, 0);
}

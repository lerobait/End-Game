/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../entities/drone.h"
#include "../entities/player.h"
#include "../game/bullets.h"
#include "../game/effects.h"
#include "../game/entities.h"
#include "../game/entityFactory.h"
#include "../game/hud.h"
#include "../game/map.h"
#include "../game/quadtree.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/util.h"
#include "stage.h"

#define CAMERA_MID_X	   36
#define CAMERA_MID_Y	   40
#define RESPAWN_TIMER_TIME (FPS * 3)
#define DRONE_SPAWN_TIME   (FPS * 15)

extern App	 app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void drawSky(void);
static void updateCamera(void);
static void loadStage(char *filename);
static void spawnDrone(void);
static void resetStage(void);

static AtlasImage *skyTexture;
static AtlasImage *moonTexture;
static double	   playerRespawnTimer;
static double	   droneSpawnTimer;

void initStage(void)
{
	memset(&stage, 0, sizeof(Stage));

	skyTexture = getAtlasImage("gfx/stage/sky.png", 1);

	moonTexture = getAtlasImage("gfx/stage/moon.png", 1);

	initQuadtree(&stage.quadtree);

	initMap();

	initEntities();

	initBullets();

	initHud();

	initEffects();

	loadStage("data/stages/1.json");

	playerRespawnTimer = RESPAWN_TIMER_TIME;

	droneSpawnTimer = DRONE_SPAWN_TIME;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	app.dev.collisionChecks = 0;

	if (stage.player->dead)
	{
		playerRespawnTimer -= app.deltaTime;

		if (playerRespawnTimer <= 0)
		{
			if (resetPlayer())
			{
				playerRespawnTimer = RESPAWN_TIMER_TIME;
			}
			else
			{
				resetStage();

				initStage();
			}
		}
	}

	spawnDrone();

	doEntities();

	doBullets();

	doEffects();

	doHud();

	updateCamera();

	if (app.keyboard[SDL_SCANCODE_F1])
	{
		app.dev.showHitboxes = !app.dev.showHitboxes;

		app.keyboard[SDL_SCANCODE_F1] = 0;
	}
}

static void spawnDrone(void)
{
	int x, y, mx, my;

	droneSpawnTimer -= app.deltaTime;

	if (droneSpawnTimer <= 0)
	{
		droneSpawnTimer = DRONE_SPAWN_TIME;

		x = stage.camera.x + (rand() % SCREEN_WIDTH);
		y = stage.camera.y - 64;

		mx = x / MAP_TILE_SIZE;
		my = y / MAP_TILE_SIZE;

		if (!isInsideMap(mx, my) || stage.map[mx][my] == 0)
		{
			addDrone(x, y);
		}
	}
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
	if (playerRespawnTimer > FPS / 2)
	{
		drawSky();

		drawEntities(LAYER_BACKGROUND);

		drawMap();

		drawEntities(LAYER_FOREGROUND);

		drawBullets();

		drawEffects();

		drawHud();
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

static void resetStage(void)
{
	destroyQuadtree(&stage.quadtree);

	clearEntities();

	clearBullets();

	clearEffects();

	free(stage.player->data);

	free(stage.player);
}

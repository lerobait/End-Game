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
#include "../game/options.h"
#include "../game/quadtree.h"
#include "../game/title.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/text.h"
#include "../system/util.h"
#include "../system/widgets.h"
#include "stage.h"

#define CAMERA_MID_X	   36
#define CAMERA_MID_Y	   40
#define RESPAWN_TIMER_TIME (FPS * 3)
#define DRONE_SPAWN_TIME   (FPS * 10)
#define BANNER_MID_Y	   ((SCREEN_HEIGHT / 2) - 50)

extern App	 app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void updateCamera(void);
static void loadStage(char *filename);
static void spawnDrone(void);
static void resetStage(void);
static void drawStage(void);
static void setupWidgets(void);
static void doPause(void);
static void doStage(void);
static void drawBanner(void);
static void doBanner(void);

static AtlasImage *missionTexture;
static AtlasImage *startTexture;
static AtlasImage *failedTexture;
static AtlasImage *completeTexture;
static double	   playerRespawnTimer;
static double	   droneSpawnTimer;
static int		   showBanner;
static double	   bannerTimer;
static double	   bannerY;
static int		   paused;
static int		   wasInit = 0;

void initStage(void)
{
	memset(&stage, 0, sizeof(Stage));

	if (!wasInit)
	{
		wasInit = 1;

		missionTexture = getAtlasImage("gfx/stage/mission.png", 1);

		startTexture = getAtlasImage("gfx/stage/start.png", 1);

		failedTexture = getAtlasImage("gfx/stage/failed.png", 1);

		completeTexture = getAtlasImage("gfx/stage/complete.png", 1);

		setupWidgets();
	}

	initQuadtree(&stage.quadtree);

	initMap();

	initEntities();

	initBullets();

	initHud();

	initEffects();

	loadStage("data/stages/1.json");

	updateStageStatus(MISSION_INCOMPLETE);

	playerRespawnTimer = RESPAWN_TIMER_TIME;

	droneSpawnTimer = DRONE_SPAWN_TIME;

	paused = 0;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (!paused)
	{
		doStage();
	}
	else
	{
		doPause();
	}
}

static void doStage(void)
{
	app.dev.collisionChecks = 0;

	if (stage.player->dead && !showBanner)
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

	stage.nearestOilDrum = NULL;

	doBanner();

	spawnDrone();

	doEntities();

	doBullets();

	doEffects();

	doHud();

	updateCamera();

	if (stage.status == MISSION_INCOMPLETE)
	{
		stage.time += app.deltaTime;
	}

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		app.activeWidget = getWidget("resume", "pause");

		paused = 1;
	}

	if (app.keyboard[SDL_SCANCODE_F1])
	{
		app.dev.showHitboxes = !app.dev.showHitboxes;

		app.keyboard[SDL_SCANCODE_F1] = 0;
	}
}

static void doBanner(void)
{
	if (bannerY < SCREEN_HEIGHT)
	{
		bannerY += app.deltaTime * 15;

		if (bannerTimer > 0 && bannerY > BANNER_MID_Y)
		{
			bannerTimer -= app.deltaTime;

			bannerY = BANNER_MID_Y;
		}
	}

	showBanner = bannerY < SCREEN_HEIGHT;
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

static void doPause(void)
{
	doWidgets("pause");

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		paused = 0;
	}
}

static void draw(void)
{
	if (playerRespawnTimer > FPS / 2)
	{
		drawStage();
	}

	if (!paused)
	{
		drawBanner();

		drawHud();
	}
	else
	{
		drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 160);

		app.fontScale = 1.5;

		drawText("PAUSE", SCREEN_WIDTH / 2, 70, 255, 255, 255, TEXT_ALIGN_CENTER, 0);

		app.fontScale = 1.0;

		drawWidgets("pause");
	}
}

static void drawStage(void)
{

	drawEntities(LAYER_BACKGROUND);

	drawMap();

	drawEntities(LAYER_FOREGROUND);

	drawBullets();

	drawEffects();
}

static void drawBanner(void)
{
	int			x1, x2;
	AtlasImage *statusTexture;

	if (showBanner)
	{
		switch (stage.status)
		{
			case MISSION_INCOMPLETE:
				statusTexture = startTexture;
				break;

			case MISSION_COMPLETE:
				statusTexture = completeTexture;
				break;

			default:
				statusTexture = failedTexture;
				break;
		}

		x2 = missionTexture->rect.w + 40;
		x1 = (SCREEN_WIDTH - (x2 + statusTexture->rect.w)) / 2;
		x2 += x1;

		blitAtlasImage(missionTexture, x1, bannerY, 0, SDL_FLIP_NONE);

		blitAtlasImage(statusTexture, x2, bannerY, 0, SDL_FLIP_NONE);
	}
}

static void resume(void)
{
	paused = 0;
}

static void postOptions(void)
{
	app.activeWidget = getWidget("options", "pause");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void options(void)
{
	initOptions(NULL, drawStage, postOptions);
}

static void quit(void)
{
	resetStage();

	initTitle();
}

static void setupWidgets(void)
{
	Widget *w;

	w = getWidget("resume", "pause");
	w->x = (SCREEN_WIDTH - w->w) / 2;
	w->action = resume;

	w = getWidget("options", "pause");
	w->x = (SCREEN_WIDTH - w->w) / 2;
	w->action = options;

	w = getWidget("quit", "pause");
	w->x = (SCREEN_WIDTH - w->w) / 2;
	w->action = quit;
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

void updateStageStatus(int status)
{
	if (stage.status == MISSION_INCOMPLETE)
	{
		stage.status = status;

		switch (status)
		{
			case MISSION_INCOMPLETE:
				bannerY = -100;
				bannerTimer = FPS;
				break;

			case MISSION_FAILED:
				bannerY = -1000;
				bannerTimer = FPS;
				break;

			default:
				bannerY = -100;
				bannerTimer = FPS * 2;
				break;
		}

		showBanner = 1;
	}
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

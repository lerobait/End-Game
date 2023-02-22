/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include <SDL2/SDL_mixer.h>

#include "../common.h"

#include "../json/cJSON.h"
#include "../system/sound.h"
#include "../system/util.h"
#include "game.h"

#define SAVE_GAME_FILENAME "save.json"

extern Game game;

static void loadGame(void);

void initGame(void)
{
	game.soundVolume = MIX_MAX_VOLUME;

	game.musicVolume = MIX_MAX_VOLUME;

	loadGame();

	setSoundVolume(game.soundVolume);

	setMusicVolume(game.musicVolume);
}

static void loadGame(void)
{
	char	 *data;
	cJSON *root, *controls, *node;

	data = readFile(SAVE_GAME_FILENAME);

	if (data != NULL)
	{
		root = cJSON_Parse(data);

		node = cJSON_GetObjectItem(root, "volumes");
		game.soundVolume = MAX(MIN(cJSON_GetObjectItem(node, "sound")->valueint, MIX_MAX_VOLUME), 0);
		game.musicVolume = MAX(MIN(cJSON_GetObjectItem(node, "music")->valueint, MIX_MAX_VOLUME), 0);

		cJSON_Delete(root);

		free(data);
	}
}

void saveGame(void)
{
	cJSON *root, *volumes;
	char	 *out;

	volumes = cJSON_CreateObject();
	cJSON_AddNumberToObject(volumes, "sound", game.soundVolume);
	cJSON_AddNumberToObject(volumes, "music", game.musicVolume);

	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "controls", controls);
	cJSON_AddItemToObject(root, "volumes", volumes);

	out = cJSON_Print(root);

	writeFile(SAVE_GAME_FILENAME, out);

	cJSON_Delete(root);

	free(out);
}

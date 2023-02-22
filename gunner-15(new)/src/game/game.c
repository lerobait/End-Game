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

	game.keyControls[CONTROL_LEFT] = SDL_SCANCODE_A;

	game.keyControls[CONTROL_RIGHT] = SDL_SCANCODE_D;

	game.keyControls[CONTROL_UP] = SDL_SCANCODE_W;

	game.keyControls[CONTROL_DOWN] = SDL_SCANCODE_S;

	game.keyControls[CONTROL_JUMP] = SDL_SCANCODE_I;

	game.keyControls[CONTROL_FIRE] = SDL_SCANCODE_J;

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

		controls = cJSON_GetObjectItem(root, "controls");

		node = cJSON_GetObjectItem(controls, "keyboard");

		game.keyControls[CONTROL_LEFT] = cJSON_GetObjectItem(node, "left")->valueint;
		game.keyControls[CONTROL_RIGHT] = cJSON_GetObjectItem(node, "right")->valueint;
		game.keyControls[CONTROL_UP] = cJSON_GetObjectItem(node, "up")->valueint;
		game.keyControls[CONTROL_DOWN] = cJSON_GetObjectItem(node, "down")->valueint;
		game.keyControls[CONTROL_JUMP] = cJSON_GetObjectItem(node, "jump")->valueint;
		game.keyControls[CONTROL_FIRE] = cJSON_GetObjectItem(node, "fire")->valueint;

		node = cJSON_GetObjectItem(root, "volumes");
		game.soundVolume = MAX(MIN(cJSON_GetObjectItem(node, "sound")->valueint, MIX_MAX_VOLUME), 0);
		game.musicVolume = MAX(MIN(cJSON_GetObjectItem(node, "music")->valueint, MIX_MAX_VOLUME), 0);

		cJSON_Delete(root);

		free(data);
	}
}

void saveGame(void)
{
	cJSON *root, *controls, *keyboard, *volumes;
	char	 *out;

	keyboard = cJSON_CreateObject();
	cJSON_AddNumberToObject(keyboard, "left", game.keyControls[CONTROL_LEFT]);
	cJSON_AddNumberToObject(keyboard, "right", game.keyControls[CONTROL_RIGHT]);
	cJSON_AddNumberToObject(keyboard, "up", game.keyControls[CONTROL_UP]);
	cJSON_AddNumberToObject(keyboard, "down", game.keyControls[CONTROL_DOWN]);
	cJSON_AddNumberToObject(keyboard, "jump", game.keyControls[CONTROL_JUMP]);
	cJSON_AddNumberToObject(keyboard, "fire", game.keyControls[CONTROL_FIRE]);

	volumes = cJSON_CreateObject();
	cJSON_AddNumberToObject(volumes, "sound", game.soundVolume);
	cJSON_AddNumberToObject(volumes, "music", game.musicVolume);

	controls = cJSON_CreateObject();
	cJSON_AddItemToObject(controls, "keyboard", keyboard);

	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "controls", controls);
	cJSON_AddItemToObject(root, "volumes", volumes);

	out = cJSON_Print(root);

	writeFile(SAVE_GAME_FILENAME, out);

	cJSON_Delete(root);

	free(out);
}

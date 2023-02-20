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
	game.joypadControls[CONTROL_LEFT] = -1;

	game.keyControls[CONTROL_RIGHT] = SDL_SCANCODE_D;
	game.joypadControls[CONTROL_RIGHT] = -1;

	game.keyControls[CONTROL_UP] = SDL_SCANCODE_W;
	game.joypadControls[CONTROL_UP] = -1;

	game.keyControls[CONTROL_DOWN] = SDL_SCANCODE_S;
	game.joypadControls[CONTROL_DOWN] = -1;

	game.keyControls[CONTROL_JUMP] = SDL_SCANCODE_I;
	game.joypadControls[CONTROL_JUMP] = -1;

	game.keyControls[CONTROL_FIRE] = SDL_SCANCODE_J;
	game.joypadControls[CONTROL_FIRE] = -1;

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

		node = cJSON_GetObjectItem(controls, "joypad");

		game.joypadControls[CONTROL_LEFT] = cJSON_GetObjectItem(node, "left")->valueint;
		game.joypadControls[CONTROL_RIGHT] = cJSON_GetObjectItem(node, "right")->valueint;
		game.joypadControls[CONTROL_UP] = cJSON_GetObjectItem(node, "up")->valueint;
		game.joypadControls[CONTROL_DOWN] = cJSON_GetObjectItem(node, "down")->valueint;
		game.joypadControls[CONTROL_JUMP] = cJSON_GetObjectItem(node, "jump")->valueint;
		game.joypadControls[CONTROL_FIRE] = cJSON_GetObjectItem(node, "fire")->valueint;
		game.deadzone = cJSON_GetObjectItem(node, "deadzone")->valueint;

		node = cJSON_GetObjectItem(root, "volumes");
		game.soundVolume = MAX(MIN(cJSON_GetObjectItem(node, "sound")->valueint, MIX_MAX_VOLUME), 0);
		game.musicVolume = MAX(MIN(cJSON_GetObjectItem(node, "music")->valueint, MIX_MAX_VOLUME), 0);

		cJSON_Delete(root);

		free(data);
	}
}

void saveGame(void)
{
	cJSON *root, *controls, *keyboard, *joypad, *volumes;
	char	 *out;

	keyboard = cJSON_CreateObject();
	cJSON_AddNumberToObject(keyboard, "left", game.keyControls[CONTROL_LEFT]);
	cJSON_AddNumberToObject(keyboard, "right", game.keyControls[CONTROL_RIGHT]);
	cJSON_AddNumberToObject(keyboard, "up", game.keyControls[CONTROL_UP]);
	cJSON_AddNumberToObject(keyboard, "down", game.keyControls[CONTROL_DOWN]);
	cJSON_AddNumberToObject(keyboard, "jump", game.keyControls[CONTROL_JUMP]);
	cJSON_AddNumberToObject(keyboard, "fire", game.keyControls[CONTROL_FIRE]);

	joypad = cJSON_CreateObject();
	cJSON_AddNumberToObject(joypad, "left", game.joypadControls[CONTROL_LEFT]);
	cJSON_AddNumberToObject(joypad, "right", game.joypadControls[CONTROL_RIGHT]);
	cJSON_AddNumberToObject(joypad, "up", game.joypadControls[CONTROL_UP]);
	cJSON_AddNumberToObject(joypad, "down", game.joypadControls[CONTROL_DOWN]);
	cJSON_AddNumberToObject(joypad, "jump", game.joypadControls[CONTROL_JUMP]);
	cJSON_AddNumberToObject(joypad, "fire", game.joypadControls[CONTROL_FIRE]);
	cJSON_AddNumberToObject(joypad, "deadzone", game.deadzone);

	volumes = cJSON_CreateObject();
	cJSON_AddNumberToObject(volumes, "sound", game.soundVolume);
	cJSON_AddNumberToObject(volumes, "music", game.musicVolume);

	controls = cJSON_CreateObject();
	cJSON_AddItemToObject(controls, "keyboard", keyboard);
	cJSON_AddItemToObject(controls, "joypad", joypad);

	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "controls", controls);
	cJSON_AddItemToObject(root, "volumes", volumes);

	out = cJSON_Print(root);

	writeFile(SAVE_GAME_FILENAME, out);

	cJSON_Delete(root);

	free(out);
}

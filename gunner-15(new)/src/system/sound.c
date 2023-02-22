/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include <SDL2/SDL_mixer.h>

#include "../common.h"

#include "sound.h"

static void loadSounds(void);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;

void initSound(void)
{
	memset(sounds, 0, sizeof(Mix_Chunk *) * SND_MAX);

	music = NULL;

	loadSounds();
}

void setSoundVolume(int vol)
{
	Mix_Volume(-1, vol);
}

void setMusicVolume(int vol)
{
	Mix_VolumeMusic(vol);
}

void loadMusic(char *filename)
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(filename);
}

void playMusic(int loop)
{
	Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void playSound(int id, int channel)
{
	Mix_PlayChannel(channel, sounds[id], 0);
}

void playSoundLoop(int id, int channel)
{
	Mix_PlayChannel(channel, sounds[id], -1);
}

void stopChannel(int channel)
{
	Mix_HaltChannel(channel);
}

static void loadSounds(void)
{
	sounds[SND_GUI] = Mix_LoadWAV("sound/264762__farpro__guiclick.ogg");
	sounds[SND_PLAYER_SHOOT] = Mix_LoadWAV("sound/321102__nsstudios__laser1.ogg");
	sounds[SND_ENEMY_SHOOT] = Mix_LoadWAV("sound/35681__jobro__laser4.ogg");
	sounds[SND_POWER_UP] = Mix_LoadWAV("sound/577965__colorscrimsontears__rpgpowerup.ogg");
	sounds[SND_DOOR_LOCKED] = Mix_LoadWAV("sound/572936__bloodpixelhero__error.ogg");
	sounds[SND_IMPACT] = Mix_LoadWAV("sound/470587__silverillusionist__heavy-impact.ogg");
	sounds[SND_DOOR_OPEN] = Mix_LoadWAV("sound/148309__keemocore__robot-step-short-medium-heavy.ogg");
	sounds[SND_METAL] = Mix_LoadWAV("sound/170959__timgormly__metal-clang.ogg");
	sounds[SND_BIG_EXPLOSION] = Mix_LoadWAV("sound/47252__deleted-user-364925__bad-explosion.ogg");
	sounds[SND_EXPLOSION] = Mix_LoadWAV("sound/523089__magnuswaker__explosion-1.ogg");
	sounds[SND_PLAYER_HURT] = Mix_LoadWAV("sound/76969__michel88__paino.ogg");
	sounds[SND_PLAYER_DIE] = Mix_LoadWAV("sound/536786__egomassive__screamm.ogg");
	sounds[SND_KEYCARD] = Mix_LoadWAV("sound/263133__pan14__tone-beep.ogg");
}

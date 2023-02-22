/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include <SDL2/SDL_mixer.h>

#include "../common.h"

#include "../game/game.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "../system/text.h"
#include "../system/widgets.h"
#include "options.h"

extern App	app;
extern Game game;

static void setupWidgets(void);
static void logic(void);
static void draw(void);
static void back(void);
static void backToOptions(void);

static void (*preLogic)(void);
static void (*preDraw)(void);
static void (*postOptions)(void);
static int wasInit = 0;
static int showControls;

void initOptions(void (*_preLogic)(void), void (*_preDraw)(void), void (*_postOptions)(void))
{
	preLogic = _preLogic;
	preDraw = _preDraw;
	postOptions = _postOptions;

	if (!wasInit)
	{
		setupWidgets();

		wasInit = 1;
	}

	showControls = 0;

	app.activeWidget = getWidget("sound", "options");

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (preLogic != NULL)
	{
		preLogic();
	}

	if (!showControls)
	{
		doWidgets("options");

		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

			back();
		}
	}
	else
	{
		doWidgets("controls");

		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

			backToOptions();
		}
	}
}

static void draw(void)
{
	preDraw();

	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 160);

	if (!showControls)
	{
		app.fontScale = 1.5;

		drawText("OPTIONS", SCREEN_WIDTH / 2, 70, 255, 255, 255, TEXT_ALIGN_CENTER, 0);

		app.fontScale = 1.0;

		drawWidgets("options");
	}
	else
	{
		app.fontScale = 1.5;

		drawText("CONTROLS", SCREEN_WIDTH / 2, 70, 255, 255, 255, TEXT_ALIGN_CENTER, 0);

		app.fontScale = 1.0;

		drawWidgets("controls");
	}
}

static void sound(void)
{
	SliderWidget *sw;

	sw = (SliderWidget *)app.activeWidget->data;

	game.soundVolume = MIX_MAX_VOLUME * (sw->value * 0.01);

	setSoundVolume(game.soundVolume);
}

static void music(void)
{
	SliderWidget *sw;

	sw = (SliderWidget *)app.activeWidget->data;

	game.musicVolume = MIX_MAX_VOLUME * (sw->value * 0.01);

	setMusicVolume(game.musicVolume);
}

static void controls(void)
{
	showControls = 1;

	app.activeWidget = getWidget("left", "controls");
}

static void deadzone(void)
{
	SliderWidget *sw;

	sw = (SliderWidget *)app.activeWidget->data;

	game.deadzone = DEADZONE_MAX * (sw->value * 0.01);
}

static void back(void)
{
	saveGame();

	postOptions();
}

static void backToOptions(void)
{
	showControls = 0;

	app.activeWidget = getWidget("controls", "options");
}

static void updateControls(void)
{
	ControlWidget *w;

	w = (ControlWidget *)getWidget("left", "controls")->data;
	game.keyControls[CONTROL_LEFT] = w->keyboard;

	w = (ControlWidget *)getWidget("right", "controls")->data;
	game.keyControls[CONTROL_RIGHT] = w->keyboard;

	w = (ControlWidget *)getWidget("up", "controls")->data;
	game.keyControls[CONTROL_UP] = w->keyboard;

	w = (ControlWidget *)getWidget("down", "controls")->data;
	game.keyControls[CONTROL_DOWN] = w->keyboard;

	w = (ControlWidget *)getWidget("jump", "controls")->data;
	game.keyControls[CONTROL_JUMP] = w->keyboard;

	w = (ControlWidget *)getWidget("fire", "controls")->data;
	game.keyControls[CONTROL_FIRE] = w->keyboard;
}

static void setupWidgets(void)
{
	Widget		   *w;
	SliderWidget	 *sw;
	ControlWidget *cw;

	w = getWidget("sound", "options");
	w->action = sound;
	sw = (SliderWidget *)w->data;
	sw->x = w->x + 175;
	sw->y = w->y + 16;
	sw->w = 350;
	sw->h = 32;
	sw->value = ((1.0 * game.soundVolume) / MIX_MAX_VOLUME) * 100;

	w = getWidget("music", "options");
	w->action = music;
	sw = (SliderWidget *)w->data;
	sw->x = w->x + 175;
	sw->y = w->y + 16;
	sw->w = 350;
	sw->h = 32;
	sw->value = ((1.0 * game.musicVolume) / MIX_MAX_VOLUME) * 100;

	w = getWidget("controls", "options");
	w->action = controls;

	w = getWidget("back", "options");
	w->action = back;

	w = getWidget("left", "controls");
	w->action = updateControls;
	cw = (ControlWidget *)w->data;
	cw->x = w->x + 200;
	cw->y = w->y;
	cw->keyboard = game.keyControls[CONTROL_LEFT];

	w = getWidget("right", "controls");
	w->action = updateControls;
	cw = (ControlWidget *)w->data;
	cw->x = w->x + 200;
	cw->y = w->y;
	cw->keyboard = game.keyControls[CONTROL_RIGHT];

	w = getWidget("up", "controls");
	w->action = updateControls;
	cw = (ControlWidget *)w->data;
	cw->x = w->x + 200;
	cw->y = w->y;
	cw->keyboard = game.keyControls[CONTROL_UP];

	w = getWidget("down", "controls");
	w->action = updateControls;
	cw = (ControlWidget *)w->data;
	cw->x = w->x + 200;
	cw->y = w->y;
	cw->keyboard = game.keyControls[CONTROL_DOWN];

	w = getWidget("jump", "controls");
	w->action = updateControls;
	cw = (ControlWidget *)w->data;
	cw->x = w->x + 200;
	cw->y = w->y;
	cw->keyboard = game.keyControls[CONTROL_JUMP];

	w = getWidget("fire", "controls");
	w->action = updateControls;
	cw = (ControlWidget *)w->data;
	cw->x = w->x + 200;
	cw->y = w->y;
	cw->keyboard = game.keyControls[CONTROL_FIRE];

	w = getWidget("deadzone", "controls");
	w->action = deadzone;
	sw = (SliderWidget *)w->data;
	sw->x = w->x + 225;
	sw->y = w->y + 16;
	sw->w = 350;
	sw->h = 32;
	sw->value = ((1.0 * game.deadzone) / DEADZONE_MAX) * 100;

	w = getWidget("back", "controls");
	w->action = backToOptions;

	updateControls();
}

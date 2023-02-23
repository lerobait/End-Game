#include "header.h"

extern App app;

void initSDL(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Couldn't initialize SDL Mixer\n");
		exit(1);
	}

	Mix_AllocateChannels(MAX_SND_CHANNELS);

	app.window = SDL_CreateWindow("SDL2 Gunner 15", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_ShowCursor(0);
}

static void initJoypad(void)
{
	int i, n;

	n = SDL_NumJoysticks();

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%d joysticks available", n);

	for (i = 0; i < n; i++)
	{
		app.joypad = SDL_JoystickOpen(i);

		if (app.joypad != NULL)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Joystick [name='%s', Axes=%d, Buttons=%d]", SDL_JoystickNameForIndex(i), SDL_JoystickNumAxes(app.joypad), SDL_JoystickNumButtons(app.joypad));

			return;
		}
	}
}

void initGameSystem(void)
{
	srand(time(NULL));

	initJoypad();

	initFonts();

	initAtlas();

	initWidgets();

	initSound();

	initTextures();

	initEntityFactory();

	initGame();
}

void cleanup(void)
{
	Mix_Quit();

	IMG_Quit();

	TTF_Quit();

	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();
}

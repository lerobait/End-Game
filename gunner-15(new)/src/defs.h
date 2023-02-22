/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#define PI		  3.14159265358979323846264338327950288
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRCPY(dest, src)             \
	strncpy(dest, src, sizeof(dest)); \
	dest[sizeof(dest) - 1] = '\0'
#define STRNCPY(dest, src, n) \
	strncpy(dest, src, n);    \
	dest[n - 1] = '\0'
#define TO_RAIDANS(angleDegrees) (angleDegrees * PI / 180.0)

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900

#define FPS		   60.0
#define LOGIC_RATE (FPS / 1000)

#define MAX_NAME_LENGTH		16
#define MAX_LINE_LENGTH		1024
#define MAX_INPUT_LENGTH	16
#define MAX_FILENAME_LENGTH 256

#define MAX_KEYBOARD_KEYS  350

#define MAX_SND_CHANNELS 16

#define GRAVITY		   1
#define MAX_FALL_SPEED 12

#define EF_NONE		   0
#define EF_WEIGHTLESS  (2 << 0)
#define EF_SOLID	   (2 << 1)
#define EF_INTERACTIVE (2 << 2)
#define EF_BLOCKS_LOS  (2 << 3)
#define EF_BOUNCES	   (2 << 4)

#define MAP_WIDTH  210
#define MAP_HEIGHT 95

#define MAP_RENDER_WIDTH  34
#define MAP_RENDER_HEIGHT 19

#define MAX_TILES	  100
#define MAP_TILE_SIZE 48

#define MAX_PLAYER_LIFE 5

#define MAX_QT_CANDIDATES 1024

enum
{
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

enum
{
	WT_BUTTON,
	WT_SELECT,
	WT_SLIDER,
	WT_INPUT,
	WT_CONTROL
};

enum
{
	CONTROL_LEFT,
	CONTROL_RIGHT,
	CONTROL_UP,
	CONTROL_DOWN,
	CONTROL_JUMP,
	CONTROL_FIRE,
	CONTROL_MAX
};

enum
{
	FACING_LEFT,
	FACING_RIGHT
};

enum
{
	WPN_NORMAL,
	WPN_SPREAD
};

enum
{
	KEYCARD_RED,
	KEYCARD_GREEN,
	KEYCARD_BLUE,
	KEYCARD_MAX
};

enum
{
	LAYER_FOREGROUND,
	LAYER_BACKGROUND
};

enum
{
	DRONE_GREY,
	DRONE_RED,
	DRONE_MAX
};

enum
{
	MISSION_INCOMPLETE,
	MISSION_COMPLETE,
	MISSION_FAILED
};

enum
{
	CH_ANY = -1,
	CH_PLAYER,
	CH_ENEMY
};

enum
{
	SND_GUI,
	SND_PLAYER_SHOOT,
	SND_ENEMY_SHOOT,
	SND_POWER_UP,
	SND_DOOR_OPEN,
	SND_KEYCARD,
	SND_DOOR_LOCKED,
	SND_IMPACT,
	SND_METAL,
	SND_EXPLOSION,
	SND_BIG_EXPLOSION,
	SND_PLAYER_HURT,
	SND_PLAYER_DIE,
	SND_MAX
};

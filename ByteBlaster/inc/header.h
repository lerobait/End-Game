#ifndef HEADER_H
#define HEADER_H

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define FPS	60.0
#define LOGIC_RATE (FPS / 1000)

#define MAX_NAME_LENGTH	16
#define MAX_LINE_LENGTH	1024
#define MAX_INPUT_LENGTH 16
#define MAX_FILENAME_LENGTH 256

#define MAX_KEYBOARD_KEYS 350

#define MAX_SND_CHANNELS 16

#define GRAVITY	1
#define MAX_FALL_SPEED 12

#define EF_NONE	0
#define EF_WEIGHTLESS (2 << 0)
#define EF_SOLID (2 << 1)
#define EF_INTERACTIVE (2 << 2)
#define EF_BLOCKS_LOS (2 << 3)
#define EF_BOUNCES (2 << 4)

#define MAP_WIDTH 210
#define MAP_HEIGHT 95

#define MAP_RENDER_WIDTH 34
#define MAP_RENDER_HEIGHT 19

#define MAX_TILES 100
#define MAP_TILE_SIZE 48

#define MAX_PLAYER_LIFE 5

#define MAX_QT_CANDIDATES 1024

#define PI 3.14159265358979323846264338327950288
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRCPY(dest, src)             \
	strncpy(dest, src, sizeof(dest)); \
	dest[sizeof(dest) - 1] = '\0'
#define STRNCPY(dest, src, n) \
	strncpy(dest, src, n);    \
	dest[n - 1] = '\0'
#define TO_RAIDANS(angleDegrees) (angleDegrees * PI / 180.0)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "SDL2_mixer/SDL_mixer.h"
#include "SDL2_ttf/SDL_ttf.h"

enum {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

enum {
	WT_BUTTON,
	WT_SELECT,
	WT_SLIDER,
	WT_INPUT,
	WT_CONTROL
};

enum {
	CONTROL_LEFT,
	CONTROL_RIGHT,
	CONTROL_UP,
	CONTROL_DOWN,
	CONTROL_JUMP,
	CONTROL_FIRE,
	CONTROL_MAX
};

enum {
	FACING_LEFT,
	FACING_RIGHT
};

enum {
	WPN_NORMAL,
	WPN_SPREAD
};

enum {
	KEYCARD_RED,
	KEYCARD_GREEN,
	KEYCARD_BLUE,
	KEYCARD_MAX
};

enum {
	LAYER_FOREGROUND,
	LAYER_BACKGROUND
};

enum {
	DRONE_GREY,
	DRONE_RED,
	DRONE_MAX
};

enum {
	MISSION_INCOMPLETE,
	MISSION_COMPLETE,
	MISSION_FAILED
};

enum {
	CH_ANY = -1,
	CH_PLAYER,
	CH_ENEMY
};

enum {
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

typedef struct Texture Texture;
typedef struct AtlasImage AtlasImage;
typedef struct Entity Entity;
typedef struct InitFunc	InitFunc;
typedef struct Widget Widget;
typedef struct Bullet Bullet;
typedef struct Quadtree	Quadtree;
typedef struct Effect Effect;

struct Texture {
	char name[MAX_FILENAME_LENGTH];
	SDL_Texture *texture;
	Texture	*next;
};

struct AtlasImage {
	char filename[MAX_FILENAME_LENGTH];
	SDL_Texture *texture;
	SDL_Rect rect;
	int	rotated;
	AtlasImage *next;
};

struct Entity {
	double x;
	double y;
	double dx;
	double dy;
	int	facing;
	int	onGround;
	int	dead;
	int	layer;
	SDL_Rect hitbox;
	AtlasImage *texture;
	unsigned long flags;
	void(*data);
	void (*tick)(Entity *self);
	void (*draw)(Entity *self);
	void (*touch)(Entity *self, Entity *other);
	void (*takeDamage)(Entity *self, int amount, Entity *attacker);
	Entity *next;
};

typedef struct {
	int rest;
	int	frame;
	int	ducking;
	int	aimingUp;
	int	life;
	int	weaponType;
	double animTimer;
	double reload;
	double immuneTimer;
	int hasKeycard[KEYCARD_MAX];
	SDL_Point checkpoint;
	double checkpointTimer;
} Gunner;

typedef struct {
	int life;
	double damageTimer;
} Target;

typedef struct {
	int	life;
	double thinkTime;
	double damageTimer;
	int	shotsToFire;
	double reload;
} EnemySoldier;

typedef struct {
	int	open;
	int	life;
	int	ty;
	int	requiredKeycard;
	double damageTimer;
	double thinkTime;
} Door;

typedef struct {
	int type;
} Keycard;

typedef struct {
	int life;
	double damageTimer;
} Serv;

typedef struct {
	double life;
	double smokeTime;
} Debris;

typedef struct {
	int type;
	int	life;
	double thinkTime;
	double damageTimer;
	double despawnTimer;
	double engineTimer;
	int	shotsToFire;
	double reload;
} Drone;

struct Bullet {
	double x;
	double y;
	double dx;
	double dy;
	double life;
	int	damage;
	Entity *owner;
	AtlasImage *texture;
	Bullet *next;
};

struct InitFunc {
	char name[MAX_NAME_LENGTH];
	void (*init)(Entity *e);
	InitFunc *next;
};

struct Quadtree {
	int depth;
	int	x, y, w, h;
	Entity **ents;
	int	capacity;
	int	numEnts;
	int	addedTo;
	Quadtree *node[4];
};

struct Effect {
	double x;
	double y;
	double dx;
	double dy;
	int	size;
	double life;
	double alpha;
	SDL_Color color;
	Effect *next;
};

struct Widget {
	int	type;
	char name[MAX_NAME_LENGTH];
	char groupName[MAX_NAME_LENGTH];
	int	x;
	int	y;
	int	w;
	int	h;
	char label[MAX_NAME_LENGTH];
	Widget *prev;
	Widget *next;
	void(*action)(void);
	void(*data);
};

typedef struct {
	int numOptions;
	char **options;
	int	x;
	int	y;
	int	value;
} SelectWidget;

typedef struct {
	int x;
	int y;
	int w;
	int h;
	int value;
	int step;
	int waitOnChange;
} SliderWidget;

typedef struct {
	int	x;
	int	y;
	int	maxLength;
	char *text;
} InputWidget;

typedef struct {
	int x;
	int y;
	int keyboard;
} ControlWidget;

typedef struct {
	int keyControls[CONTROL_MAX];
	int soundVolume;
	int musicVolume;
} Game;

typedef struct {
	Entity entityHead, *entityTail;
	Bullet bulletHead, *bulletTail;
	Effect effectHead, *effectTail;
	Entity *player;
	int	status;
	int	map[MAP_WIDTH][MAP_HEIGHT];
	SDL_Point camera;
	Quadtree quadtree;
	Entity *nearestServ;
	int	numServ;
	double time;
} Stage;

typedef struct {
	struct {
		void (*logic)(void);
		void (*draw)(void);
	} delegate;

	SDL_Renderer *renderer;
	SDL_Window *window;
	int	keyboard[MAX_KEYBOARD_KEYS];
	char inputText[MAX_INPUT_LENGTH];
	int	lastKeyPressed;
	int	lastButtonPressed;
	double deltaTime;
	double fontScale;
	Widget *activeWidget;
	struct {
		int fps;
		int collisionChecks;
		int showHitboxes;
	} dev;
} App;

int canSeePlayer(Entity *e);
AtlasImage *getAtlasImage(char *filename, int required);
void initAtlas(void);
void initBlueSoldier(Entity *e);
void clearBullets(void);
void drawBullets(void);
void doBullets(void);
Bullet *spawnBullet(Entity *owner);
void initBullets(void);
void addDebris(int x, int y);
void initBlueDoor(Entity *e);
void initGreenDoor(Entity *e);
void initRedDoor(Entity *e);
void initWeakDoor(Entity *e);
void initNormalDoor(Entity *e);
void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
void blitRotated(AtlasImage *atlasImage, int x, int y, float angle);
void blitAtlasImageScaled(AtlasImage *atlasImage, int x, int y, int w, int h, int center);
void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
void blit(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip);
void presentScene(void);
void prepareScene(void);
void addDrone(int x, int y);
void clearEffects(void);
void addBulletImpactEffect(Bullet *b);
void addSmokeEffect(int x, int y);
void addExplosionEffect(int x, int y, int size);
void drawEffects(void);
void doEffects(void);
void initEffects(void);
void clearEntities(void);
void drawEntities(int layer);
void doEntities(void);
void initEntities(void);
Entity *spawnEntity(void);
Entity *initEntity(char *name);
void initEntityFactory(void);
void saveGame(void);
void initGame(void);
void initGreenSoldier(Entity *e);
void drawHud(void);
void doHud(void);
void initHud(void);
void cleanup(void);
void initGameSystem(void);
void initSDL(void);
void doInput(void);
void initBlueKeycard(Entity *e);
void initGreenKeycard(Entity *e);
void initRedKeycard(Entity *e);
int main(int argc, char *argv[]);
int	isInsideMap(int x, int y);
void drawMap(void);
void initMap(void);
void initServ(Entity *e);
void initOptions(void (*_preLogic)(void), void (*_preDraw)(void), void (*_postOptions)(void));
int	 resetPlayer(void);
void initPlayer(Entity *e);
void initSpreadGun(Entity *e);
void initHealth(Entity *e);
void destroyQuadtree(Quadtree *root);
void getAllEntsWithin(int x, int y, int w, int h, Entity **candidates, Entity *ignore);
void removeFromQuadtree(Entity *e, Quadtree *root);
void addToQuadtree(Entity *e, Quadtree *root);
void initQuadtree(Quadtree *root);
void stopChannel(int channel);
void playSoundLoop(int id, int channel);
void playSound(int id, int channel);
void playMusic(int loop);
void loadMusic(char *filename);
void setMusicVolume(int vol);
void setSoundVolume(int vol);
void initSound(void);
void updateStageStatus(int status);
void initStage(void);
void initTarget(Entity *e);
int	getWrappedTextHeight(char *text, int maxWidth);
void calcTextDimensions(char *text, int *w, int *h);
void drawText(char *text, int x, int y, int r, int g, int b, int align, int maxWidth);
SDL_Texture *getTextTexture(char *text, int type);
void initFonts(void);
SDL_Texture *loadTexture(char *filename);
SDL_Texture *toTexture(SDL_Surface *surface, int destroySurface);
void initTextures(void);
void initTitle(void);
void *resize(void *array, int oldSize, int newSize);
void calcSlope(int x1, int y1, int x2, int y2, double *dx, double *dy);
int	writeFile(const char *filename, const char *data);
char *readFile(char *filename);
double getAngle(int x1, int y1, int x2, int y2);
int	getDistance(int x1, int y1, int x2, int y2);
int	lineRectCollision(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh);
int	lineLineCollision(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int	collisionRects(SDL_Rect *r1, SDL_Rect *r2);
int	collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
unsigned long hashcode(const char *str);
Widget *getWidget(char *name, char *groupName);
void drawWidgets(char *groupName);
void doWidgets(char *groupName);
void initWidgets(void);

#endif


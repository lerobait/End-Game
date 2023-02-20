#ifndef HEADER_H
#define HEADER_H

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRCPY(dest, src)             \
	strncpy(dest, src, sizeof(dest)); \
	dest[sizeof(dest) - 1] = '\0'
#define STRNCPY(dest, src, n) \
	strncpy(dest, src, n);    \
	dest[n - 1] = '\0'

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900

#define FPS		   60.0
#define LOGIC_RATE (FPS / 1000)

#define MAX_NAME_LENGTH		16
#define MAX_LINE_LENGTH		1024
#define MAX_INPUT_LENGTH	16
#define MAX_FILENAME_LENGTH 256

#define MAX_KEYBOARD_KEYS 350

#define MAX_SND_CHANNELS 16

#define GRAVITY		   1
#define MAX_FALL_SPEED 12

#include <math.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

enum
{
	FACING_LEFT,
	FACING_RIGHT
};

enum
{
	SND_MAX
};


typedef struct Texture	  Texture;
typedef struct AtlasImage AtlasImage;
typedef struct Entity	  Entity;
typedef struct InitFunc	  InitFunc;
typedef struct Bullet	  Bullet;

struct Texture
{
	char		 name[MAX_FILENAME_LENGTH];
	SDL_Texture *texture;
	Texture		*next;
};

struct AtlasImage
{
	char		 filename[MAX_FILENAME_LENGTH];
	SDL_Texture *texture;
	SDL_Rect	 rect;
	int			 rotated;
	AtlasImage  *next;
};

struct Entity
{
	double		x;
	double		y;
	double		dx;
	double		dy;
	int			facing;
	int			onGround;
	AtlasImage *texture;
	void(*data);
	void (*tick)(Entity *self);
	void (*draw)(Entity *self);
	Entity *next;
};

typedef struct
{
	int	   frame;
	int	   ducking;
	int	   aimingUp;
	double animTimer;
	double reload;
} Gunner;

struct Bullet
{
	double		x;
	double		y;
	double		dx;
	double		dy;
	double		life;
	Entity	   *owner;
	AtlasImage *texture;
	Bullet	   *next;
};

struct InitFunc
{
	char name[MAX_NAME_LENGTH];
	void (*init)(Entity *e);
	InitFunc *next;
};

typedef struct
{
	Entity entityHead, *entityTail;
	Bullet bulletHead, *bulletTail;
} Stage;

typedef struct
{
	struct
	{
		void (*logic)(void);
		void (*draw)(void);
	} delegate;
	SDL_Renderer *renderer;
	SDL_Window   *window;
	int			  keyboard[MAX_KEYBOARD_KEYS];
	char		  inputText[MAX_INPUT_LENGTH];
	int			  lastKeyPressed;
	double		  deltaTime;
	double		  fontScale;
	struct
	{
		int fps;
		int showFPS;
	} dev;
} App;

int canSeePlayer(int x, int y);
AtlasImage *getAtlasImage(char *filename, int required);
void		initAtlas(void);
void initBlueSoldier(Entity *e);
void	drawBullets(void);
void	doBullets(void);
Bullet *spawnBullet(Entity *owner);
void	initBullets(void);
void addDebris(int x, int y);
void initRedDoor(Entity *e);
void initWeakDoor(Entity *e);
void initNormalDoor(Entity *e);
void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
void blit(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip);
void presentScene(void);
void prepareScene(void);
void addDrone(int x, int y);
void addBulletImpactEffect(Bullet *b);
void addSmokeEffect(int x, int y);
void addExplosionEffect(int x, int y, int size);
void drawEffects(void);
void doEffects(void);
void initEffects(void);
void drawEntities(void);
void doEntities(void);
void initEntities(void);
Entity *spawnEntity(void);
Entity *initEntity(char *name);
void	initEntityFactory(void);
void saveGame(void);
void initGame(void);
void initGreenSoldier(Entity *e);
void cleanup(void);
void initGameSystem(void);
void initSDL(void);
void doInput(void);
void initRedKeycard(Entity *e);
int main(int argc, char *argv[]);
int	 isInsideMap(int x, int y);
void drawMap(void);
void initMap(void);
void initOilDrum(Entity *e);
void initOptions(void (*_preLogic)(void), void (*_preDraw)(void), void (*_postOptions)(void));
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
void initStage(void);
void initTarget(Entity *e);
int			 getWrappedTextHeight(char *text, int maxWidth);
void		 calcTextDimensions(char *text, int *w, int *h);
void		 drawText(char *text, int x, int y, int r, int g, int b, int align, int maxWidth);
SDL_Texture *getTextTexture(char *text, int type);
void		 initFonts(void);
SDL_Texture *loadTexture(char *filename);
SDL_Texture *toTexture(SDL_Surface *surface, int destroySurface);
void		 initTextures(void);
void initTitle(void);void		  calcSlope(int x1, int y1, int x2, int y2, double *dx, double *dy);
int			  writeFile(const char *filename, const char *data);
char		 *readFile(char *filename);
int			  getDistance(int x1, int y1, int x2, int y2);
int			  collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
unsigned long hashcode(const char *str);




#endif


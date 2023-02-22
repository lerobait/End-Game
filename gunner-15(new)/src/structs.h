/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

typedef struct Texture	  Texture;
typedef struct AtlasImage AtlasImage;
typedef struct Entity	  Entity;
typedef struct InitFunc	  InitFunc;
typedef struct Widget	  Widget;
typedef struct Bullet	  Bullet;
typedef struct Quadtree	  Quadtree;
typedef struct Effect	  Effect;

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
	double		  x;
	double		  y;
	double		  dx;
	double		  dy;
	int			  facing;
	int			  onGround;
	int			  dead;
	int			  layer;
	SDL_Rect	  hitbox;
	AtlasImage   *texture;
	unsigned long flags;
	void(*data);
	void (*tick)(Entity *self);
	void (*draw)(Entity *self);
	void (*touch)(Entity *self, Entity *other);
	void (*takeDamage)(Entity *self, int amount, Entity *attacker);
	Entity *next;
};

typedef struct
{
	int		  rest;
	int		  frame;
	int		  ducking;
	int		  aimingUp;
	int		  life;
	int		  weaponType;
	double	  animTimer;
	double	  reload;
	double	  immuneTimer;
	int		  hasKeycard[KEYCARD_MAX];
	SDL_Point checkpoint;
	double	  checkpointTimer;
} Gunner;

typedef struct
{
	int	   life;
	double damageTimer;
} Target;

typedef struct
{
	int	   life;
	double thinkTime;
	double damageTimer;
	int	   shotsToFire;
	double reload;
} EnemySoldier;

typedef struct
{
	int	   open;
	int	   life;
	int	   ty;
	int	   requiredKeycard;
	double damageTimer;
	double thinkTime;
} Door;

typedef struct
{
	int type;
} Keycard;

typedef struct
{
	int	   life;
	double damageTimer;
} OilDrum;

typedef struct
{
	double life;
	double smokeTime;
} Debris;

typedef struct
{
	int	   type;
	int	   life;
	double thinkTime;
	double damageTimer;
	double despawnTimer;
	double engineTimer;
	int	   shotsToFire;
	double reload;
} Drone;

struct Bullet
{
	double		x;
	double		y;
	double		dx;
	double		dy;
	double		life;
	int			damage;
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

struct Quadtree
{
	int		  depth;
	int		  x, y, w, h;
	Entity  **ents;
	int		  capacity;
	int		  numEnts;
	int		  addedTo;
	Quadtree *node[4];
};

struct Effect
{
	double	  x;
	double	  y;
	double	  dx;
	double	  dy;
	int		  size;
	double	  life;
	double	  alpha;
	SDL_Color color;
	Effect   *next;
};

struct Widget
{
	int		type;
	char	name[MAX_NAME_LENGTH];
	char	groupName[MAX_NAME_LENGTH];
	int		x;
	int		y;
	int		w;
	int		h;
	char	label[MAX_NAME_LENGTH];
	Widget *prev;
	Widget *next;
	void (*action)(void);
	void(*data);
};

typedef struct
{
	int	   numOptions;
	char **options;
	int	   x;
	int	   y;
	int	   value;
} SelectWidget;

typedef struct
{
	int x;
	int y;
	int w;
	int h;
	int value;
	int step;
	int waitOnChange;
} SliderWidget;

typedef struct
{
	int	  x;
	int	  y;
	int	  maxLength;
	char *text;
} InputWidget;

typedef struct
{
	int x;
	int y;
	int keyboard;
} ControlWidget;

typedef struct
{
	int keyControls[CONTROL_MAX];
	int soundVolume;
	int musicVolume;
} Game;

typedef struct
{
	Entity	  entityHead, *entityTail;
	Bullet	  bulletHead, *bulletTail;
	Effect	  effectHead, *effectTail;
	Entity   *player;
	int		  status;
	int		  map[MAP_WIDTH][MAP_HEIGHT];
	SDL_Point camera;
	Quadtree  quadtree;
	Entity   *nearestOilDrum;
	int		  numOilDrums;
	double	  time;
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
	Widget	   *activeWidget;
	struct
	{
		int fps;
		int collisionChecks;
		int showHitboxes;
	} dev;
} App;

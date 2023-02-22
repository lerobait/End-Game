/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../entities/debris.h"
#include "../game/bullets.h"
#include "../game/effects.h"
#include "../game/stage.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "player.h"

#define NUM_RUN_TEXTURES	  6
#define RUN_SPEED			  7
#define JUMP_SPEED			  -15
#define ANIM_TIME			  6
#define RELOAD_SPEED		  8
#define BULLET_SPEED		  24
#define CHECKPOINT_TIMER_TIME (FPS * 10)
#define NUM_PLAYER_RESTS	  2

extern App	 app;
extern Game	 game;
extern Stage stage;

static void tick(Entity *self);
static void handleMovement(Entity *self, Gunner *g);
static void handleShoot(Entity *self, Gunner *g);
static void updateHitbox(Entity *self, Gunner *g);
static void draw(Entity *self);
static void fireBullet(Entity *self, Gunner *g, int baseAngle);
static void takeDamage(Entity *self, int amount, Entity *attacker);
static int	isControl(int type);

static AtlasImage *standTexture = NULL;
static AtlasImage *aimUpTexture;
static AtlasImage *runTextures[NUM_RUN_TEXTURES];
static AtlasImage *aimUpRightTextures[NUM_RUN_TEXTURES];
static AtlasImage *jumpTexture;
static AtlasImage *duckTexture;
static AtlasImage *bulletTexture;

void initPlayer(Entity *e)
{
	int		i;
	char	filename[MAX_FILENAME_LENGTH];
	Gunner *g;

	if (standTexture == NULL)
	{
		for (i = 0; i < NUM_RUN_TEXTURES; i++)
		{
			sprintf(filename, "gfx/sprites/gunnerRun%02d.png", i + 1);
			runTextures[i] = getAtlasImage(filename, 1);

			sprintf(filename, "gfx/sprites/gunnerAimUpRight%02d.png", i + 1);
			aimUpRightTextures[i] = getAtlasImage(filename, 1);
		}

		standTexture = getAtlasImage("gfx/sprites/gunnerStand.png", 1);

		aimUpTexture = getAtlasImage("gfx/sprites/gunnerAimUp.png", 1);

		jumpTexture = getAtlasImage("gfx/sprites/gunnerJump.png", 1);

		duckTexture = getAtlasImage("gfx/sprites/gunnerDuck.png", 1);

		bulletTexture = getAtlasImage("gfx/sprites/playerBullet.png", 1);
	}

	g = malloc(sizeof(Gunner));
	memset(g, 0, sizeof(Gunner));

	g->rest = NUM_PLAYER_RESTS;
	g->animTimer = ANIM_TIME;
	g->life = MAX_PLAYER_LIFE;
	g->weaponType = WPN_NORMAL;
	g->checkpoint.x = e->x;
	g->checkpoint.y = e->y;

	e->facing = FACING_RIGHT;
	e->texture = standTexture;
	e->data = g;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;

	stage.player = e;
}

static void tick(Entity *self)
{
	Gunner *g;

	g = (Gunner *)self->data;

	g->immuneTimer = MAX(g->immuneTimer - app.deltaTime, 0);

	g->checkpointTimer = MAX(g->checkpointTimer - app.deltaTime, 0);

	if (g->checkpointTimer == 0 && self->onGround)
	{
		g->checkpoint.x = self->x;
		g->checkpoint.y = self->y;

		g->checkpointTimer = CHECKPOINT_TIMER_TIME;
	}

	handleMovement(self, g);

	handleShoot(self, g);

	updateHitbox(self, g);
}

static void handleMovement(Entity *self, Gunner *g)
{
	self->dx = 0;

	g->aimingUp = 0;

	g->ducking = 0;

	if (isControl(CONTROL_LEFT))
	{
		self->dx = -RUN_SPEED;

		self->facing = FACING_LEFT;
	}

	if (isControl(CONTROL_RIGHT))
	{
		self->dx = RUN_SPEED;

		self->facing = FACING_RIGHT;
	}

	if (isControl(CONTROL_JUMP) && self->onGround)
	{
		self->dy = JUMP_SPEED;

		self->onGround = 0;
	}

	if (!self->onGround)
	{
		self->texture = jumpTexture;
	}
	else if (isControl(CONTROL_DOWN))
	{
		self->texture = duckTexture;

		self->dx = 0;

		g->ducking = 1;

		g->frame = 0;
	}
	else
	{
		if (isControl(CONTROL_UP))
		{
			g->aimingUp = 1;
		}

		if (self->dx != 0)
		{
			g->animTimer -= app.deltaTime;

			if (g->animTimer <= 0)
			{
				g->frame = (g->frame + 1) % NUM_RUN_TEXTURES;

				g->animTimer = ANIM_TIME;
			}
			
		  
			self->texture = runTextures[g->frame];
			
			
		}
		else
		{
			g->frame = 0;

			if (g->aimingUp)
			{
				self->texture = aimUpTexture;
			}
			else
			{
				self->texture = standTexture;
			}
		}
	}
}

static void handleShoot(Entity *self, Gunner *g)
{
	g->reload = MAX(g->reload - app.deltaTime, 0);

	if (isControl(CONTROL_FIRE) && g->reload == 0)
	{
		switch (g->weaponType)
		{
			case WPN_SPREAD:
				fireBullet(self, g, 10);
				fireBullet(self, g, 0);
				fireBullet(self, g, -10);
				break;

			default:
				fireBullet(self, g, 0);
				break;
		}

		g->reload = RELOAD_SPEED;

		playSound(SND_PLAYER_SHOOT, CH_PLAYER);
	}
}

static void fireBullet(Entity *self, Gunner *g, int angle)
{
    Bullet *b;
    int baseAngle;
    
    b = spawnBullet(self);
    b->texture = bulletTexture;
    b->damage = 1;
    b->life = FPS * 2;
    
    baseAngle = self->facing == FACING_RIGHT ? 90 : -90;
    
    if (g->ducking)
    {
        b->y = self->y + self->texture->rect.h - 34;
        b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x - b->texture->rect.w;
    }
    else if (g->aimingUp)
    {
        b->y = self->y - b->texture->rect.h;
        b->dy = -BULLET_SPEED;
        
        if (self->dx == 0)
        {
            b->x = self->x + (self->texture->rect.w / 2) - (b->texture->rect.w / 2);
            b->x += self->facing == FACING_RIGHT ? 12 : -12;
            
            baseAngle = 0;
        }
        else if (isControl(CONTROL_LEFT) && self->facing == FACING_LEFT)
        {
            b->x = self->x - b->texture->rect.w;
            b->dx = -BULLET_SPEED;
            
            baseAngle = -90;
        }
        else if (isControl(CONTROL_RIGHT) && self->facing == FACING_RIGHT)
        {
            b->x = self->x + self->texture->rect.w;
            b->dx = BULLET_SPEED;
            
            baseAngle = 90;
        }
        else
        {
            b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x;
            
            baseAngle = self->facing == FACING_RIGHT ? 45 : -45;
        }
    }
    else
    {
        b->x = self->facing == FACING_RIGHT ? self->x + self->texture->rect.w : self->x;
        b->y = self->y + 19;
        
        if (isControl(CONTROL_LEFT) && self->facing == FACING_LEFT)
        {
            b->dx = -BULLET_SPEED;
            baseAngle = -90;
        }
        else if (isControl(CONTROL_RIGHT) && self->facing == FACING_RIGHT)
        {
            b->dx = BULLET_SPEED;
            baseAngle = 90;
        }
    }
    
    angle += baseAngle;
    
    b->dx = sin(TO_RAIDANS(angle)) * BULLET_SPEED;
    b->dy = -cos(TO_RAIDANS(angle)) * BULLET_SPEED;
}

static void updateHitbox(Entity *self, Gunner *g)
{
	self->hitbox.w = 44;

	if (!g->ducking)
	{
		self->hitbox.x = self->x + 16;
		self->hitbox.y = self->y;
		self->hitbox.h = self->texture->rect.h;
	}
	else
	{
		self->hitbox.x = self->x + 12;
		self->hitbox.y = self->y + 34;
		self->hitbox.h = 46;
	}
}

static void draw(Entity *self)
{
	Gunner *g;

	g = (Gunner *)self->data;

	if ((int)g->immuneTimer % 5 == 0)
	{
		blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, self->facing == FACING_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
	}
}

static void takeDamage(Entity *self, int amount, Entity *attacker)
{
	Gunner *g;
	int		i, x, y;

	g = (Gunner *)self->data;

	if (g->immuneTimer == 0)
	{
		g->life -= amount;

		g->immuneTimer = FPS * 1.5;

		playSound(SND_METAL, CH_ANY);

		if (g->life <= 0)
		{
			for (i = 0; i < 32; i++)
			{
				x = self->x + rand() % self->texture->rect.w;
				y = self->y + rand() % self->texture->rect.h;

				addExplosionEffect(x, y, 96);
			}

			for (i = 0; i < 8; i++)
			{
				x = self->x + (self->texture->rect.w / 2);
				y = self->y + (self->texture->rect.h / 2);

				addDebris(x, y);
			}

			self->dead = 1;

			if (g->rest == 0)
			{
				updateStageStatus(MISSION_FAILED);
			}

			playSound(SND_EXPLOSION, CH_ANY);

			playSound(SND_PLAYER_DIE, CH_ANY);
		}
		else
		{
			playSound(SND_PLAYER_HURT, CH_ANY);
		}
	}
}

static int isControl(int type)
{
	int key;

	if (type == CONTROL_LEFT)
	{
		return 1;
	}

	if (type == CONTROL_RIGHT)
	{
		return 1;
	}

	if (type == CONTROL_UP)
	{
		return 1;
	}

	if (type == CONTROL_DOWN)
	{
		return 1;
	}

	key = game.keyControls[type];

	return ((key != 0 && app.keyboard[key]));
}

int resetPlayer(void)
{
	Gunner *g;

	g = (Gunner *)stage.player->data;

	if (g->rest > 0)
	{
		stage.player->x = g->checkpoint.x;
		stage.player->y = g->checkpoint.y;
		stage.player->dead = 0;
		stage.player->onGround = 1;
		stage.player->texture = standTexture;

		stage.entityTail->next = stage.player;
		stage.entityTail = stage.player;

		g->life = MAX_PLAYER_LIFE;
		g->weaponType = WPN_NORMAL;
		g->immuneTimer = FPS * 3;

		g->rest--;

		return 1;
	}

	return 0;
}

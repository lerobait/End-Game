#include "../../inc/header.h"

#define NUM_TEXTURES 3

extern App app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void takeDamage(Entity *self, int damage, Entity *attacker);

static AtlasImage *textures[NUM_TEXTURES] = {NULL};
static int nearestServDistance;

void initServ(Entity *e) {
	Serv *o;

	o = malloc(sizeof(Serv));
	memset(o, 0, sizeof(Serv));

	o->life = 12;

	if(textures[0] == NULL) {
		textures[0] = getAtlasImage("gfx/sprites/greenServ.png", 1);
		textures[1] = getAtlasImage("gfx/sprites/blueServ.png", 1);
		textures[2] = getAtlasImage("gfx/sprites/greyServ.png", 1);
	}
	e->flags = EF_SOLID;

	e->data = o;
	e->texture = textures[rand() % NUM_TEXTURES];
	e->hitbox.w = e->texture->rect.w;
	e->hitbox.h = e->texture->rect.h;

	e->tick = tick;
	e->draw = draw;
	e->takeDamage = takeDamage;

	stage.numServ++;
}
static void tick(Entity *self) {
	Serv *o;
	int	dist;

	o = (Serv *)self->data;

	o->damageTimer = MAX(o->damageTimer - app.deltaTime, 0);

	self->hitbox.x = self->x;
	self->hitbox.y = self->y;

	dist = getDistance(self->x, self->y, stage.player->x, stage.player->y);

	if(stage.nearestServ == NULL || dist < nearestServDistance) {
		stage.nearestServ = self;

		nearestServDistance = dist;
	}
}
static void draw(Entity *self) {
	Serv *o;
	int	x, y;

	o = (Serv *)self->data;

	x = self->x - stage.camera.x;
	y = self->y - stage.camera.y;

	if(o->damageTimer == 0) {
		blitAtlasImage(self->texture, x, y, 0, SDL_FLIP_NONE);
	}
	else {
		SDL_SetTextureColorMod(self->texture->texture, 255, 32, 32);
		blitAtlasImage(self->texture, x, y, 0, SDL_FLIP_NONE);
		SDL_SetTextureColorMod(self->texture->texture, 255, 255, 255);
	}
}
static void takeDamage(Entity *self, int damage, Entity *attacker) {
	Serv *o;
	int	i, x, y;

	o = (Serv *)self->data;

	o->damageTimer = 2;

	o->life -= damage;

	playSound(SND_METAL, CH_ANY);

	if(o->life <= 0) {
		for(i = 0; i < 32; i++) {
			x = self->x + rand() % self->texture->rect.w;
			y = self->y + rand() % self->texture->rect.h;

			addExplosionEffect(x, y, 256);
		}
		for (i = 0; i < 16; i++) {
			x = self->x + (self->texture->rect.w / 2);
			y = self->y + (self->texture->rect.h / 2);

			addDebris(x, y);
		}
		self->dead = 1;

		stage.numServ--;

		if(stage.numServ <= 0) {
			updateStageStatus(MISSION_COMPLETE);
		}
		playSound(SND_BIG_EXPLOSION, CH_ANY);
	}
}

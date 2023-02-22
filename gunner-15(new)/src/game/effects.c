
#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "effects.h"

extern App	 app;
extern Stage stage;

static AtlasImage *explosionTexture;

void initEffects(void)
{
	explosionTexture = getAtlasImage("gfx/effects/explosion.png", 1);

	memset(&stage.effectHead, 0, sizeof(Effect));
	stage.effectTail = &stage.effectHead;
}

void doEffects(void)
{
	Effect *e, *prev;

	prev = &stage.effectHead;

	for (e = stage.effectHead.next; e != NULL; e = e->next)
	{
		e->life -= app.deltaTime;
		e->alpha -= app.deltaTime;

		e->x += e->dx * app.deltaTime;
		e->y += e->dy * app.deltaTime;

		if (e->life <= 0 || e->alpha <= 0)
		{
			prev->next = e->next;

			if (e == stage.effectTail)
			{
				stage.effectTail = prev;
			}

			free(e);

			e = prev;
		}

		prev = e;
	}
}

void drawEffects(void)
{
	Effect *e;
	int		x, y;

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(explosionTexture->texture, SDL_BLENDMODE_ADD);

	for (e = stage.effectHead.next; e != NULL; e = e->next)
	{
		SDL_SetTextureAlphaMod(explosionTexture->texture, MIN(e->alpha, 255));
		SDL_SetTextureColorMod(explosionTexture->texture, e->color.r, e->color.g, e->color.b);

		x = e->x - stage.camera.x - (e->size / 2);
		y = e->y - stage.camera.y - (e->size / 2);

		blitAtlasImageScaled(explosionTexture, x, y, e->size, e->size, 0);
	}

	SDL_SetTextureColorMod(explosionTexture->texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(explosionTexture->texture, 255);

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(explosionTexture->texture, SDL_BLENDMODE_BLEND);
}

void addExplosionEffect(int x, int y, int size)
{
	Effect *e;

	e = malloc(sizeof(Effect));
	memset(e, 0, sizeof(Effect));
	stage.effectTail->next = e;
	stage.effectTail = e;

	e->x = x;
	e->y = y;
	e->size = size;
	e->life = e->alpha = FPS + (FPS / 4);

	switch (rand() % 5)
	{
		case 0:
			e->color.r = 255;
			break;

		case 1:
			e->color.r = 255;
			e->color.g = 128;
			break;

		case 2:
			e->color.r = 255;
			e->color.g = 255;
			break;

		case 3:
			e->color.r = 255;
			e->color.g = 255;
			e->color.b = 255;
			break;

		default:
			break;
	}
}

void addSmokeEffect(int x, int y)
{
	Effect *e;

	e = malloc(sizeof(Effect));
	memset(e, 0, sizeof(Effect));
	stage.effectTail->next = e;
	stage.effectTail = e;

	e->x = x;
	e->y = y;
	e->dy = (1.0 * (-rand() % 300)) * 0.01;
	e->size = 32;
	e->life = e->alpha = 64;

	e->color.r = e->color.g = e->color.b = rand() % 255;
}

void addBulletImpactEffect(Bullet *b)
{
	Effect *e;
	int		i;

	for (i = 0; i < 8; i++)
	{
		e = malloc(sizeof(Effect));
		memset(e, 0, sizeof(Effect));
		stage.effectTail->next = e;
		stage.effectTail = e;

		e->x = b->x + (b->texture->rect.w / 2);
		e->y = b->y + (b->texture->rect.h / 2);
		e->dx = (1.0 * (rand() % 100) - (rand() % 100)) * 0.01;
		e->dy = (1.0 * (rand() % 100) - (rand() % 100)) * 0.01;
		e->size = 8;
		e->life = FPS / 2;
		e->alpha = 255;

		if (b->owner == stage.player)
		{
			e->color.r = 128;
			e->color.g = 128;
			e->color.b = 255;
		}
		else
		{
			e->color.r = 255;
			e->color.g = 128;
			e->color.b = 128;
		}
	}
}

void addDroneEngineEffect(int x, int y, double dy)
{
	Effect *e;

	e = malloc(sizeof(Effect));
	memset(e, 0, sizeof(Effect));
	stage.effectTail->next = e;
	stage.effectTail = e;

	e->x = x;
	e->y = y;
	e->dy = dy + 2;
	e->size = 24;
	e->life = FPS;
	e->alpha = 32;

	e->color.r = 255;
	e->color.g = 0;
	e->color.b = 255;
}

void clearEffects(void)
{
	Effect *e;

	while (stage.effectHead.next != NULL)
	{
		e = stage.effectHead.next;

		stage.effectHead.next = e->next;

		free(e);
	}
}

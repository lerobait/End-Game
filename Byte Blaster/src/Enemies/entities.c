/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/draw.h"
#include "entities.h"

extern App	 app;
extern Stage stage;

static Entity deadHead, *deadTail;

void initEntities(void)
{
	memset(&stage.entityHead, 0, sizeof(Entity));
	stage.entityTail = &stage.entityHead;

	memset(&deadHead, 0, sizeof(Entity));
	deadTail = &deadHead;
}

void doEntities(void)
{
	Entity *e, *prev;

	prev = &stage.entityHead;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		e->onGround = 0;

		if (!(e->flags & EF_WEIGHTLESS))
		{
			e->dy = MIN(e->dy + (GRAVITY * app.deltaTime), MAX_FALL_SPEED);
		}

		e->x += (e->dx * app.deltaTime);
		e->y += (e->dy * app.deltaTime);

		e->x = MIN(MAX(e->x, 10), SCREEN_WIDTH - (e->texture->rect.w + 10));

		if (e->y + e->texture->rect.h >= 430)
		{
			e->y = 430 - e->texture->rect.h;
			e->dy = 0;

			e->onGround = 1;
		}

		e->tick(e);

		if (e->dead)
		{
			prev->next = e->next;

			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			deadTail->next = e;
			deadTail = e;
			deadTail->next = NULL;

			e = prev;
		}

		prev = e;
	}
}

void drawEntities(void)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		e->draw(e);
	}

	if (app.dev.showHitboxes)
	{
		for (e = stage.entityHead.next; e != NULL; e = e->next)
		{
			drawOutlineRect(e->hitbox.x, e->hitbox.y, e->hitbox.w, e->hitbox.h, 255, 255, 0, 255);
		}
	}
}

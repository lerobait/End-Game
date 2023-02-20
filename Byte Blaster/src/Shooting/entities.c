/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "entities.h"

extern App	 app;
extern Stage stage;

void initEntities(void)
{
	memset(&stage.entityHead, 0, sizeof(Entity));
	stage.entityTail = &stage.entityHead;
}

void doEntities(void)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		e->onGround = 0;

		e->dy = MIN(e->dy + (GRAVITY * app.deltaTime), MAX_FALL_SPEED);

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
	}
}

void drawEntities(void)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		e->draw(e);
	}
}

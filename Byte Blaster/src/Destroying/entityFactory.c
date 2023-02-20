/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../entities/player.h"
#include "../entities/target.h"
#include "entityFactory.h"

extern Stage stage;

static InitFunc *getInitFunc(char *name);
static void		 addInitFunc(char *name, void (*init)(Entity *e));

static InitFunc head, *tail;

void initEntityFactory(void)
{
	memset(&head, 0, sizeof(InitFunc));
	tail = &head;

	addInitFunc("player", initPlayer);
	addInitFunc("target", initTarget);
}

Entity *initEntity(char *name)
{
	Entity *e;

	e = spawnEntity();

	getInitFunc(name)->init(e);

	return e;
}

Entity *spawnEntity(void)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.entityTail->next = e;
	stage.entityTail = e;

	return e;
}

static void addInitFunc(char *name, void (*init)(Entity *e))
{
	InitFunc *i;

	i = malloc(sizeof(InitFunc));
	memset(i, 0, sizeof(InitFunc));
	tail->next = i;
	tail = i;

	STRCPY(i->name, name);
	i->init = init;
}

static InitFunc *getInitFunc(char *name)
{
	InitFunc *i;

	for (i = head.next; i != NULL; i = i->next)
	{
		if (strcmp(i->name, name) == 0)
		{
			return i;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Unknown entity '%s'", name);
	exit(1);

	return NULL;
}

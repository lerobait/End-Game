/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/util.h"
#include "quadtree.h"

#define MAX_DEPTH			5
#define QT_INITIAL_CAPACITY 8

extern Stage stage;

static int	getIndex(Quadtree *root, int x, int y, int w, int h);
static void removeEntity(Entity *e, Quadtree *root);
static int	entityComparator(const void *a, const void *b);
static void getAllEntsWithinNode(int x, int y, int w, int h, Entity **candidates, Entity *ignore, Quadtree *root);
static void destroyQuadtreeNode(Quadtree *root);
static void resizeQTEntCapacity(Quadtree *root);

static int cIndex;
static int numCells;

void initQuadtree(Quadtree *root)
{
	Quadtree *node;
	int		  i, w, h;

	/* entire map */
	if (root->depth == 0)
	{
		root->w = MAP_WIDTH * MAP_TILE_SIZE;
		root->h = MAP_HEIGHT * MAP_TILE_SIZE;
		root->capacity = QT_INITIAL_CAPACITY;
		root->ents = malloc(sizeof(Entity *) * root->capacity);
		memset(root->ents, 0, sizeof(Entity *) * root->capacity);

		numCells = 1;
	}

	if (root->depth < MAX_DEPTH)
	{
		w = root->w / 2;
		h = root->h / 2;

		for (i = 0; i < 4; i++)
		{
			node = malloc(sizeof(Quadtree));
			memset(node, 0, sizeof(Quadtree));
			root->node[i] = node;

			node->depth = root->depth + 1;
			node->capacity = QT_INITIAL_CAPACITY;
			node->ents = malloc(sizeof(Entity *) * node->capacity);
			memset(node->ents, 0, sizeof(Entity *) * node->capacity);

			node->w = w;
			node->h = h;

			switch (i)
			{
				case 0:
					node->x = root->x;
					node->y = root->y;
					break;

				case 1:
					node->x = root->x + w;
					node->y = root->y;
					break;

				case 2:
					node->x = root->x;
					node->y = root->y + h;
					break;

				default:
					node->x = root->x + w;
					node->y = root->y + h;
					break;
			}

			numCells++;

			initQuadtree(node);
		}
	}

	if (root->depth == 0)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Quadtree: [numCells = %d, memory = %.2fkb]", numCells, (double)((sizeof(Quadtree) * numCells) / 1024.0));
	}
}

void addToQuadtree(Entity *e, Quadtree *root)
{
	int index;

	root->addedTo = 1;

	if (root->node[0] != NULL)
	{
		index = getIndex(root, e->x, e->y, e->texture->rect.w, e->texture->rect.h);

		if (index != -1)
		{
			addToQuadtree(e, root->node[index]);
			return;
		}
	}

	if (root->numEnts == root->capacity)
	{
		resizeQTEntCapacity(root);
	}

	root->ents[root->numEnts++] = e;
}

static void resizeQTEntCapacity(Quadtree *root)
{
	int n;

	n = root->capacity + QT_INITIAL_CAPACITY;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Resizing QT node: %d -> %d", root->capacity, n);

	root->ents = resize(root->ents, sizeof(Entity *) * root->capacity, sizeof(Entity *) * n);
	root->capacity = n;
}

static int getIndex(Quadtree *root, int x, int y, int w, int h)
{
	int verticalMidpoint, horizontalMidpoint, topHalf, bottomHalf;

	verticalMidpoint = root->x + (root->w / 2);
	horizontalMidpoint = root->y + (root->h / 2);
	topHalf = (y < horizontalMidpoint && y + h < horizontalMidpoint);
	bottomHalf = (y > horizontalMidpoint);

	if (x < verticalMidpoint && x + w < verticalMidpoint)
	{
		if (topHalf)
		{
			return 0;
		}
		else if (bottomHalf)
		{
			return 2;
		}
	}
	else if (x > verticalMidpoint)
	{
		if (topHalf)
		{
			return 1;
		}
		else if (bottomHalf)
		{
			return 3;
		}
	}

	return -1;
}

void removeFromQuadtree(Entity *e, Quadtree *root)
{
	int index;

	if (root->addedTo)
	{
		if (root->node[0] != NULL)
		{
			index = getIndex(root, e->x, e->y, e->texture->rect.w, e->texture->rect.h);

			if (index != -1)
			{
				removeFromQuadtree(e, root->node[index]);
				return;
			}
		}

		removeEntity(e, root);

		if (root->numEnts == 0)
		{
			root->addedTo = 0;

			if (root->node[0] != NULL)
			{
				root->addedTo = root->node[0]->addedTo || root->node[1]->addedTo || root->node[2]->addedTo || root->node[3]->addedTo;
			}
		}
	}
}

static void removeEntity(Entity *e, Quadtree *root)
{
	int i, n;

	n = root->numEnts;

	for (i = 0; i < n; i++)
	{
		if (root->ents[i] == e)
		{
			root->ents[i] = NULL;
			root->numEnts--;
		}
	}

	qsort(root->ents, n, sizeof(Entity *), entityComparator);
}

void getAllEntsWithin(int x, int y, int w, int h, Entity **candidates, Entity *ignore)
{
	cIndex = 0;

	memset(candidates, 0, sizeof(Entity *) * MAX_QT_CANDIDATES);

	getAllEntsWithinNode(x, y, w, h, candidates, ignore, &stage.quadtree);
}

static void getAllEntsWithinNode(int x, int y, int w, int h, Entity **candidates, Entity *ignore, Quadtree *root)
{
	int index, i;

	if (root->addedTo)
	{
		if (root->node[0] != NULL)
		{
			index = getIndex(root, x, y, w, h);

			if (index != -1)
			{
				getAllEntsWithinNode(x, y, w, h, candidates, ignore, root->node[index]);
			}
			else
			{
				for (i = 0; i < 4; i++)
				{
					getAllEntsWithinNode(x, y, w, h, candidates, ignore, root->node[i]);
				}
			}
		}

		for (i = 0; i < root->numEnts; i++)
		{
			if (cIndex < MAX_QT_CANDIDATES)
			{
				if (root->ents[i] != ignore)
				{
					candidates[cIndex++] = root->ents[i];
				}
			}
			else
			{
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of quadtree candidate space (%d)", MAX_QT_CANDIDATES);
				exit(1);
			}
		}
	}
}

void destroyQuadtree(Quadtree *root)
{
	destroyQuadtreeNode(root);
}

static void destroyQuadtreeNode(Quadtree *root)
{
	int i;

	free(root->ents);

	root->ents = NULL;

	if (root->node[0] != NULL)
	{
		for (i = 0; i < 4; i++)
		{
			destroyQuadtreeNode(root->node[i]);

			free(root->node[i]);

			root->node[i] = NULL;
		}
	}
}

static int entityComparator(const void *a, const void *b)
{
	Entity *e1 = *((Entity **)a);
	Entity *e2 = *((Entity **)b);

	if (e1 == NULL)
	{
		return 1;
	}
	else if (e2 == NULL)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

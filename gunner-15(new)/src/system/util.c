/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "util.h"

unsigned long hashcode(const char *str)
{
	unsigned long hash = 5381;
	int			  c;

	c = *str;

	while (c)
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		c = *str++;
	}

	hash = ((hash << 5) + hash);

	return hash;
}

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

int collisionRects(SDL_Rect *r1, SDL_Rect *r2)
{
	return collision(r1->x, r1->y, r1->w, r1->h, r2->x, r2->y, r2->w, r2->h);
}

int lineLineCollision(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	float denominator, numerator1, numerator2, r, s;

	denominator = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));

	if (denominator == 0)
	{
		return 0;
	}

	numerator1 = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
	numerator2 = ((y1 - y3) * (x2 - x1)) - ((x1 - x3) * (y2 - y1));

	r = numerator1 / denominator;
	s = numerator2 / denominator;

	return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

int lineRectCollision(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh)
{
	return
		// top
		lineLineCollision(x1, y1, x2, y2, rx, ry, rx + rw, ry) ||

		// bottom
		lineLineCollision(x1, y1, x2, y2, rx, ry + rh, rx + rw, ry + rh) ||

		// left
		lineLineCollision(x1, y1, x2, y2, rx, ry, rx, ry + rh) ||

		// right
		lineLineCollision(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh);
}

int getDistance(int x1, int y1, int x2, int y2)
{
	int x, y;

	x = x2 - x1;
	y = y2 - y1;

	return sqrt(x * x + y * y);
}

double getAngle(int x1, int y1, int x2, int y2)
{
	double angle;

	angle = -90 + atan2(y1 - y2, x1 - x2) * (180 / PI);

	return angle >= 0 ? angle : 360 + angle;
}

char *readFile(char *filename)
{
	char *buffer;
	long  length;
	FILE *file;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading file '%s'", filename);

	file = fopen(filename, "rb");

	buffer = NULL;

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = malloc(length);
		memset(buffer, 0, length);
		fread(buffer, 1, length, file);

		fclose(file);
	}
	else
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "No such file '%s'", filename);
	}

	return buffer;
}

int writeFile(const char *filename, const char *data)
{
	FILE *file;

	file = fopen(filename, "wb");

	if (file)
	{
		fprintf(file, "%s\n", data);
		fclose(file);
		return 1;
	}

	return 0;
}

void calcSlope(int x1, int y1, int x2, int y2, double *dx, double *dy)
{
	int steps = MAX(abs(x1 - x2), abs(y1 - y2));

	if (steps == 0)
	{
		*dx = *dy = 0;
		return;
	}

	*dx = (x1 - x2);
	*dx /= steps;

	*dy = (y1 - y2);
	*dy /= steps;
}

void *resize(void *array, int oldSize, int newSize)
{
	void **newArray;
	int	   copySize;

	copySize = newSize > oldSize ? oldSize : newSize;

	newArray = malloc(newSize);
	memset(newArray, 0, newSize);
	memcpy(newArray, array, copySize);
	free(array);

	return newArray;
}

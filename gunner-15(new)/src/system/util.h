/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

void		 *resize(void *array, int oldSize, int newSize);
void		  calcSlope(int x1, int y1, int x2, int y2, double *dx, double *dy);
int			  writeFile(const char *filename, const char *data);
char		 *readFile(char *filename);
double		  getAngle(int x1, int y1, int x2, int y2);
int			  getDistance(int x1, int y1, int x2, int y2);
int			  lineRectCollision(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh);
int			  lineLineCollision(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int			  collisionRects(SDL_Rect *r1, SDL_Rect *r2);
int			  collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
unsigned long hashcode(const char *str);

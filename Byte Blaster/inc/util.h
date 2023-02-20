/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

void		  calcSlope(int x1, int y1, int x2, int y2, double *dx, double *dy);
int			  writeFile(const char *filename, const char *data);
char		 *readFile(char *filename);
int			  getDistance(int x1, int y1, int x2, int y2);
int			  collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
unsigned long hashcode(const char *str);

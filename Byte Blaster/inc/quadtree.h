/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

void destroyQuadtree(Quadtree *root);
void getAllEntsWithin(int x, int y, int w, int h, Entity **candidates, Entity *ignore);
void removeFromQuadtree(Entity *e, Quadtree *root);
void addToQuadtree(Entity *e, Quadtree *root);
void initQuadtree(Quadtree *root);

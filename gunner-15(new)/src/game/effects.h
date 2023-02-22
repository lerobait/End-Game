/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

void clearEffects(void);
void addDroneEngineEffect(int x, int y, double dy);
void addBulletImpactEffect(Bullet *b);
void addSmokeEffect(int x, int y);
void addExplosionEffect(int x, int y, int size);
void drawEffects(void);
void doEffects(void);
void initEffects(void);

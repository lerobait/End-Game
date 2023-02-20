/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
void blit(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip);
void presentScene(void);
void prepareScene(void);

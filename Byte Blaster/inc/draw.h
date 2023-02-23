
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stdio.h>

#include <stdlib.h>
void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
void blitRotated(AtlasImage *atlasImage, int x, int y, float angle);
void blitAtlasImageScaled(AtlasImage *atlasImage, int x, int y, int w, int h, int center);
void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
void blit(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip);
void presentScene(void);
void prepareScene(void);

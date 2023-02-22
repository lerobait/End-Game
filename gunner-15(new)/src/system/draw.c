/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "draw.h"

extern App app;

void prepareScene(void)
{
	SDL_Surface* image = IMG_Load("./gfx/back.png");
    	SDL_Texture* texture = SDL_CreateTextureFromSurface(app.renderer, image);
    	SDL_FreeSurface(image);
    	SDL_RenderCopy(app.renderer, texture, NULL, NULL);
    	SDL_SetRenderDrawColor(app.renderer, 16, 32, 48, 255);
    	SDL_DestroyTexture(texture);
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

void blit(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dest, 0, NULL, flip);
}

void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = atlasImage->rect.w;
	dest.h = atlasImage->rect.h;

	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopyEx(app.renderer, atlasImage->texture, &atlasImage->rect, &dest, 0, NULL, flip);
}

void blitAtlasImageScaled(AtlasImage *atlasImage, int x, int y, int w, int h, int center)
{
	SDL_Rect dstRect;

	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = w;
	dstRect.h = h;

	if (center)
	{
		dstRect.x -= (dstRect.w / 2);
		dstRect.y -= (dstRect.h / 2);
	}

	SDL_RenderCopy(app.renderer, atlasImage->texture, &atlasImage->rect, &dstRect);
}

void blitRotated(AtlasImage *atlasImage, int x, int y, float angle)
{
	SDL_Rect dstRect;

	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = atlasImage->rect.w;
	dstRect.h = atlasImage->rect.h;

	dstRect.x -= (dstRect.w / 2);
	dstRect.y -= (dstRect.h / 2);

	SDL_RenderCopyEx(app.renderer, atlasImage->texture, &atlasImage->rect, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

void drawRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderFillRect(app.renderer, &rect);
}

void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawRect(app.renderer, &rect);
}

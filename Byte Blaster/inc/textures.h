/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

SDL_Texture *loadTexture(char *filename);
SDL_Texture *toTexture(SDL_Surface *surface, int destroySurface);
void		 initTextures(void);

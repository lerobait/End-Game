/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

int			 getWrappedTextHeight(char *text, int maxWidth);
void		 calcTextDimensions(char *text, int *w, int *h);
void		 drawText(char *text, int x, int y, int r, int g, int b, int align, int maxWidth);
SDL_Texture *getTextTexture(char *text, int type);
void		 initFonts(void);

/*
 * Copyright (C) 2021-2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../json/cJSON.h"
#include "../system/draw.h"
#include "../system/sound.h"
#include "../system/text.h"
#include "../system/util.h"
#include "widgets.h"

extern App app;

static Widget  widgetHead;
static Widget *widgetTail;
static void	   createWidget(cJSON *root);
static int	   getWidgetType(char *type);
static void	   createButtonWidget(Widget *w, cJSON *root);
static void	   createSelectWidget(Widget *w, cJSON *root);
static void	   createSliderWidget(Widget *w, cJSON *root);
static void	   createInputWidget(Widget *w, cJSON *root);
static void	   createControlWidget(Widget *w, cJSON *root);
static void	   doInputWidget(void);
static void	   doControlWidget(void);
static void	   drawButtonWidget(Widget *w);
static void	   drawSelectWidget(Widget *w);
static void	   drawSliderWidget(Widget *w);
static void	   drawInputWidget(Widget *w);
static void	   drawControlWidget(Widget *w);
static void	   changeWidgetValue(int dir);
static void	   loadWidgets(char *filename);

static double sliderDelay;
static double cursorBlink;
static int	  handleInputWidget;
static int	  handleControlWidget;

void initWidgets(void)
{
	memset(&widgetHead, 0, sizeof(Widget));
	widgetTail = &widgetHead;

	loadWidgets("data/widgets/title.json");
	loadWidgets("data/widgets/pause.json");
	loadWidgets("data/widgets/options.json");
	loadWidgets("data/widgets/controls.json");

	sliderDelay = 0;

	cursorBlink = 0;

	handleInputWidget = 0;

	handleControlWidget = 0;
}

void doWidgets(char *groupName)
{
	sliderDelay = MAX(sliderDelay - app.deltaTime, 0);

	cursorBlink += app.deltaTime;

	if (!handleInputWidget && !handleControlWidget)
	{
		if (app.keyboard[SDL_SCANCODE_UP])
		{
			app.keyboard[SDL_SCANCODE_UP] = 0;

			cursorBlink = 0;

			do
			{
				app.activeWidget = app.activeWidget->prev;

				if (app.activeWidget == &widgetHead)
				{
					app.activeWidget = widgetTail;
				}
			} while (strcmp(app.activeWidget->groupName, groupName) != 0);

			playSound(SND_GUI, 0);
		}

		if (app.keyboard[SDL_SCANCODE_DOWN])
		{
			app.keyboard[SDL_SCANCODE_DOWN] = 0;

			cursorBlink = 0;

			do
			{
				app.activeWidget = app.activeWidget->next;

				if (app.activeWidget == NULL)
				{
					app.activeWidget = widgetHead.next;
				}
			} while (strcmp(app.activeWidget->groupName, groupName) != 0);

			playSound(SND_GUI, 0);
		}

		if (app.keyboard[SDL_SCANCODE_LEFT])
		{
			changeWidgetValue(-1);
		}

		if (app.keyboard[SDL_SCANCODE_RIGHT])
		{
			changeWidgetValue(1);
		}

		if (app.keyboard[SDL_SCANCODE_SPACE] || app.keyboard[SDL_SCANCODE_RETURN])
		{
			app.keyboard[SDL_SCANCODE_SPACE] = app.keyboard[SDL_SCANCODE_RETURN] = 0;

			if (app.activeWidget->type == WT_INPUT)
			{
				cursorBlink = 0;

				handleInputWidget = 1;

				memset(app.inputText, 0, sizeof(app.inputText));
			}
			else if (app.activeWidget->type == WT_CONTROL)
			{
				app.lastKeyPressed = -1;

				app.lastButtonPressed = -1;

				handleControlWidget = 1;
			}
			else if (app.activeWidget->action != NULL)
			{
				app.activeWidget->action();
			}

			playSound(SND_GUI, 0);
		}
	}
	else if (handleInputWidget)
	{
		doInputWidget();
	}
	else if (handleControlWidget)
	{
		doControlWidget();
	}
}

static void changeWidgetValue(int dir)
{
	SelectWidget *select;
	SliderWidget *slider;

	switch (app.activeWidget->type)
	{
		case WT_SELECT:
			app.keyboard[SDL_SCANCODE_LEFT] = app.keyboard[SDL_SCANCODE_RIGHT] = 0;

			select = (SelectWidget *)app.activeWidget->data;

			select->value += dir;

			if (select->value < 0)
			{
				select->value = select->numOptions - 1;
			}

			if (select->value >= select->numOptions)
			{
				select->value = 0;
			}

			if (app.activeWidget->action != NULL)
			{
				app.activeWidget->action();
			}

			playSound(SND_GUI, 0);

			break;

		case WT_SLIDER:
			slider = (SliderWidget *)app.activeWidget->data;

			if (sliderDelay == 0 || slider->waitOnChange)
			{
				if (slider->waitOnChange)
				{
					app.keyboard[SDL_SCANCODE_LEFT] = app.keyboard[SDL_SCANCODE_RIGHT] = 0;

					playSound(SND_GUI, 0);
				}

				slider->value = MIN(MAX(slider->value + (slider->step * dir), 0), 100);

				sliderDelay = 1;

				if (app.activeWidget->action != NULL)
				{
					app.activeWidget->action();
				}
			}
			break;

		default:
			break;
	}
}

static void doInputWidget(void)
{
	InputWidget *iw;
	int			 i, l, n;

	iw = (InputWidget *)app.activeWidget->data;

	l = strlen(iw->text);
	n = strlen(app.inputText);

	if (n + l > iw->maxLength)
	{
		n = iw->maxLength - l;
	}

	for (i = 0; i < n; i++)
	{
		iw->text[l++] = app.inputText[i];
	}

	memset(app.inputText, 0, sizeof(app.inputText));

	if (l > 0 && app.keyboard[SDL_SCANCODE_BACKSPACE])
	{
		iw->text[--l] = '\0';

		app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;
	}

	if (app.keyboard[SDL_SCANCODE_RETURN] || app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_RETURN] = app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		handleInputWidget = 0;

		if (app.activeWidget->action != NULL)
		{
			app.activeWidget->action();
		}

		playSound(SND_GUI, 0);
	}
}

static void doControlWidget(void)
{
	ControlWidget *cw;

	if (app.lastKeyPressed != -1)
	{
		cw = (ControlWidget *)app.activeWidget->data;

		if (app.lastKeyPressed == SDL_SCANCODE_BACKSPACE)
		{
			cw->keyboard = 0;

		}
		else if (app.lastKeyPressed != SDL_SCANCODE_ESCAPE)
		{
			if (app.lastKeyPressed != -1)
			{
				cw->keyboard = app.lastKeyPressed;

				app.keyboard[app.lastKeyPressed] = 0;
			}

			if (app.activeWidget->action != NULL)
			{
				app.activeWidget->action();
			}
		}

		handleControlWidget = 0;

		playSound(SND_GUI, 0);
	}
}

void drawWidgets(char *groupName)
{
	Widget *w;
	int		h;

	for (w = widgetHead.next; w != NULL; w = w->next)
	{
		if (strcmp(w->groupName, groupName) == 0)
		{
			switch (w->type)
			{
				case WT_BUTTON:
					drawButtonWidget(w);
					break;

				case WT_SELECT:
					drawSelectWidget(w);
					break;

				case WT_SLIDER:
					drawSliderWidget(w);
					break;

				case WT_INPUT:
					drawInputWidget(w);
					break;

				case WT_CONTROL:
					drawControlWidget(w);
					break;

				default:
					break;
			}

			if (w == app.activeWidget && ((int)cursorBlink % (int)FPS < (FPS / 2)))
			{
				h = w->h / 2;

				drawRect(w->x - (h * 2), w->y + (h / 2), h, h, 0, 255, 0, 255);
			}
		}
	}
}

static void drawButtonWidget(Widget *w)
{
	SDL_Color c;

	if (w == app.activeWidget)
	{
		c.g = 255;
		c.r = c.b = 0;
	}
	else
	{
		c.r = c.g = c.b = 255;
	}

	drawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
}

static void drawSelectWidget(Widget *w)
{
	SDL_Color	  c;
	char		  text[128];
	SelectWidget *s;

	s = (SelectWidget *)w->data;

	if (w == app.activeWidget)
	{
		c.g = 255;
		c.r = c.b = 0;
	}
	else
	{
		c.r = c.g = c.b = 255;
	}

	drawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "< %s >", s->options[s->value]);

	drawText(text, s->x, s->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
}

static void drawSliderWidget(Widget *w)
{
	SDL_Color	  c;
	SliderWidget *s;
	double		  width;

	s = (SliderWidget *)w->data;

	if (w == app.activeWidget)
	{
		c.g = 255;
		c.r = c.b = 0;
	}
	else
	{
		c.r = c.g = c.b = 255;
	}

	width = (1.0 * s->value) / 100;

	drawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

	drawRect(s->x + 2, s->y + 2, (s->w - 4) * width, s->h - 4, c.r, c.g, c.b, 255);

	drawOutlineRect(s->x, s->y, s->w, s->h, 255, 255, 255, 255);
}

static void drawInputWidget(Widget *w)
{
	SDL_Color	 c;
	InputWidget *i;
	int			 width, height;

	i = (InputWidget *)w->data;

	if (w == app.activeWidget)
	{
		c.g = 255;
		c.r = c.b = 0;
	}
	else
	{
		c.r = c.g = c.b = 255;
	}

	drawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

	drawText(i->text, i->x, i->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

	if (handleInputWidget && app.activeWidget == w && ((int)cursorBlink % (int)FPS < (FPS / 2)))
	{
		calcTextDimensions(i->text, &width, &height);

		drawRect(i->x + width + 4, i->y + 14, 32, 32, 0, 255, 0, 255);
	}
}

static void drawControlWidget(Widget *w)
{
	SDL_Color	   c;
	ControlWidget *cw;
	char		   text[32];

	cw = (ControlWidget *)w->data;

	if (w == app.activeWidget)
	{
		c.g = 255;
		c.r = c.b = 0;
	}
	else
	{
		c.r = c.g = c.b = 255;
	}

	drawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

	if (handleControlWidget && app.activeWidget == w)
	{
		drawText("...", cw->x, cw->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
	}
	else
	{
		if (cw->keyboard != 0)
		{
			sprintf(text, "%s", SDL_GetScancodeName(cw->keyboard));
		}
		else if (cw->keyboard != 0)
		{
			sprintf(text, "%s", SDL_GetScancodeName(cw->keyboard));
		}
		else
		{
			sprintf(text, "N/A");
		}

		drawText(text, cw->x, cw->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
	}
}

Widget *getWidget(char *name, char *groupName)
{
	Widget *w;

	for (w = widgetHead.next; w != NULL; w = w->next)
	{
		if (strcmp(w->name, name) == 0 && strcmp(w->groupName, groupName) == 0)
		{
			return w;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "No such widget: name='%s', groupName='%s'", name, groupName);

	return NULL;
}

static void loadWidgets(char *filename)
{
	cJSON *root, *node;
	char	 *text;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ...", filename);

	text = readFile(filename);

	root = cJSON_Parse(text);

	for (node = root->child; node != NULL; node = node->next)
	{
		createWidget(node);
	}

	cJSON_Delete(root);

	free(text);
}

static void createWidget(cJSON *root)
{
	Widget *w;
	int		type;

	type = getWidgetType(cJSON_GetObjectItem(root, "type")->valuestring);

	if (type != -1)
	{
		w = malloc(sizeof(Widget));
		memset(w, 0, sizeof(Widget));
		widgetTail->next = w;
		w->prev = widgetTail;
		widgetTail = w;

		STRCPY(w->name, cJSON_GetObjectItem(root, "name")->valuestring);
		STRCPY(w->groupName, cJSON_GetObjectItem(root, "groupName")->valuestring);
		STRCPY(w->label, cJSON_GetObjectItem(root, "label")->valuestring);
		w->type = getWidgetType(cJSON_GetObjectItem(root, "type")->valuestring);
		w->x = cJSON_GetObjectItem(root, "x")->valueint;
		w->y = cJSON_GetObjectItem(root, "y")->valueint;

		switch (w->type)
		{
			case WT_BUTTON:
				createButtonWidget(w, root);
				break;

			case WT_SELECT:
				createSelectWidget(w, root);
				break;

			case WT_SLIDER:
				createSliderWidget(w, root);
				break;

			case WT_INPUT:
				createInputWidget(w, root);
				break;

			case WT_CONTROL:
				createControlWidget(w, root);
				break;

			default:
				break;
		}
	}
}

static void createButtonWidget(Widget *w, cJSON *root)
{
	calcTextDimensions(w->label, &w->w, &w->h);
}

static void createSelectWidget(Widget *w, cJSON *root)
{
	cJSON		  *options, *node;
	int			  i, len;
	SelectWidget *s;

	s = malloc(sizeof(SelectWidget));
	memset(s, 0, sizeof(SelectWidget));
	w->data = s;

	options = cJSON_GetObjectItem(root, "options");

	s->numOptions = cJSON_GetArraySize(options);

	if (s->numOptions > 0)
	{
		i = 0;

		s->options = malloc(sizeof(char *) * s->numOptions);

		for (node = options->child; node != NULL; node = node->next)
		{
			len = strlen(node->valuestring) + 1;

			s->options[i] = malloc(len);

			STRNCPY(s->options[i], node->valuestring, len);

			i++;
		}
	}

	calcTextDimensions(w->label, &w->w, &w->h);

	s->x = w->x + w->w + 50;
	s->y = w->y;
}

static void createSliderWidget(Widget *w, cJSON *root)
{
	SliderWidget *s;

	s = malloc(sizeof(SliderWidget));
	memset(s, 0, sizeof(SliderWidget));
	w->data = s;

	s->step = cJSON_GetObjectItem(root, "step")->valueint;
	s->waitOnChange = cJSON_GetObjectItem(root, "waitOnChange")->valueint;

	calcTextDimensions(w->label, &w->w, &w->h);

	s->x = w->x + w->w + 50;
	s->y = w->y;
	s->w = w->w;
	s->h = w->h;
}

static void createInputWidget(Widget *w, cJSON *root)
{
	InputWidget *i;

	i = malloc(sizeof(InputWidget));
	memset(i, 0, sizeof(InputWidget));
	w->data = i;

	i->maxLength = cJSON_GetObjectItem(root, "maxLength")->valueint;
	i->text = malloc(i->maxLength + 1);
	memset(i->text, 0, i->maxLength + 1);

	calcTextDimensions(w->label, &w->w, &w->h);

	i->x = w->x + w->w + 50;
}

static void createControlWidget(Widget *w, cJSON *root)
{
	ControlWidget *c;

	c = malloc(sizeof(ControlWidget));
	memset(c, 0, sizeof(ControlWidget));
	w->data = c;

	calcTextDimensions(w->label, &w->w, &w->h);
}

static int getWidgetType(char *type)
{
	if (strcmp(type, "WT_BUTTON") == 0)
	{
		return WT_BUTTON;
	}
	else if (strcmp(type, "WT_SELECT") == 0)
	{
		return WT_SELECT;
	}
	else if (strcmp(type, "WT_SLIDER") == 0)
	{
		return WT_SLIDER;
	}
	else if (strcmp(type, "WT_INPUT") == 0)
	{
		return WT_INPUT;
	}
	else if (strcmp(type, "WT_CONTROL") == 0)
	{
		return WT_CONTROL;
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Unknown widget type: '%s'", type);

	return -1;
}

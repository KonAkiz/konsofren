/* SPDX-License-Identifier: MIT */
/*
 * kondisplay.h
 *
 * Copyright (c) 2026 KonAki
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef KONDISPLAY_H
#define KONDISPLAY_H

#include <stdint.h>
#include <stdlib.h>

#include "konsofren.h"

/*** display declarations ***/

/* game is the fixed-size framebuffer you draw into, screen always matches the window */
typedef struct kon_display {
	kon_framebuffer_t *game;
	kon_framebuffer_t *screen;
	int integerOnly;

	/* where the game image sits inside screen, kept up to date by the functions below */
	int viewX, viewY, viewWidth, viewHeight;
	int *columnMap;
} kon_display_t;

kon_display_t *kon_createDisplay(int gameWidth, int gameHeight, int windowWidth, int windowHeight);
void kon_freeDisplay(kon_display_t *display);

void kon_setGameResolution(kon_display_t *display, int gameWidth, int gameHeight);
void kon_setIntegerScaling(kon_display_t *display, int integerOnly);
void kon_resizeDisplay(kon_display_t *display, int windowWidth, int windowHeight);

/* scales game into screen, then blit display->screen to the window */
void kon_presentDisplay(kon_display_t *display);

/* returns 0 if the window position is on a bar, outside the game image */
int kon_windowToGame(const kon_display_t *display, int windowX, int windowY, int *gameX, int *gameY);

/*** implementation ***/

#ifdef KONDISPLAY_IMPLEMENTATION

/*** display implementation ***/

static void kon_updateView_(kon_display_t *display) {
	int gameWidth = display->game->width, gameHeight = display->game->height;
	int windowWidth = display->screen->width, windowHeight = display->screen->height;

	int scaleX = windowWidth / gameWidth;
	int scaleY = windowHeight / gameHeight;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;

	if (display->integerOnly && scale >= 1) {
		display->viewWidth  = gameWidth  * scale;
		display->viewHeight = gameHeight * scale;
	} else if ((int64_t)windowWidth * gameHeight <= (int64_t)windowHeight * gameWidth) {
		/* window is narrower than the game: bars on top and bottom */
		display->viewWidth  = windowWidth;
		display->viewHeight = (int)((int64_t)windowWidth * gameHeight / gameWidth);
	} else {
		/* window is wider than the game: bars on the sides */
		display->viewHeight = windowHeight;
		display->viewWidth  = (int)((int64_t)windowHeight * gameWidth / gameHeight);
	}

	display->viewX = (windowWidth  - display->viewWidth)  / 2;
	display->viewY = (windowHeight - display->viewHeight) / 2;

	/* which game column each view column shows, worked out once instead of for every pixel */
	int *map = realloc(display->columnMap, (size_t)windowWidth * sizeof(int));
	if (!map) {
		display->viewWidth = 0;
		return;
	}
	display->columnMap = map;

	for (int x = 0; x < display->viewWidth; x++) {
		map[x] = (int)((int64_t)x * gameWidth / display->viewWidth);
	}
}

kon_display_t *kon_createDisplay(int gameWidth, int gameHeight, int windowWidth, int windowHeight) {
	if (gameWidth <= 0 || gameHeight <= 0 || windowWidth <= 0 || windowHeight <= 0) return NULL;

	kon_display_t *display = calloc(1, sizeof(kon_display_t));
	if (!display) return NULL;

	display->game   = kon_createFramebuffer(gameWidth, gameHeight);
	display->screen = kon_createFramebuffer(windowWidth, windowHeight);
	if (!display->game || !display->screen) {
		kon_freeDisplay(display);
		return NULL;
	}

	kon_clearFramebuffer(display->game, KON_BACKGROUND_COLOR);
	kon_updateView_(display);
	return display;
}

void kon_freeDisplay(kon_display_t *display) {
	if (!display) return;

	kon_freeFramebuffer(display->game);
	kon_freeFramebuffer(display->screen);
	free(display->columnMap);
	free(display);
}

void kon_setGameResolution(kon_display_t *display, int gameWidth, int gameHeight) {
	if (!display || gameWidth <= 0 || gameHeight <= 0) return;

	kon_resizeFramebuffer(display->game, gameWidth, gameHeight);
	kon_updateView_(display);
}

void kon_setIntegerScaling(kon_display_t *display, int integerOnly) {
	if (!display) return;

	display->integerOnly = integerOnly;
	kon_updateView_(display);
}

void kon_resizeDisplay(kon_display_t *display, int windowWidth, int windowHeight) {
	if (!display || windowWidth <= 0 || windowHeight <= 0) return;

	kon_resizeFramebuffer(display->screen, windowWidth, windowHeight);
	kon_updateView_(display);
}

void kon_presentDisplay(kon_display_t *display) {
	if (!display) return;

	const uint32_t barColor = 0xFF000000;
	kon_framebuffer_t *game = display->game;
	kon_framebuffer_t *screen = display->screen;
	int rightX = display->viewX + display->viewWidth;

	for (int y = 0; y < screen->height; y++) {
		uint32_t *row = &screen->data[(size_t)y * screen->width];

		if (display->viewWidth <= 0 || y < display->viewY || y >= display->viewY + display->viewHeight) {
			for (int x = 0; x < screen->width; x++) row[x] = barColor;
			continue;
		}

		int srcY = (int)((int64_t)(y - display->viewY) * game->height / display->viewHeight);
		const uint32_t *srcRow = &game->data[(size_t)srcY * game->width];

		for (int x = 0; x < display->viewX; x++) row[x] = barColor;
		for (int x = 0; x < display->viewWidth; x++) row[display->viewX + x] = srcRow[display->columnMap[x]];
		for (int x = rightX; x < screen->width; x++) row[x] = barColor;
	}
}

int kon_windowToGame(const kon_display_t *display, int windowX, int windowY, int *gameX, int *gameY) {
	if (!display || display->viewWidth <= 0 || display->viewHeight <= 0) return 0;

	int localX = windowX - display->viewX;
	int localY = windowY - display->viewY;
	if (localX < 0 || localX >= display->viewWidth || localY < 0 || localY >= display->viewHeight) return 0;

	if (gameX) *gameX = (int)((int64_t)localX * display->game->width  / display->viewWidth);
	if (gameY) *gameY = (int)((int64_t)localY * display->game->height / display->viewHeight);
	return 1;
}

#endif /* end of KONDISPLAY_IMPLEMENTATION */

#endif

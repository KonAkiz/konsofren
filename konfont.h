/* SPDX-License-Identifier: MIT */
/*
 * konfont.h
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

#ifndef KONFONT_H
#define KONFONT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "konsofren.h"

#define KON_FONT_FIRST_CHAR 32
#define KON_FONT_NUM_CHARS 95

typedef struct kon_font kon_font_t;

/*** font declarations ***/

kon_font_t *kon_loadFont(const uint8_t *ttf_data, size_t size, float pixelHeight);
void kon_freeFont(kon_font_t *font);

kon_image *kon_fontGetAtlas(kon_font_t *font);

/*** text draw declarations ***/

void kon_drawText(kon_framebuffer_t *fb, kon_font_t *font, int x, int y, const char *text, uint32_t color);
void kon_measureText(kon_font_t *font, const char *text, int *outWidth, int *outHeight);

/*** implementatoin ***/

#ifdef KONFONT_IMPLEMENTATION

#include <stb_truetype.h>

struct kon_font {
	stbtt_bakedchar chardata[KON_FONT_NUM_CHARS];
	uint8_t *atlasPixels;
	int atlasSize;
	float pixelHeight;
};

/*** private helpers ***/

static bool kon_fontBakeAtlas(kon_font_t *font, const uint8_t *ttf_data, float pixelHeight) {
	int sizes[] = {256, 512, 1024, 2048};
	
	for (int i = 0; i < 4; i++) {
		int atlasSize = sizes[i];

		uint8_t *bitmap = malloc((size_t)atlas_size * (size_t)atlas_size);
		if (!bitmap) return false;

		int result = stbtt_BakeFontBitmap(ttf_data, 0, pixelHeight, bitmap, atlasSize, atlasSize, KON_FONT_FIRST_CHAR, KON_FONT_NUM_CHARS, font->chardata);

		if (result > 0) {
			font->atlasPixels = bitmap;
			font->atlasSize = atlasSize;
			return true;
		}

		free(bitmap);
	}

	return false;
}

#endif /* KONFONT_IMPLEMENTATION */

#endif /* KONFONT_H */

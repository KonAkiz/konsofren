#ifndef KONSOFREN_H
#define KONSOFREN_H

#include <stdint.h>
#include <stdlib.h>

#define KON_BACKGROUND_COLOR 0x05050AFF

/*** framebuffer declarations ***/

typedef struct kon_framebuffer {
	int width, height;
	uint32_t *data;
	uint32_t backgroundColor;
} kon_framebuffer_t;

kon_framebuffer_t *kon_createFramebuffer(int width, int height);
void kon_freeFramebuffer(kon_framebuffer_t *fb);
void kon_putPixel(kon_framebuffer_t *fb, int x, int y, uint32_t color);
void kon_clearFramebuffer(kon_framebuffer_t *fb, uint32_t color);
void kon_resizeFramebuffer(kon_framebuffer_t *fb, int width, int height);

/*** draw function declarations ***/

void kon_drawRectangle(kon_framebuffer_t *fb, int x, int y, int width, int height, uint32_t color);

/*** implementation ***/

#ifdef KONSOFREN_IMPLEMENTATION

/*** framebuffer implementation ***/

kon_framebuffer_t *kon_createFramebuffer(int width, int height) {
	kon_framebuffer_t *fb = malloc(sizeof(kon_framebuffer_t));
	if (!fb) return NULL;

	fb->data = malloc(width * height * sizeof(uint32_t));
	if (!fb->data) {
		free(fb);
		return NULL;
	}

	fb->width = width;
	fb->height = height;
	fb->backgroundColor = KON_BACKGROUND_COLOR;

	kon_clearFramebuffer(fb, fb->backgroundColor);
	return fb;
}

void kon_freeFramebuffer(kon_framebuffer_t *fb) {
	if (!fb) return;

	free(fb->data);
	free(fb);
}

void kon_putPixel(kon_framebuffer_t *fb, int x, int y, uint32_t color) {
	if (!fb) return;

	if (x < 0 || x >= fb->width || y < 0 || y >= fb->height) return;
	fb->data[y * fb->width + x] = color;
}

void kon_clearFramebuffer(kon_framebuffer_t *fb, uint32_t color) {
	if (!fb) return;

	if (color == 0)
		color = fb->backgroundColor;

	for (int i = 0; i < fb->width * fb->height; i++) {
		/* did it directly to not check overhead because of the if in bounds check */
		fb->data[i] = color;
	}
}

void kon_resizeFramebuffer(kon_framebuffer_t *fb, int width, int height) {
	if (!fb) return;

	uint32_t *tmp = realloc(fb->data, (size_t)width * height * sizeof(uint32_t));
	if (!tmp) return;

	fb->data = tmp;
	fb->width  = width;
	fb->height = height;

	kon_clearFramebuffer(fb, fb->backgroundColor);
}

/*** draw functions implementation ***/

void kon_drawRectangle(kon_framebuffer_t *fb, int x, int y, int width, int height, uint32_t color) {
	if (!fb) return;

	if (width < 0 || height < 0) return;

	if (x < 0) {
		width += x;
		x = 0;
	}
	if (y < 0) {
		height += y;
		y = 0;
	}

	if (x + width  > fb->width ) width  = fb->width  - x;
	if (y + height > fb->height) height = fb->height - y;

	for (int offset_y = 0; offset_y < height; offset_y++) {
		for (int offset_x = 0; offset_x < width; offset_x++) {
			fb->data[(y + offset_y) * fb->width + x + offset_x] = color;
		}
	}
}

#endif

#endif

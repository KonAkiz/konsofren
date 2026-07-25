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
void kon_drawLine(kon_framebuffer_t *fb, int x0, int y0, int x1, int y1, uint32_t color);
void kon_drawCircle(kon_framebuffer_t *fb, int center_x, int center_y, int radius, uint32_t color);

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

void kon_drawLine(kon_framebuffer_t *fb, int x0, int y0, int x1, int y1, uint32_t color) {
	if (!fb) return;

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	
	int err = dx - dy;

	for (;;) {
		kon_putPixel(fb, x0, y0, color);

		if (x0 == x1 && y0 == y1) break;

		int e2 = 2 * err;

		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

void kon_drawCircle(kon_framebuffer_t *fb, int center_x, int center_y, int radius, uint32_t color) {
	if (!fb) return;
	if (radius <= 0) return;

	int x = radius;
	int y = 0;
	int err = 1 - radius;

	while (x >= y) {
		kon_putPixel(fb, center_x + x, center_y + y, color);
		kon_putPixel(fb, center_x + y, center_y + x, color);
		kon_putPixel(fb, center_x - y, center_y + x, color);
		kon_putPixel(fb, center_x - x, center_y + y, color);
		kon_putPixel(fb, center_x - x, center_y - y, color);
		kon_putPixel(fb, center_x - y, center_y - x, color);
		kon_putPixel(fb, center_x + y, center_y - x, color);
		kon_putPixel(fb, center_x + x, center_y - y, color);

		y += 1;

		if (err < 0) {
			err += 2 * y + 1;
		} else {
			x -= 1;
			err += 2 * (y - x) + 1;
		}
	}
}

#endif

#endif

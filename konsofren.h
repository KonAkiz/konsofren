#ifndef KONSOFREN_H
#define KONSOFREN_H

#include <stdint.h>
#include <stdlib.h>

typedef struct kon_framebuffer {
	int width, height;
	uint32_t *data;
} kon_framebuffer_t;

kon_framebuffer_t *kon_createFramebuffer(int width, int height);
void kon_freeFramebuffer(kon_framebuffer_t *fb);
void kon_putPixel(kon_framebuffer_t *fb, int x, int y, uint32_t color);
void kon_clearFramebuffer(kon_framebuffer_t *fb, uint32_t color);
void kon_resizeFramebuffer(kon_framebuffer_t *fb, int width, int height);

/*** implementation ***/

#ifdef KONSOFREN_IMPLEMENTATION

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

	kon_clearFramebuffer(fb, 0x000000FF);
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

	kon_clearFramebuffer(fb, 0x000000FF);
}

#endif

#endif

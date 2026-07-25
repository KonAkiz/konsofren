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

/*** implementation ***/

#ifdef KONSOFREN_IMPLEMENTATION

kon_framebuffer_t *kon_createFramebuffer(int width, int height) {
	kon_framebuffer_t *fb = malloc(sizeof(kon_framebuffer_t));
	fb->width = width;
	fb->height = height;
	fb->data = malloc(width * height * sizeof(uint32_t));

	kon_clearFramebuffer(fb, 0x000000FF);
	return fb;
}

void kon_freeFramebuffer(kon_framebuffer_t *fb) {
	if (fb) {
		free(fb->data);
		free(fb);
	}
}

void kon_putPixel(kon_framebuffer_t *fb, int x, int y, uint32_t color) {
	if (fb) {
		if (x < 0 || x >= fb->width || y < 0 || y >= fb->height) return;
		fb->data[y * fb->width + x] = color;
	}
}

void kon_clearFramebuffer(kon_framebuffer_t *fb, uint32_t color) {
	if (fb) {
		for (int i = 0; i < fb->width * fb->height; i++) {
			/* did it directly to not check overhead because of the if in bounds check */
			fb->data[i] = color;
		}
	}
}

#endif

#endif

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

/*** implementation ***/

#ifdef KONSOFREN_IMPLEMENTATION

kon_framebuffer_t *koncreate_Framebuffer(int width, int height) {
	kon_framebuffer_t *fb = malloc(sizeof(kon_framebuffer_t));
	fb->width = width;
	fb->height = height;
	fb->data = malloc(width * height * sizeof(uint32_t));
	return fb;
}

void kon_freeFramebuffer(kon_framebuffer_t *fb) {
	if (fb) {
		free(fb->data);
		free(fb);
	}
}

#endif

#endif

#include <stdio.h>

#define KONSOFREN_IMPLEMENTATION
#include "konsofren.h"

#define EXIT_SUCCESS 0

int main(void) {
	kon_framebuffer_t *fb;

	fb = kon_createFramebuffer(640, 480);

	puts("Hello, World!");

	kon_freeFramebuffer(fb);

	return EXIT_SUCCESS;
}

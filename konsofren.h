#ifndef KONSOFREN_H
#define KONSOFREN_H

#include <stdint.h>

struct kon_framebuffer {
	int width, height;
	uint32_t *data;
};

#endif

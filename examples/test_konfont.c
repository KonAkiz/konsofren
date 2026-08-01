#include <stdio.h>
#include <stdlib.h>

#define KONWINLIB_IMPLEMENTATION
#include "konwinlib.h"

#define KONSOFREN_IMPLEMENTATION
#include "konsofren.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define KONFONT_IMPLEMENTATION
#include "konfont.h"

static uint8_t *readFile(const char *path, size_t *outSize) {
	FILE *f = fopen(path, "rb");
	if (!f) return NULL;

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t *buf = malloc((size_t)size);
	fread(buf, 1, (size_t)size, f);
	fclose(f);

	*outSize = (size_t)size;
	return buf;
}

int main(void) {
	if (!kon_init()) {
		printf("kon_init failed\n");
		return 1;
	}

	kon_window_t *window = kon_createWindow("konfont test", 0, 0, 400, 100, KON_WINDOW_CENTER | KON_WINDOW_RESIZABLE);
	if (!window) {
		printf("kon_createWindow failed\n");
		kon_deinit();
		return 1;
	}
	kon_setExitKey(window, 'q');

	size_t fontSize;
	uint8_t *fontData = readFile("examples/IosevkaTerm-Extended.ttf", &fontSize);
	kon_font_t *font = kon_loadFont(fontData, fontSize, 32.0f);
	if (!font) {
		printf("kon_loadFont failed\n");
		kon_destroyWindow(window);
		kon_deinit();
		return 1;
	}

	kon_framebuffer_t *fb = kon_createFramebuffer(400, 100);

	while (!kon_windowShouldClose(window)) {
		kon_event_t event;
		while (kon_pollEvent(window, &event)) {
			if (event.type == KON_EVENT_NONE) break;
			if (event.type == KON_EVENT_CLOSE) break;
		}

		kon_clearFramebuffer(fb, 0x00000000);
		kon_drawText(fb, font, 10, 40, "Hello, KonFont!", 0xFFFFFFFF);
		kon_blitPixels(window, fb->data, fb->width, fb->height);
	}

	kon_freeFramebuffer(fb);
	kon_freeFont(font);
	free(fontData);
	kon_destroyWindow(window);
	kon_deinit();

	return 0;
}

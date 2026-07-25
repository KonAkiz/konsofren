#include <stdio.h>

/* using this for creating windows to see results */
#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define KONSOFREN_IMPLEMENTATION
#include "konsofren.h"

#define EXIT_SUCCESS 0

#define WINDOW_TITLE "konsofren test - using RGFW"
#define WINDOW_W 640
#define WINDOW_H 480

int main(void) {

	/*** window handling initialization ***/

	RGFW_init("konsofren test", 0);
	RGFW_window *win = RGFW_createWindow(WINDOW_TITLE, 0, 0, WINDOW_W, WINDOW_H, RGFW_windowCenter | RGFW_windowTransparent);
	if (!win) {
		RGFW_deinit();
		return 1;
	}
	RGFW_window_setExitKey(win, RGFW_keyQ);

	kon_framebuffer_t *fb = kon_createFramebuffer(WINDOW_W, WINDOW_H);
	if (!fb) {
		RGFW_window_close(win);
		RGFW_deinit();
		return 1;
	}

	kon_putPixel(fb, 20, 20, 0xFFFFFFFF);

	RGFW_surface *surface = RGFW_window_createSurface(win, (u8*)fb->data, fb->width, fb->height, RGFW_formatABGR8);
	if (!surface) {
		kon_freeFramebuffer(fb);
		RGFW_window_close(win);
		RGFW_deinit();
		return 1;
	}

	/*** etc ***/

	const char *image_file = "test.png";

	int w, h, channels;
	unsigned char *pixels = stbi_load(image_file, &w, &h, &channels, 4);
	if (!pixels) {
		RGFW_surface_free(surface);
		kon_freeFramebuffer(fb);
		RGFW_window_close(win);
		RGFW_deinit();
		printf("stbi_load: error loading image \"%s\"", image_file);
		return 1;
	}

	kon_image *image = kon_loadImage(pixels, w, h, konFormatRGBA8);
	if (!image) {
		stbi_image_free(pixels);
		RGFW_surface_free(surface);
		kon_freeFramebuffer(fb);
		RGFW_window_close(win);
		RGFW_deinit();
		return 1;
	}

	/*** main loop ***/

	RGFW_event event;
	while (RGFW_window_shouldClose(win) == RGFW_FALSE) {
		while (RGFW_window_checkEvent(win, &event)) {
			if (event.type == RGFW_windowResized) {
				kon_resizeFramebuffer(fb, event.update.w, event.update.h);
				
				RGFW_surface_free(surface);
				surface = RGFW_window_createSurface(win, (u8*)fb->data, fb->width, fb->height, RGFW_formatABGR8);
			}
		}

		/*** draw tests ***/
		kon_clearFramebuffer(fb, KON_BACKGROUND_COLOR);

		kon_drawImage(fb, 50, 50, 4, 4, image);

		/* kon_drawRectangle(fb, 400, 300, 50, 50, 0xFFA500FF); */
		/* kon_fillRectangle(fb, 20, 20, 50, 50, 0xFFA500FF); */
		/* kon_drawLine(fb, 300, 100, 100, 250, 0xFFFFFFFF); */
		/* kon_drawCircle(fb, fb->width / 2, fb->height / 2, 90, 0xFF0000FF); */
		/* kon_fillRectangle(fb, 200, 200, 50, 50, 0xFF00005F); */

		RGFW_window_blitSurface(win, surface);
	}

	/*** cleanup ***/

	kon_freeImage(image);
	stbi_image_free(pixels);
	RGFW_surface_free(surface);
	kon_freeFramebuffer(fb);
	RGFW_window_close(win);

	RGFW_deinit();

	return EXIT_SUCCESS;
}

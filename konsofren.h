/* SPDX-License-Identifier: MIT */
/*
 * konsofren.h
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

#ifndef KONSOFREN_H
#define KONSOFREN_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define KON_BACKGROUND_COLOR 0xFF05050A

typedef struct kon_framebuffer {
	int width, height;
	uint32_t *data;
	float *depth; /* created the first time something is drawn with KON_RENDER_DEPTH */
} kon_framebuffer_t;

typedef kon_framebuffer_t kon_image;

typedef enum kon_imageFormat {
	konFormatRGBA8 = 0,
	konFormatABGR8,
	konFormatARGB8,
	konFormatBGRA8
} kon_imageFormat_t;

#define KON_PI 3.14159265358979323846f
#define KON_TAU (2.0f * KON_PI)

#define KON_DEG2RAD(d) ((float)(d) * (KON_PI / 180.0f))
#define KON_RAD2DEG(r) ((float)(r) * (180.0f / KON_PI))

/* kon_vec3_t built from degrees, handy for rotations: KON_DEG3(0, 45, 0) */
#define KON_DEG3(x, y, z) ((kon_vec3_t){KON_DEG2RAD(x), KON_DEG2RAD(y), KON_DEG2RAD(z)})

#define KON_VEC2(x, y) ((kon_vec2_t){(float)(x), (float)(y)})
#define KON_VEC3(x, y, z) ((kon_vec3_t){(float)(x), (float)(y), (float)(z)})
#define KON_VEC4(x, y, z, w) ((kon_vec4_t){(float)(x), (float)(y), (float)(z), (float)(w)})

#define KON_VEC3_ZERO KON_VEC3(0, 0, 0)
#define KON_VEC3_ONE KON_VEC3(1, 1, 1)
#define KON_VEC3_UP KON_VEC3(0, 1, 0)

/* right-handed, +Y up, camera looks down -Z, column-major: m[column * 4 + row] */

typedef struct kon_vec2 { float x, y; } kon_vec2_t;
typedef struct kon_vec3 { float x, y, z; } kon_vec3_t;
typedef struct kon_vec4 { float x, y, z, w; } kon_vec4_t;
typedef struct kon_mat4 { float m[16]; } kon_mat4_t;

/* game is the fixed-size framebuffer you draw into, screen always matches the window */
typedef struct kon_display {
	kon_framebuffer_t *game;
	kon_framebuffer_t *screen;
	int integerOnly;

	/* where the game image sits inside screen, kept up to date by the functions below */
	int viewX, viewY, viewWidth, viewHeight;
	int *columnMap;
} kon_display_t;

typedef enum kon_projection {
	KON_PROJECTION_PERSPECTIVE = 0,
	KON_PROJECTION_ORTHOGRAPHIC
} kon_projection_t;

typedef struct kon_camera {
	kon_vec3_t position;
	kon_vec3_t target;
	kon_vec3_t up;
	kon_projection_t projection;
	float fov;       /* vertical field of view in radians, perspective only */
	float orthoSize; /* half the visible height in world units, orthographic only */
	float nearZ, farZ;
} kon_camera_t;

/* edges holds pairs of indices into vertices */
typedef struct kon_wireMesh {
	const kon_vec3_t *vertices;
	const uint16_t *edges;
	int edgeCount;
} kon_wireMesh_t;

typedef enum kon_renderFlags {
	KON_RENDER_NONE = 0,
	KON_RENDER_WIREFRAME = 1 << 0,
	KON_RENDER_DEPTH = 1 << 1,
	KON_RENDER_CULL_BACK = 1 << 2,
	KON_RENDER_FLAT = 1 << 3 /* light each triangle as a whole instead of smoothly across its vertex normals */
} kon_renderFlags_t;

#define KON_RENDER_DEFAULT ((kon_renderFlags_t)(KON_RENDER_DEPTH | KON_RENDER_CULL_BACK))

/* normal points away from the surface and is only used for lighting, uv (0, 0) is the top-left of the texture, (1, 1) the bottom-right */
typedef struct kon_vertex {
	kon_vec3_t position;
	kon_vec3_t normal;
	kon_vec2_t uv;
} kon_vertex_t;

/* texture is used when it isn't NULL, otherwise color */
typedef struct kon_material {
	const kon_image *texture;
	uint32_t color;
} kon_material_t;

/* one light from far away, like the sun, plus a base light that reaches everything */
typedef struct kon_light {
	kon_vec3_t direction; /* the way the light travels, (0, -1, 0) shines straight down */
	uint32_t color;       /* 0xFFRRGGBB, alpha is ignored */
	float intensity;      /* strength of the directional part, 0 to 1 */
	float ambient;        /* strength of the base light, 0 to 1 */
} kon_light_t;

/* front faces are counter-clockwise, indices hold three entries per triangle */
typedef struct kon_mesh {
	const kon_vertex_t *vertices;
	const uint16_t *indices;
	int triangleCount;
} kon_mesh_t;

/*** 2D declarations ***/

/*** framebuffer declarations ***/

kon_framebuffer_t *kon_createFramebuffer(int width, int height);
void kon_freeFramebuffer(kon_framebuffer_t *fb);
void kon_putPixel(kon_framebuffer_t *fb, int x, int y, uint32_t color);
void kon_clearFramebuffer(kon_framebuffer_t *fb, uint32_t color);
void kon_resizeFramebuffer(kon_framebuffer_t *fb, int width, int height);
void kon_exportPixels(kon_framebuffer_t *fb, kon_imageFormat_t format, uint8_t *out);

/*** image declarations ***/

kon_image *kon_loadImage(const uint8_t *pixels, int width, int height, kon_imageFormat_t format);
void kon_freeImage(kon_image *image);

void kon_drawImage(kon_framebuffer_t *fb, int x, int y, int width, int height, kon_image *image);

/*** draw function declarations ***/

void kon_drawRectangle(kon_framebuffer_t *fb, int x, int y, int width, int height, uint32_t color);
void kon_fillRectangle(kon_framebuffer_t *fb, int x, int y, int width, int height, uint32_t color);
void kon_drawLine(kon_framebuffer_t *fb, int x0, int y0, int x1, int y1, uint32_t color);
void kon_drawCircle(kon_framebuffer_t *fb, int center_x, int center_y, int radius, uint32_t color);
void kon_fillCircle(kon_framebuffer_t *fb, int center_x, int center_y, int radius, uint32_t color);

/*** math declarations ***/

/*** scalar declarations ***/

float kon_clamp(float v, float min, float max);
float kon_lerp(float a, float b, float t);

/*** vec2 declarations ***/

kon_vec2_t kon_vec2Add(kon_vec2_t a, kon_vec2_t b);
kon_vec2_t kon_vec2Sub(kon_vec2_t a, kon_vec2_t b);
kon_vec2_t kon_vec2Scale(kon_vec2_t a, float s);
float kon_vec2Dot(kon_vec2_t a, kon_vec2_t b);

/*** vec3 declarations ***/

kon_vec3_t kon_vec3Add(kon_vec3_t a, kon_vec3_t b);
kon_vec3_t kon_vec3Sub(kon_vec3_t a, kon_vec3_t b);
kon_vec3_t kon_vec3Scale(kon_vec3_t a, float s);
float kon_vec3Dot(kon_vec3_t a, kon_vec3_t b);
kon_vec3_t kon_vec3Cross(kon_vec3_t a, kon_vec3_t b);
float kon_vec3Length(kon_vec3_t a);
kon_vec3_t kon_vec3Normalize(kon_vec3_t a);
kon_vec3_t kon_vec3Lerp(kon_vec3_t a, kon_vec3_t b, float t);

/*** mat4 declarations ***/

kon_mat4_t kon_mat4Identity(void);
kon_mat4_t kon_mat4Mul(kon_mat4_t a, kon_mat4_t b);
kon_vec4_t kon_mat4MulVec4(kon_mat4_t m, kon_vec4_t v);
kon_vec3_t kon_mat4MulPoint(kon_mat4_t m, kon_vec3_t p);

/* like blender's local/global: the plain versions work along the model's own axes,
   the World versions along the world's axes. neither rotate nor scale ever moves the model's position.
   angles are euler angles in radians (x = pitch, y = yaw, z = roll), see KON_DEG3 */
kon_mat4_t kon_mat4Translate(kon_mat4_t m, kon_vec3_t v);
kon_mat4_t kon_mat4Rotate(kon_mat4_t m, kon_vec3_t angles);
kon_mat4_t kon_mat4Scale(kon_mat4_t m, kon_vec3_t v);
kon_mat4_t kon_mat4TranslateWorld(kon_mat4_t m, kon_vec3_t v);
kon_mat4_t kon_mat4RotateWorld(kon_mat4_t m, kon_vec3_t angles);
kon_mat4_t kon_mat4ScaleWorld(kon_mat4_t m, kon_vec3_t v);

kon_mat4_t kon_mat4FromTranslation(kon_vec3_t v);
kon_mat4_t kon_mat4FromRotation(kon_vec3_t angles);
kon_mat4_t kon_mat4FromAxisAngle(kon_vec3_t axis, float angle);
kon_mat4_t kon_mat4FromScale(kon_vec3_t v);
kon_mat4_t kon_mat4FromTransform(kon_vec3_t position, kon_vec3_t angles, kon_vec3_t scale);

kon_mat4_t kon_mat4LookAt(kon_vec3_t eye, kon_vec3_t target, kon_vec3_t up);
kon_mat4_t kon_mat4Perspective(float fovY, float aspect, float nearZ, float farZ);
kon_mat4_t kon_mat4Ortho(float left, float right, float bottom, float top, float nearZ, float farZ);

/*** display declarations ***/


kon_display_t *kon_createDisplay(int gameWidth, int gameHeight, int windowWidth, int windowHeight);
void kon_freeDisplay(kon_display_t *display);

void kon_setGameResolution(kon_display_t *display, int gameWidth, int gameHeight);
void kon_setIntegerScaling(kon_display_t *display, int integerOnly);
void kon_resizeDisplay(kon_display_t *display, int windowWidth, int windowHeight);

/* scales game into screen, then blit display->screen to the window */
void kon_presentDisplay(kon_display_t *display);

/* returns 0 if the window position is on a bar, outside the game image */
int kon_windowToGame(const kon_display_t *display, int windowX, int windowY, int *gameX, int *gameY);

/*** 3D declarations ***/

/*** camera declarations ***/

kon_camera_t kon_cameraDefault(void);
kon_mat4_t kon_cameraView(const kon_camera_t *camera);
kon_mat4_t kon_cameraProjection(const kon_camera_t *camera, int width, int height);
kon_mat4_t kon_cameraViewProjection(const kon_camera_t *camera, int width, int height);

/*** 3D draw declarations ***/

/* returns 0 if the point is behind the camera */
int kon_worldToScreen(const kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_vec3_t point, kon_vec2_t *out);

void kon_drawLine3D(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_vec3_t a, kon_vec3_t b, uint32_t color);
void kon_drawWireMesh(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_mat4_t model, const kon_wireMesh_t *mesh, uint32_t color);

kon_material_t kon_materialColor(uint32_t color);
kon_material_t kon_materialTexture(const kon_image *texture);
kon_light_t kon_lightDefault(void);

/* light can be NULL for no lighting. the triangle's positions and normals are in world space */
void kon_drawTriangle3D(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_vertex_t a, kon_vertex_t b, kon_vertex_t c, const kon_material_t *material, const kon_light_t *light, kon_renderFlags_t flags);
void kon_drawMesh(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_mat4_t model, const kon_mesh_t *mesh, const kon_material_t *material, const kon_light_t *light, kon_renderFlags_t flags);

/* cube from -1 to 1 with every face mapped to the whole texture */
extern const kon_mesh_t kon_cubeMesh;

/*** implementation ***/

#ifdef KONSOFREN_IMPLEMENTATION


/*** private helper ***/

static inline uint32_t kon_blendColor(uint32_t dst, uint32_t src) {
	
	uint8_t src_a = (uint8_t)((src >> 24) & 0xFF);
	uint8_t src_r = (uint8_t)((src >> 16) & 0xFF);
	uint8_t src_g = (uint8_t)((src >> 8)  & 0xFF);
	uint8_t src_b = (uint8_t)((src >> 0)  & 0xFF);

	if (src_a == 0xFF) {
		return src;
	}

	uint8_t dst_a = (uint8_t)((dst >> 24) & 0xFF);
	uint8_t dst_r = (uint8_t)((dst >> 16) & 0xFF);
	uint8_t dst_g = (uint8_t)((dst >> 8)  & 0xFF);
	uint8_t dst_b = (uint8_t)((dst >> 0)  & 0xFF);

	uint8_t inv_a = (uint8_t)(255 - src_a);

	uint8_t out_r = (uint8_t)((src_r * src_a + dst_r * inv_a) / 255);
	uint8_t out_g = (uint8_t)((src_g * src_a + dst_g * inv_a) / 255);
	uint8_t out_b = (uint8_t)((src_b * src_a + dst_b * inv_a) / 255);

	return ((uint32_t)dst_a << 24) | ((uint32_t)out_r << 16) | ((uint32_t)out_g << 8) | (uint32_t)out_b;
}

/* Cohen-Sutherland: clips the line to the framebuffer, returns 0 if nothing is visible */
static int kon_outCode_(double x, double y, int width, int height) {
	int code = 0;
	if (x < 0) code |= 1; else if (x > width - 1) code |= 2;
	if (y < 0) code |= 4; else if (y > height - 1) code |= 8;
	return code;
}

static int kon_clipLine_(int width, int height, int *x0, int *y0, int *x1, int *y1) {
	double ax = *x0, ay = *y0, bx = *x1, by = *y1;
	double maxX = width - 1, maxY = height - 1;
	int ca = kon_outCode_(ax, ay, width, height);
	int cb = kon_outCode_(bx, by, width, height);

	while (ca | cb) {
		if (ca & cb) return 0;

		int out = ca ? ca : cb;
		double x, y;

		if (out & 8) {
			x = ax + (bx - ax) * (maxY - ay) / (by - ay);
			y = maxY;
		} else if (out & 4) {
			x = ax + (bx - ax) * (0 - ay) / (by - ay);
			y = 0;
		} else if (out & 2) {
			y = ay + (by - ay) * (maxX - ax) / (bx - ax);
			x = maxX;
		} else {
			y = ay + (by - ay) * (0 - ax) / (bx - ax);
			x = 0;
		}

		if (out == ca) {
			ax = x; ay = y;
			ca = kon_outCode_(ax, ay, width, height);
		} else {
			bx = x; by = y;
			cb = kon_outCode_(bx, by, width, height);
		}
	}

	*x0 = (int)(ax + 0.5); *y0 = (int)(ay + 0.5);
	*x1 = (int)(bx + 0.5); *y1 = (int)(by + 0.5);
	return 1;
}

/*** framebuffer implementation ***/

kon_framebuffer_t *kon_createFramebuffer(int width, int height) {
	kon_framebuffer_t *fb = calloc(1, sizeof(kon_framebuffer_t));
	if (!fb) return NULL;

	fb->data = calloc((size_t)width * (size_t)height, sizeof(uint32_t));
	if (!fb->data) {
		free(fb);
		return NULL;
	}

	fb->width = width;
	fb->height = height;
	return fb;
}

void kon_freeFramebuffer(kon_framebuffer_t *fb) {
	if (!fb) return;

	free(fb->data);
	free(fb->depth);
	free(fb);
}

void kon_putPixel(kon_framebuffer_t *fb, int x, int y, uint32_t color) {
	if (!fb) return;

	if (x < 0 || x >= fb->width || y < 0 || y >= fb->height) return;

	uint32_t dst = fb->data[y * fb->width + x];
	fb->data[y * fb->width + x] = kon_blendColor(dst, color);
}

void kon_clearFramebuffer(kon_framebuffer_t *fb, uint32_t color) {
	if (!fb) return;

	for (int i = 0; i < fb->width * fb->height; i++) {
		/* did it directly to not check overhead because of the if in bounds check */
		fb->data[i] = color;
	}

	if (fb->depth) {
		for (int i = 0; i < fb->width * fb->height; i++) {
			fb->depth[i] = 1.0f;
		}
	}
}

void kon_resizeFramebuffer(kon_framebuffer_t *fb, int width, int height) {
	if (!fb) return;

	uint32_t *tmp = realloc(fb->data, (size_t)width * (size_t)height * sizeof(uint32_t));
	if (!tmp) return;

	fb->data = tmp;
	fb->width  = width;
	fb->height = height;

	if (fb->depth) {
		float *depth = realloc(fb->depth, (size_t)width * (size_t)height * sizeof(float));
		if (!depth) free(fb->depth);
		fb->depth = depth;
	}

	kon_clearFramebuffer(fb, KON_BACKGROUND_COLOR);
}

void kon_exportPixels(kon_framebuffer_t *fb, kon_imageFormat_t format, uint8_t *out) {
	if (!fb || !out) return;

	int fbSize = fb->width * fb->height;

	for (int i = 0; i < fbSize; i++) {
		uint32_t color = fb->data[i];

		uint8_t a = (color >> 24) & 0xFF;
		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8)  & 0xFF;
		uint8_t b = (color >> 0)  & 0xFF;

		switch (format) {
		case konFormatRGBA8:
			out[i * 4 + 0] = r;
			out[i * 4 + 1] = g;
			out[i * 4 + 2] = b;
			out[i * 4 + 3] = a;
			break;
		case konFormatABGR8:
			out[i * 4 + 0] = a;
			out[i * 4 + 1] = b;
			out[i * 4 + 2] = g;
			out[i * 4 + 3] = r;
			break;
		case konFormatARGB8:
			out[i * 4 + 0] = a;
			out[i * 4 + 1] = r;
			out[i * 4 + 2] = g;
			out[i * 4 + 3] = b;
			break;
		case konFormatBGRA8:
			out[i * 4 + 0] = b;
			out[i * 4 + 1] = g;
			out[i * 4 + 2] = r;
			out[i * 4 + 3] = a;
			break;
		default:
			return;
		}
	}
}

/*** draw functions implementation ***/

void kon_drawRectangle(kon_framebuffer_t *fb, int x, int y, int width, int height, uint32_t color) {
	if (!fb) return;

	kon_drawLine(fb, x, y, x + width, y, color);
	kon_drawLine(fb, x, y + height, x + width, y + height, color);
	kon_drawLine(fb, x, y, x, y + height, color);
	kon_drawLine(fb, x + width, y, x + width, y + height, color);
}

void kon_fillRectangle(kon_framebuffer_t *fb, int x, int y, int width, int height, uint32_t color) {
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
			uint32_t dst = fb->data[(offset_y + y) * fb->width + (offset_x + x)];
			fb->data[(y + offset_y) * fb->width + (x + offset_x)] = kon_blendColor(dst, color);

		}
	}
}

void kon_drawLine(kon_framebuffer_t *fb, int x0, int y0, int x1, int y1, uint32_t color) {
	if (!fb) return;

	if (!kon_clipLine_(fb->width, fb->height, &x0, &y0, &x1, &y1)) return;

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

void kon_fillCircle(kon_framebuffer_t *fb, int center_x, int center_y, int radius, uint32_t color) {
	if (!fb) return;
	if (radius <= 0) return;

	int x = radius;
	int y = 0;
	int err = 1 - radius;

	while (x >= y) {
		kon_drawLine(fb, center_x - x, center_y + y, center_x + x, center_y + y, color);
		kon_drawLine(fb, center_x - x, center_y - y, center_x + x, center_y - y, color);
		kon_drawLine(fb, center_x - y, center_y + x, center_x + y, center_y + x, color);
		kon_drawLine(fb, center_x - y, center_y - x, center_x + y, center_y - x, color);

		y += 1;

		if (err < 0) {
			err += 2 * y + 1;
		} else {
			x -= 1;
			err += 2 * (y - x) + 1;
		}
	}
}

/*** image implementation ***/

kon_image *kon_loadImage(const uint8_t *pixels, int width, int height, kon_imageFormat_t format) {
	kon_image *image = calloc(1, sizeof(kon_image));
	if (!image) return NULL;

	image->data = malloc((size_t)width * (size_t)height * sizeof(uint32_t));
	if (!image->data) {
		free(image);
		return NULL;
	}

	image->width = width;
	image->height = height;

	for (int i = 0; i < width * height; i++) {
		uint8_t r, g, b, a;

		switch(format) {
		case konFormatRGBA8:
			r = pixels[i * 4 + 0];
			g = pixels[i * 4 + 1];
			b = pixels[i * 4 + 2];
			a = pixels[i * 4 + 3];
			break;
		case konFormatABGR8:
			a = pixels[i * 4 + 0];
			b = pixels[i * 4 + 1];
			g = pixels[i * 4 + 2];
			r = pixels[i * 4 + 3];
			break;
		case konFormatARGB8:
			a = pixels[i * 4 + 0];
			r = pixels[i * 4 + 1];
			g = pixels[i * 4 + 2];
			b = pixels[i * 4 + 3];
			break;
		case konFormatBGRA8:
			b = pixels[i * 4 + 0];
			g = pixels[i * 4 + 1];
			r = pixels[i * 4 + 2];
			a = pixels[i * 4 + 3];
			break;
		default:
			free(image->data);
			free(image);
			return NULL;
		}

		image->data[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
	}

	return image;
}
void kon_freeImage(kon_image *image) {
	kon_freeFramebuffer(image);
}

void kon_drawImage(kon_framebuffer_t *fb, int x, int y, int width, int height, kon_image *image) {
	if (!fb || !image) return;

	/* just trying to not draw when it's unnecessary */
	if (image->width <= 0 || image->height <= 0) return;
	if (width <= 0 || height <= 0) return;
	if (x >= fb->width || y >= fb->height) return;

	/* using this loop to make sure we only the parts of the image that are on screen */
	for (int iy = 0; iy < height; iy++) {
		for (int ix = 0; ix < width; ix++) {
			int src_x = ix * image->width  / width;
			int src_y = iy * image->height / height;

			uint32_t color = image->data[src_y * image->width + src_x];
			kon_putPixel(fb, x+ix, y+iy, color);
		}
	}
}

/*** math implementation ***/


/*** scalar ***/

float kon_clamp(float v, float min, float max) { return v < min ? min : (v > max ? max : v); }
float kon_lerp(float a, float b, float t) { return a + (b - a) * t; }

/*** vec2 ***/

kon_vec2_t kon_vec2Add(kon_vec2_t a, kon_vec2_t b) { return (kon_vec2_t){a.x + b.x, a.y + b.y}; }
kon_vec2_t kon_vec2Sub(kon_vec2_t a, kon_vec2_t b) { return (kon_vec2_t){a.x - b.x, a.y - b.y}; }
kon_vec2_t kon_vec2Scale(kon_vec2_t a, float s) { return (kon_vec2_t){a.x * s, a.y * s}; }
float kon_vec2Dot(kon_vec2_t a, kon_vec2_t b) { return a.x * b.x + a.y * b.y; }

/*** vec3 ***/

kon_vec3_t kon_vec3Add(kon_vec3_t a, kon_vec3_t b) { return (kon_vec3_t){a.x + b.x, a.y + b.y, a.z + b.z}; }
kon_vec3_t kon_vec3Sub(kon_vec3_t a, kon_vec3_t b) { return (kon_vec3_t){a.x - b.x, a.y - b.y, a.z - b.z}; }
kon_vec3_t kon_vec3Scale(kon_vec3_t a, float s) { return (kon_vec3_t){a.x * s, a.y * s, a.z * s}; }
float kon_vec3Dot(kon_vec3_t a, kon_vec3_t b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

kon_vec3_t kon_vec3Cross(kon_vec3_t a, kon_vec3_t b) {
	return (kon_vec3_t){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

float kon_vec3Length(kon_vec3_t a) { return sqrtf(kon_vec3Dot(a, a)); }

kon_vec3_t kon_vec3Normalize(kon_vec3_t a) {
	float len = kon_vec3Length(a);
	return len > 0.0f ? kon_vec3Scale(a, 1.0f / len) : a;
}

kon_vec3_t kon_vec3Lerp(kon_vec3_t a, kon_vec3_t b, float t) {
	return (kon_vec3_t){kon_lerp(a.x, b.x, t), kon_lerp(a.y, b.y, t), kon_lerp(a.z, b.z, t)};
}

/*** mat4 ***/

kon_mat4_t kon_mat4Identity(void) {
	kon_mat4_t r = {{0}};
	r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
	return r;
}

/* returns a * b, b is applied first */
kon_mat4_t kon_mat4Mul(kon_mat4_t a, kon_mat4_t b) {
	kon_mat4_t r;
	for (int c = 0; c < 4; c++) {
		for (int row = 0; row < 4; row++) {
			float sum = 0.0f;
			for (int k = 0; k < 4; k++) {
				sum += a.m[k * 4 + row] * b.m[c * 4 + k];
			}
			r.m[c * 4 + row] = sum;
		}
	}
	return r;
}

kon_vec4_t kon_mat4MulVec4(kon_mat4_t m, kon_vec4_t v) {
	return (kon_vec4_t){
		m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12] * v.w,
		m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13] * v.w,
		m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
		m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
	};
}

kon_vec3_t kon_mat4MulPoint(kon_mat4_t m, kon_vec3_t p) {
	kon_vec4_t r = kon_mat4MulVec4(m, (kon_vec4_t){p.x, p.y, p.z, 1.0f});
	return (kon_vec3_t){r.x, r.y, r.z};
}

kon_mat4_t kon_mat4FromTranslation(kon_vec3_t v) {
	kon_mat4_t r = kon_mat4Identity();
	r.m[12] = v.x;
	r.m[13] = v.y;
	r.m[14] = v.z;
	return r;
}

kon_mat4_t kon_mat4FromScale(kon_vec3_t v) {
	kon_mat4_t r = kon_mat4Identity();
	r.m[0] = v.x;
	r.m[5] = v.y;
	r.m[10] = v.z;
	return r;
}

kon_mat4_t kon_mat4FromAxisAngle(kon_vec3_t axis, float angle) {
	kon_vec3_t a = kon_vec3Normalize(axis);
	float c = cosf(angle), s = sinf(angle), t = 1.0f - c;
	kon_mat4_t r = kon_mat4Identity();
	r.m[0] = t * a.x * a.x + c;
	r.m[1] = t * a.x * a.y + s * a.z;
	r.m[2] = t * a.x * a.z - s * a.y;
	r.m[4] = t * a.x * a.y - s * a.z;
	r.m[5] = t * a.y * a.y + c;
	r.m[6] = t * a.y * a.z + s * a.x;
	r.m[8] = t * a.x * a.z + s * a.y;
	r.m[9] = t * a.y * a.z - s * a.x;
	r.m[10] = t * a.z * a.z + c;
	return r;
}

/* applied in order roll (z), pitch (x), yaw (y) */
kon_mat4_t kon_mat4FromRotation(kon_vec3_t angles) {
	kon_mat4_t x = kon_mat4FromAxisAngle(KON_VEC3(1, 0, 0), angles.x);
	kon_mat4_t y = kon_mat4FromAxisAngle(KON_VEC3(0, 1, 0), angles.y);
	kon_mat4_t z = kon_mat4FromAxisAngle(KON_VEC3(0, 0, 1), angles.z);
	return kon_mat4Mul(y, kon_mat4Mul(x, z));
}

kon_mat4_t kon_mat4FromTransform(kon_vec3_t position, kon_vec3_t angles, kon_vec3_t scale) {
	kon_mat4_t r = kon_mat4Mul(kon_mat4FromRotation(angles), kon_mat4FromScale(scale));
	return kon_mat4Mul(kon_mat4FromTranslation(position), r);
}

kon_mat4_t kon_mat4Translate(kon_mat4_t m, kon_vec3_t v) { return kon_mat4Mul(m, kon_mat4FromTranslation(v)); }
kon_mat4_t kon_mat4Rotate(kon_mat4_t m, kon_vec3_t angles) { return kon_mat4Mul(m, kon_mat4FromRotation(angles)); }
kon_mat4_t kon_mat4Scale(kon_mat4_t m, kon_vec3_t v) { return kon_mat4Mul(m, kon_mat4FromScale(v)); }
kon_mat4_t kon_mat4TranslateWorld(kon_mat4_t m, kon_vec3_t v) { return kon_mat4Mul(kon_mat4FromTranslation(v), m); }

/* pre-multiplying would also swing the position around the world origin, so put it back */
kon_mat4_t kon_mat4RotateWorld(kon_mat4_t m, kon_vec3_t angles) {
	kon_mat4_t r = kon_mat4Mul(kon_mat4FromRotation(angles), m);
	r.m[12] = m.m[12];
	r.m[13] = m.m[13];
	r.m[14] = m.m[14];
	return r;
}

kon_mat4_t kon_mat4ScaleWorld(kon_mat4_t m, kon_vec3_t v) {
	kon_mat4_t r = kon_mat4Mul(kon_mat4FromScale(v), m);
	r.m[12] = m.m[12];
	r.m[13] = m.m[13];
	r.m[14] = m.m[14];
	return r;
}

kon_mat4_t kon_mat4LookAt(kon_vec3_t eye, kon_vec3_t target, kon_vec3_t up) {
	kon_vec3_t f = kon_vec3Normalize(kon_vec3Sub(target, eye));
	kon_vec3_t s = kon_vec3Normalize(kon_vec3Cross(f, up));
	kon_vec3_t u = kon_vec3Cross(s, f);

	kon_mat4_t r = kon_mat4Identity();
	r.m[0] = s.x;  r.m[4] = s.y;  r.m[8]  = s.z;  r.m[12] = -kon_vec3Dot(s, eye);
	r.m[1] = u.x;  r.m[5] = u.y;  r.m[9]  = u.z;  r.m[13] = -kon_vec3Dot(u, eye);
	r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z; r.m[14] =  kon_vec3Dot(f, eye);
	return r;
}

/* fovY in radians, aspect = width / height */
kon_mat4_t kon_mat4Perspective(float fovY, float aspect, float nearZ, float farZ) {
	kon_mat4_t r = {{0}};
	float f = 1.0f / tanf(fovY * 0.5f);
	r.m[0] = f / aspect;
	r.m[5] = f;
	r.m[10] = (farZ + nearZ) / (nearZ - farZ);
	r.m[11] = -1.0f;
	r.m[14] = (2.0f * farZ * nearZ) / (nearZ - farZ);
	return r;
}

kon_mat4_t kon_mat4Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) {
	kon_mat4_t r = kon_mat4Identity();
	r.m[0] = 2.0f / (right - left);
	r.m[5] = 2.0f / (top - bottom);
	r.m[10] = -2.0f / (farZ - nearZ);
	r.m[12] = -(right + left) / (right - left);
	r.m[13] = -(top + bottom) / (top - bottom);
	r.m[14] = -(farZ + nearZ) / (farZ - nearZ);
	return r;
}


/*** display implementation ***/

static void kon_updateView_(kon_display_t *display) {
	int gameWidth = display->game->width, gameHeight = display->game->height;
	int windowWidth = display->screen->width, windowHeight = display->screen->height;

	int scaleX = windowWidth / gameWidth;
	int scaleY = windowHeight / gameHeight;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;

	if (display->integerOnly && scale >= 1) {
		display->viewWidth  = gameWidth  * scale;
		display->viewHeight = gameHeight * scale;
	} else if ((int64_t)windowWidth * gameHeight <= (int64_t)windowHeight * gameWidth) {
		/* window is narrower than the game: bars on top and bottom */
		display->viewWidth  = windowWidth;
		display->viewHeight = (int)((int64_t)windowWidth * gameHeight / gameWidth);
	} else {
		/* window is wider than the game: bars on the sides */
		display->viewHeight = windowHeight;
		display->viewWidth  = (int)((int64_t)windowHeight * gameWidth / gameHeight);
	}

	display->viewX = (windowWidth  - display->viewWidth)  / 2;
	display->viewY = (windowHeight - display->viewHeight) / 2;

	/* which game column each view column shows, worked out once instead of for every pixel */
	int *map = realloc(display->columnMap, (size_t)windowWidth * sizeof(int));
	if (!map) {
		display->viewWidth = 0;
		return;
	}
	display->columnMap = map;

	for (int x = 0; x < display->viewWidth; x++) {
		map[x] = (int)((int64_t)x * gameWidth / display->viewWidth);
	}
}

kon_display_t *kon_createDisplay(int gameWidth, int gameHeight, int windowWidth, int windowHeight) {
	if (gameWidth <= 0 || gameHeight <= 0 || windowWidth <= 0 || windowHeight <= 0) return NULL;

	kon_display_t *display = calloc(1, sizeof(kon_display_t));
	if (!display) return NULL;

	display->game   = kon_createFramebuffer(gameWidth, gameHeight);
	display->screen = kon_createFramebuffer(windowWidth, windowHeight);
	if (!display->game || !display->screen) {
		kon_freeDisplay(display);
		return NULL;
	}

	kon_clearFramebuffer(display->game, KON_BACKGROUND_COLOR);
	kon_updateView_(display);
	return display;
}

void kon_freeDisplay(kon_display_t *display) {
	if (!display) return;

	kon_freeFramebuffer(display->game);
	kon_freeFramebuffer(display->screen);
	free(display->columnMap);
	free(display);
}

void kon_setGameResolution(kon_display_t *display, int gameWidth, int gameHeight) {
	if (!display || gameWidth <= 0 || gameHeight <= 0) return;

	kon_resizeFramebuffer(display->game, gameWidth, gameHeight);
	kon_updateView_(display);
}

void kon_setIntegerScaling(kon_display_t *display, int integerOnly) {
	if (!display) return;

	display->integerOnly = integerOnly;
	kon_updateView_(display);
}

void kon_resizeDisplay(kon_display_t *display, int windowWidth, int windowHeight) {
	if (!display || windowWidth <= 0 || windowHeight <= 0) return;

	kon_resizeFramebuffer(display->screen, windowWidth, windowHeight);
	kon_updateView_(display);
}

void kon_presentDisplay(kon_display_t *display) {
	if (!display) return;

	const uint32_t barColor = 0xFF000000;
	kon_framebuffer_t *game = display->game;
	kon_framebuffer_t *screen = display->screen;
	int rightX = display->viewX + display->viewWidth;

	for (int y = 0; y < screen->height; y++) {
		uint32_t *row = &screen->data[(size_t)y * (size_t)screen->width];

		if (display->viewWidth <= 0 || y < display->viewY || y >= display->viewY + display->viewHeight) {
			for (int x = 0; x < screen->width; x++) row[x] = barColor;
			continue;
		}

		int srcY = (int)((int64_t)(y - display->viewY) * game->height / display->viewHeight);
		const uint32_t *srcRow = &game->data[(size_t)srcY * (size_t)game->width];

		for (int x = 0; x < display->viewX; x++) row[x] = barColor;
		for (int x = 0; x < display->viewWidth; x++) row[display->viewX + x] = srcRow[display->columnMap[x]];
		for (int x = rightX; x < screen->width; x++) row[x] = barColor;
	}
}

int kon_windowToGame(const kon_display_t *display, int windowX, int windowY, int *gameX, int *gameY) {
	if (!display || display->viewWidth <= 0 || display->viewHeight <= 0) return 0;

	int localX = windowX - display->viewX;
	int localY = windowY - display->viewY;
	if (localX < 0 || localX >= display->viewWidth || localY < 0 || localY >= display->viewHeight) return 0;

	if (gameX) *gameX = (int)((int64_t)localX * display->game->width  / display->viewWidth);
	if (gameY) *gameY = (int)((int64_t)localY * display->game->height / display->viewHeight);
	return 1;
}

/*** 3D implementation ***/


/*** private helper ***/

static kon_vec2_t kon_clipToScreen_(const kon_framebuffer_t *fb, kon_vec4_t clip) {
	/* clamped so the later float to int conversion can't overflow */
	float ndcX = kon_clamp(clip.x / clip.w, -1.0e5f, 1.0e5f);
	float ndcY = kon_clamp(clip.y / clip.w, -1.0e5f, 1.0e5f);

	/* screen +Y points down, NDC +Y points up */
	return KON_VEC2((ndcX + 1.0f) * 0.5f * (float)fb->width, (1.0f - ndcY) * 0.5f * (float)fb->height);
}

/*** camera implementation ***/

kon_camera_t kon_cameraDefault(void) {
	kon_camera_t camera;
	camera.position = KON_VEC3(0, 0, 5);
	camera.target = KON_VEC3_ZERO;
	camera.up = KON_VEC3_UP;
	camera.projection = KON_PROJECTION_PERSPECTIVE;
	camera.fov = KON_DEG2RAD(60);
	camera.orthoSize = 5.0f;
	camera.nearZ = 0.1f;
	camera.farZ = 100.0f;
	return camera;
}

kon_mat4_t kon_cameraView(const kon_camera_t *camera) {
	return kon_mat4LookAt(camera->position, camera->target, camera->up);
}

kon_mat4_t kon_cameraProjection(const kon_camera_t *camera, int width, int height) {
	float aspect = (height > 0) ? (float)width / (float)height : 1.0f;

	if (camera->projection == KON_PROJECTION_ORTHOGRAPHIC) {
		float halfH = camera->orthoSize;
		float halfW = halfH * aspect;
		return kon_mat4Ortho(-halfW, halfW, -halfH, halfH, camera->nearZ, camera->farZ);
	}

	return kon_mat4Perspective(camera->fov, aspect, camera->nearZ, camera->farZ);
}

kon_mat4_t kon_cameraViewProjection(const kon_camera_t *camera, int width, int height) {
	return kon_mat4Mul(kon_cameraProjection(camera, width, height), kon_cameraView(camera));
}

/*** 3D draw implementation ***/

int kon_worldToScreen(const kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_vec3_t point, kon_vec2_t *out) {
	if (!fb || !out) return 0;

	kon_vec4_t clip = kon_mat4MulVec4(viewProjection, KON_VEC4(point.x, point.y, point.z, 1));

	/* in front of the near plane when z >= -w */
	if (clip.z < -clip.w || clip.w <= 0.0f) return 0;

	*out = kon_clipToScreen_(fb, clip);
	return 1;
}

void kon_drawLine3D(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_vec3_t a, kon_vec3_t b, uint32_t color) {
	if (!fb) return;

	kon_vec4_t ca = kon_mat4MulVec4(viewProjection, KON_VEC4(a.x, a.y, a.z, 1));
	kon_vec4_t cb = kon_mat4MulVec4(viewProjection, KON_VEC4(b.x, b.y, b.z, 1));

	/* clip against the near plane (z + w >= 0) before dividing, so points behind the camera never get projected */
	float da = ca.z + ca.w;
	float db = cb.z + cb.w;

	if (da < 0.0f && db < 0.0f) return;

	if (da < 0.0f || db < 0.0f) {
		float t = da / (da - db);
		kon_vec4_t hit = KON_VEC4(
			ca.x + (cb.x - ca.x) * t,
			ca.y + (cb.y - ca.y) * t,
			ca.z + (cb.z - ca.z) * t,
			ca.w + (cb.w - ca.w) * t
		);

		if (da < 0.0f) ca = hit;
		else cb = hit;
	}

	if (ca.w <= 0.0f || cb.w <= 0.0f) return;

	kon_vec2_t sa = kon_clipToScreen_(fb, ca);
	kon_vec2_t sb = kon_clipToScreen_(fb, cb);

	kon_drawLine(fb, (int)sa.x, (int)sa.y, (int)sb.x, (int)sb.y, color);
}

void kon_drawWireMesh(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_mat4_t model, const kon_wireMesh_t *mesh, uint32_t color) {
	if (!fb || !mesh) return;

	kon_mat4_t mvp = kon_mat4Mul(viewProjection, model);

	for (int i = 0; i < mesh->edgeCount; i++) {
		kon_vec3_t a = mesh->vertices[mesh->edges[i * 2 + 0]];
		kon_vec3_t b = mesh->vertices[mesh->edges[i * 2 + 1]];
		kon_drawLine3D(fb, mvp, a, b, color);
	}
}

kon_material_t kon_materialColor(uint32_t color) {
	kon_material_t material = { NULL, color };
	return material;
}

kon_material_t kon_materialTexture(const kon_image *texture) {
	kon_material_t material = { texture, 0xFFFFFFFF };
	return material;
}

kon_light_t kon_lightDefault(void) {
	kon_light_t light;
	light.direction = KON_VEC3(-0.5f, -1.0f, -0.3f);
	light.color = 0xFFFFFFFF;
	light.intensity = 0.8f;
	light.ambient = 0.3f;
	return light;
}

/* how bright a surface with this normal is, 0 to 1. toLight points at the light and has length 1 */
static double kon_lightFactor_(const kon_light_t *light, kon_vec3_t toLight, kon_vec3_t normal) {
	double diffuse = (double)kon_vec3Dot(kon_vec3Normalize(normal), toLight);
	if (diffuse < 0.0) diffuse = 0.0;

	double factor = (double)light->ambient + (double)light->intensity * diffuse;
	return factor > 1.0 ? 1.0 : (factor < 0.0 ? 0.0 : factor);
}

static uint32_t kon_applyLight_(uint32_t color, double factor, const double rgb[3]) {
	uint32_t out = color & 0xFF000000u;

	for (int i = 0; i < 3; i++) {
		int shift = 16 - i * 8;
		double channel = (double)((color >> shift) & 0xFF) * rgb[i] * factor;
		out |= (channel >= 255.0 ? 255u : (uint32_t)(channel + 0.5)) << shift;
	}

	return out;
}

/* the triangle rasterizer works in doubles so huge coordinates near the camera don't lose precision */
typedef struct kon_rasterVertex {
	double x, y, z, w; /* clip space */
	double u, v;
	double light;
} kon_rasterVertex_t;

static kon_rasterVertex_t kon_lerpRasterVertex_(kon_rasterVertex_t a, kon_rasterVertex_t b, double t) {
	kon_rasterVertex_t r;
	r.x = a.x + (b.x - a.x) * t;
	r.y = a.y + (b.y - a.y) * t;
	r.z = a.z + (b.z - a.z) * t;
	r.w = a.w + (b.w - a.w) * t;
	r.u = a.u + (b.u - a.u) * t;
	r.v = a.v + (b.v - a.v) * t;
	r.light = a.light + (b.light - a.light) * t;
	return r;
}

/* keeps the part of the triangle in front of the near plane (z + w >= 0), returns 0 to 4 vertices */
static int kon_clipNear_(const kon_rasterVertex_t in[3], kon_rasterVertex_t out[4]) {
	int count = 0;

	for (int i = 0; i < 3; i++) {
		kon_rasterVertex_t current = in[i];
		kon_rasterVertex_t next = in[(i + 1) % 3];
		double dc = current.z + current.w;
		double dn = next.z + next.w;

		if (dc >= 0.0) out[count++] = current;
		if ((dc >= 0.0) != (dn >= 0.0)) out[count++] = kon_lerpRasterVertex_(current, next, dc / (dc - dn));
	}

	return count;
}

static double kon_edge_(double ax, double ay, double bx, double by, double px, double py) {
	return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

static void kon_rasterizeTriangle_(kon_framebuffer_t *fb, const kon_rasterVertex_t *a, const kon_rasterVertex_t *b, const kon_rasterVertex_t *c,
		const kon_material_t *material, const kon_light_t *light, kon_renderFlags_t flags) {
	const kon_rasterVertex_t *in[3] = {a, b, c};
	double sx[3], sy[3], sz[3], invW[3], uw[3], vw[3], lw[3];

	for (int i = 0; i < 3; i++) {
		if (in[i]->w <= 1.0e-12) return;
		invW[i] = 1.0 / in[i]->w;
		sx[i] = (in[i]->x * invW[i] + 1.0) * 0.5 * fb->width;
		sy[i] = (1.0 - in[i]->y * invW[i]) * 0.5 * fb->height;
		sz[i] = in[i]->z * invW[i];
		uw[i] = in[i]->u * invW[i];
		vw[i] = in[i]->v * invW[i];
		lw[i] = in[i]->light * invW[i];
	}

	/* screen +Y points down, so a counter-clockwise (front) triangle has a negative area here */
	double area = kon_edge_(sx[0], sy[0], sx[1], sy[1], sx[2], sy[2]);
	if (area == 0.0) return;

	if (area > 0.0) {
		if (flags & KON_RENDER_CULL_BACK) return;

		/* back face with culling off: swap two vertices so the math below only deals with one winding */
		double t;
		t = sx[1]; sx[1] = sx[2]; sx[2] = t;
		t = sy[1]; sy[1] = sy[2]; sy[2] = t;
		t = sz[1]; sz[1] = sz[2]; sz[2] = t;
		t = invW[1]; invW[1] = invW[2]; invW[2] = t;
		t = uw[1]; uw[1] = uw[2]; uw[2] = t;
		t = vw[1]; vw[1] = vw[2]; vw[2] = t;
		t = lw[1]; lw[1] = lw[2]; lw[2] = t;
		area = -area;
	}

	double minX = sx[0], maxX = sx[0], minY = sy[0], maxY = sy[0];
	for (int i = 1; i < 3; i++) {
		if (sx[i] < minX) minX = sx[i];
		if (sx[i] > maxX) maxX = sx[i];
		if (sy[i] < minY) minY = sy[i];
		if (sy[i] > maxY) maxY = sy[i];
	}

	/* pixel centers sit at +0.5, clamped to the framebuffer before turning into ints */
	minX = ceil(minX - 0.5); maxX = floor(maxX - 0.5);
	minY = ceil(minY - 0.5); maxY = floor(maxY - 0.5);
	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX > fb->width  - 1) maxX = fb->width  - 1;
	if (maxY > fb->height - 1) maxY = fb->height - 1;
	if (minX > maxX || minY > maxY) return;

	int x0 = (int)minX, x1 = (int)maxX, y0 = (int)minY, y1 = (int)maxY;
	double px = x0 + 0.5, py = y0 + 0.5;

	/* edge i is opposite vertex i, inside means all three are <= 0 for this winding */
	double e0Row = kon_edge_(sx[1], sy[1], sx[2], sy[2], px, py);
	double e1Row = kon_edge_(sx[2], sy[2], sx[0], sy[0], px, py);
	double e2Row = kon_edge_(sx[0], sy[0], sx[1], sy[1], px, py);
	double e0Dx = -(sy[2] - sy[1]), e0Dy = sx[2] - sx[1];
	double e1Dx = -(sy[0] - sy[2]), e1Dy = sx[0] - sx[2];
	double e2Dx = -(sy[1] - sy[0]), e2Dy = sx[1] - sx[0];
	double invArea = 1.0 / area;

	int useDepth = (flags & KON_RENDER_DEPTH) != 0;
	const kon_image *texture = material->texture;

	double lightRgb[3] = {1.0, 1.0, 1.0};
	if (light) {
		lightRgb[0] = (double)((light->color >> 16) & 0xFF) / 255.0;
		lightRgb[1] = (double)((light->color >> 8)  & 0xFF) / 255.0;
		lightRgb[2] = (double)((light->color >> 0)  & 0xFF) / 255.0;
	}

	for (int y = y0; y <= y1; y++) {
		double e0 = e0Row, e1 = e1Row, e2 = e2Row;

		for (int x = x0; x <= x1; x++) {
			if (e0 <= 0.0 && e1 <= 0.0 && e2 <= 0.0) {
				double l0 = e0 * invArea, l1 = e1 * invArea, l2 = e2 * invArea;
				size_t index = (size_t)y * (size_t)fb->width + (size_t)x;
				double z = l0 * sz[0] + l1 * sz[1] + l2 * sz[2];

				if (!useDepth || z < (double)fb->depth[index]) {
					uint32_t texel = material->color;
					double iw = l0 * invW[0] + l1 * invW[1] + l2 * invW[2];

					if (texture) {
						/* uv / w is what varies linearly on screen, dividing by 1 / w again undoes the perspective */
						double u = (l0 * uw[0] + l1 * uw[1] + l2 * uw[2]) / iw;
						double v = (l0 * vw[0] + l1 * vw[1] + l2 * vw[2]) / iw;

						u -= floor(u);
						v -= floor(v);

						int tx = (int)(u * texture->width);
						int ty = (int)(v * texture->height);
						if (tx > texture->width  - 1) tx = texture->width  - 1;
						if (ty > texture->height - 1) ty = texture->height - 1;

						texel = texture->data[(size_t)ty * (size_t)texture->width + (size_t)tx];
					}

					if (light) {
						texel = kon_applyLight_(texel, (l0 * lw[0] + l1 * lw[1] + l2 * lw[2]) / iw, lightRgb);
					}

					uint32_t alpha = texel >> 24;
					if (alpha == 0xFF) {
						fb->data[index] = texel;
						if (useDepth) fb->depth[index] = (float)z;
					} else if (alpha != 0) {
						fb->data[index] = kon_blendColor(fb->data[index], texel);
					}
				}
			}

			e0 += e0Dx; e1 += e1Dx; e2 += e2Dx;
		}

		e0Row += e0Dy; e1Row += e1Dy; e2Row += e2Dy;
	}
}

/* projects, clips and rasterizes one triangle, brightness holds the light factor of each vertex */
static void kon_submitTriangle_(kon_framebuffer_t *fb, kon_mat4_t mvp, const kon_vertex_t *source[3], const double brightness[3],
		const kon_material_t *material, const kon_light_t *light, kon_renderFlags_t flags) {
	if ((flags & KON_RENDER_DEPTH) && !fb->depth) {
		fb->depth = malloc((size_t)fb->width * (size_t)fb->height * sizeof(float));
		if (!fb->depth) return;

		for (int i = 0; i < fb->width * fb->height; i++) {
			fb->depth[i] = 1.0f;
		}
	}

	kon_rasterVertex_t in[3], clipped[4];

	for (int i = 0; i < 3; i++) {
		kon_vec4_t clip = kon_mat4MulVec4(mvp, KON_VEC4(source[i]->position.x, source[i]->position.y, source[i]->position.z, 1));
		in[i].x = clip.x; in[i].y = clip.y; in[i].z = clip.z; in[i].w = clip.w;
		in[i].u = source[i]->uv.x;
		in[i].v = source[i]->uv.y;
		in[i].light = brightness[i];
	}

	int count = kon_clipNear_(in, clipped);
	for (int i = 1; i + 1 < count; i++) {
		kon_rasterizeTriangle_(fb, &clipped[0], &clipped[i], &clipped[i + 1], material, light, flags);
	}
}

static kon_vec3_t kon_faceNormal_(kon_vec3_t a, kon_vec3_t b, kon_vec3_t c) {
	return kon_vec3Normalize(kon_vec3Cross(kon_vec3Sub(b, a), kon_vec3Sub(c, a)));
}

void kon_drawTriangle3D(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_vertex_t a, kon_vertex_t b, kon_vertex_t c,
		const kon_material_t *material, const kon_light_t *light, kon_renderFlags_t flags) {
	if (!fb || !material) return;

	if (flags & KON_RENDER_WIREFRAME) {
		kon_drawLine3D(fb, viewProjection, a.position, b.position, material->color);
		kon_drawLine3D(fb, viewProjection, b.position, c.position, material->color);
		kon_drawLine3D(fb, viewProjection, c.position, a.position, material->color);
		return;
	}

	const kon_vertex_t *source[3] = {&a, &b, &c};
	double brightness[3] = {1.0, 1.0, 1.0};

	if (light) {
		kon_vec3_t toLight = kon_vec3Scale(kon_vec3Normalize(light->direction), -1.0f);

		if (flags & KON_RENDER_FLAT) {
			double factor = kon_lightFactor_(light, toLight, kon_faceNormal_(a.position, b.position, c.position));
			brightness[0] = brightness[1] = brightness[2] = factor;
		} else {
			for (int i = 0; i < 3; i++) brightness[i] = kon_lightFactor_(light, toLight, source[i]->normal);
		}
	}

	kon_submitTriangle_(fb, viewProjection, source, brightness, material, light, flags);
}

void kon_drawMesh(kon_framebuffer_t *fb, kon_mat4_t viewProjection, kon_mat4_t model, const kon_mesh_t *mesh,
		const kon_material_t *material, const kon_light_t *light, kon_renderFlags_t flags) {
	if (!fb || !mesh || !material) return;

	kon_mat4_t mvp = kon_mat4Mul(viewProjection, model);

	if (flags & KON_RENDER_WIREFRAME) light = NULL;

	kon_vec3_t toLight = KON_VEC3_ZERO;
	kon_vec3_t normalMatrix[3] = {KON_VEC3_ZERO, KON_VEC3_ZERO, KON_VEC3_ZERO};

	if (light) {
		toLight = kon_vec3Scale(kon_vec3Normalize(light->direction), -1.0f);

		/* the cofactor matrix keeps normals perpendicular to the surface even when the model is scaled unevenly */
		kon_vec3_t c0 = KON_VEC3(model.m[0], model.m[1], model.m[2]);
		kon_vec3_t c1 = KON_VEC3(model.m[4], model.m[5], model.m[6]);
		kon_vec3_t c2 = KON_VEC3(model.m[8], model.m[9], model.m[10]);
		float mirror = (kon_vec3Dot(c0, kon_vec3Cross(c1, c2)) < 0.0f) ? -1.0f : 1.0f;

		normalMatrix[0] = kon_vec3Scale(kon_vec3Cross(c1, c2), mirror);
		normalMatrix[1] = kon_vec3Scale(kon_vec3Cross(c2, c0), mirror);
		normalMatrix[2] = kon_vec3Scale(kon_vec3Cross(c0, c1), mirror);
	}

	for (int i = 0; i < mesh->triangleCount; i++) {
		const kon_vertex_t *source[3] = {
			&mesh->vertices[mesh->indices[i * 3 + 0]],
			&mesh->vertices[mesh->indices[i * 3 + 1]],
			&mesh->vertices[mesh->indices[i * 3 + 2]]
		};
		double brightness[3] = {1.0, 1.0, 1.0};

		if (flags & KON_RENDER_WIREFRAME) {
			kon_drawTriangle3D(fb, mvp, *source[0], *source[1], *source[2], material, NULL, flags);
			continue;
		}

		if (light) {
			if (flags & KON_RENDER_FLAT) {
				kon_vec3_t world[3];
				for (int j = 0; j < 3; j++) world[j] = kon_mat4MulPoint(model, source[j]->position);

				double factor = kon_lightFactor_(light, toLight, kon_faceNormal_(world[0], world[1], world[2]));
				brightness[0] = brightness[1] = brightness[2] = factor;
			} else {
				for (int j = 0; j < 3; j++) {
					kon_vec3_t n = source[j]->normal;
					kon_vec3_t worldNormal = kon_vec3Add(kon_vec3Add(kon_vec3Scale(normalMatrix[0], n.x), kon_vec3Scale(normalMatrix[1], n.y)), kon_vec3Scale(normalMatrix[2], n.z));
					brightness[j] = kon_lightFactor_(light, toLight, worldNormal);
				}
			}
		}

		kon_submitTriangle_(fb, mvp, source, brightness, material, light, flags);
	}
}

/*** built-in meshes ***/

static const kon_vertex_t kon_cubeVertices_[24] = {
	/* +Z */ {{-1, -1,  1}, {0, 0, 1}, {0, 1}}, {{ 1, -1,  1}, {0, 0, 1}, {1, 1}}, {{ 1,  1,  1}, {0, 0, 1}, {1, 0}}, {{-1,  1,  1}, {0, 0, 1}, {0, 0}},
	/* -Z */ {{ 1, -1, -1}, {0, 0, -1}, {0, 1}}, {{-1, -1, -1}, {0, 0, -1}, {1, 1}}, {{-1,  1, -1}, {0, 0, -1}, {1, 0}}, {{ 1,  1, -1}, {0, 0, -1}, {0, 0}},
	/* +X */ {{ 1, -1,  1}, {1, 0, 0}, {0, 1}}, {{ 1, -1, -1}, {1, 0, 0}, {1, 1}}, {{ 1,  1, -1}, {1, 0, 0}, {1, 0}}, {{ 1,  1,  1}, {1, 0, 0}, {0, 0}},
	/* -X */ {{-1, -1, -1}, {-1, 0, 0}, {0, 1}}, {{-1, -1,  1}, {-1, 0, 0}, {1, 1}}, {{-1,  1,  1}, {-1, 0, 0}, {1, 0}}, {{-1,  1, -1}, {-1, 0, 0}, {0, 0}},
	/* +Y */ {{-1,  1,  1}, {0, 1, 0}, {0, 1}}, {{ 1,  1,  1}, {0, 1, 0}, {1, 1}}, {{ 1,  1, -1}, {0, 1, 0}, {1, 0}}, {{-1,  1, -1}, {0, 1, 0}, {0, 0}},
	/* -Y */ {{-1, -1, -1}, {0, -1, 0}, {0, 1}}, {{ 1, -1, -1}, {0, -1, 0}, {1, 1}}, {{ 1, -1,  1}, {0, -1, 0}, {1, 0}}, {{-1, -1,  1}, {0, -1, 0}, {0, 0}}
};

static const uint16_t kon_cubeIndices_[36] = {
	0, 1, 2,  0, 2, 3,
	4, 5, 6,  4, 6, 7,
	8, 9, 10,  8, 10, 11,
	12, 13, 14,  12, 14, 15,
	16, 17, 18,  16, 18, 19,
	20, 21, 22,  20, 22, 23
};

const kon_mesh_t kon_cubeMesh = { kon_cubeVertices_, kon_cubeIndices_, 12 };

#endif /* end of KONSOFREN_IMPLEMENTATION */

#endif

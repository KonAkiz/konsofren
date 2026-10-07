/* SPDX-License-Identifier: MIT */
/*
 * konmath.h
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

#ifndef KONMATH_H
#define KONMATH_H

#include <math.h>

/*** definition ***/

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

/*** implementation ***/

/*** scalar ***/

static inline float kon_clamp(float v, float min, float max) { return v < min ? min : (v > max ? max : v); }
static inline float kon_lerp(float a, float b, float t) { return a + (b - a) * t; }

/*** vec2 ***/

static inline kon_vec2_t kon_vec2Add(kon_vec2_t a, kon_vec2_t b) { return (kon_vec2_t){a.x + b.x, a.y + b.y}; }
static inline kon_vec2_t kon_vec2Sub(kon_vec2_t a, kon_vec2_t b) { return (kon_vec2_t){a.x - b.x, a.y - b.y}; }
static inline kon_vec2_t kon_vec2Scale(kon_vec2_t a, float s) { return (kon_vec2_t){a.x * s, a.y * s}; }
static inline float kon_vec2Dot(kon_vec2_t a, kon_vec2_t b) { return a.x * b.x + a.y * b.y; }

static inline float kon_vec2Length(kon_vec2_t a) { return sqrtf(kon_vec2Dot(a, a)); }

static inline kon_vec2_t kon_vec2Normalize(kon_vec2_t a) {
	float len = kon_vec2Length(a);
	return len > 0.0f ? kon_vec2Scale(a, 1.0f / len) : a;
}

/*** vec3 ***/

static inline kon_vec3_t kon_vec3Add(kon_vec3_t a, kon_vec3_t b) { return (kon_vec3_t){a.x + b.x, a.y + b.y, a.z + b.z}; }
static inline kon_vec3_t kon_vec3Sub(kon_vec3_t a, kon_vec3_t b) { return (kon_vec3_t){a.x - b.x, a.y - b.y, a.z - b.z}; }
static inline kon_vec3_t kon_vec3Scale(kon_vec3_t a, float s) { return (kon_vec3_t){a.x * s, a.y * s, a.z * s}; }
static inline float kon_vec3Dot(kon_vec3_t a, kon_vec3_t b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

static inline kon_vec3_t kon_vec3Cross(kon_vec3_t a, kon_vec3_t b) {
	return (kon_vec3_t){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

static inline float kon_vec3Length(kon_vec3_t a) { return sqrtf(kon_vec3Dot(a, a)); }

static inline kon_vec3_t kon_vec3Normalize(kon_vec3_t a) {
	float len = kon_vec3Length(a);
	return len > 0.0f ? kon_vec3Scale(a, 1.0f / len) : a;
}

static inline kon_vec3_t kon_vec3Lerp(kon_vec3_t a, kon_vec3_t b, float t) {
	return (kon_vec3_t){kon_lerp(a.x, b.x, t), kon_lerp(a.y, b.y, t), kon_lerp(a.z, b.z, t)};
}

/*** mat4 ***/

static inline kon_mat4_t kon_mat4Identity(void) {
	kon_mat4_t r = {{0}};
	r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
	return r;
}

/* returns a * b, b is applied first */
static inline kon_mat4_t kon_mat4Mul(kon_mat4_t a, kon_mat4_t b) {
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

static inline kon_vec4_t kon_mat4MulVec4(kon_mat4_t m, kon_vec4_t v) {
	return (kon_vec4_t){
		m.m[0] * v.x + m.m[4] * v.y + m.m[8]  * v.z + m.m[12] * v.w,
		m.m[1] * v.x + m.m[5] * v.y + m.m[9]  * v.z + m.m[13] * v.w,
		m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
		m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
	};
}

static inline kon_vec3_t kon_mat4MulPoint(kon_mat4_t m, kon_vec3_t p) {
	kon_vec4_t r = kon_mat4MulVec4(m, (kon_vec4_t){p.x, p.y, p.z, 1.0f});
	return (kon_vec3_t){r.x, r.y, r.z};
}

static inline kon_mat4_t kon_mat4FromTranslation(kon_vec3_t v) {
	kon_mat4_t r = kon_mat4Identity();
	r.m[12] = v.x;
	r.m[13] = v.y;
	r.m[14] = v.z;
	return r;
}

static inline kon_mat4_t kon_mat4FromScale(kon_vec3_t v) {
	kon_mat4_t r = kon_mat4Identity();
	r.m[0] = v.x;
	r.m[5] = v.y;
	r.m[10] = v.z;
	return r;
}

static inline kon_mat4_t kon_mat4FromAxisAngle(kon_vec3_t axis, float angle) {
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
static inline kon_mat4_t kon_mat4FromRotation(kon_vec3_t angles) {
	kon_mat4_t x = kon_mat4FromAxisAngle(KON_VEC3(1, 0, 0), angles.x);
	kon_mat4_t y = kon_mat4FromAxisAngle(KON_VEC3(0, 1, 0), angles.y);
	kon_mat4_t z = kon_mat4FromAxisAngle(KON_VEC3(0, 0, 1), angles.z);
	return kon_mat4Mul(y, kon_mat4Mul(x, z));
}

static inline kon_mat4_t kon_mat4FromTransform(kon_vec3_t position, kon_vec3_t angles, kon_vec3_t scale) {
	kon_mat4_t r = kon_mat4Mul(kon_mat4FromRotation(angles), kon_mat4FromScale(scale));
	return kon_mat4Mul(kon_mat4FromTranslation(position), r);
}

static inline kon_mat4_t kon_mat4Translate(kon_mat4_t m, kon_vec3_t v) { return kon_mat4Mul(m, kon_mat4FromTranslation(v)); }
static inline kon_mat4_t kon_mat4Rotate(kon_mat4_t m, kon_vec3_t angles) { return kon_mat4Mul(m, kon_mat4FromRotation(angles)); }
static inline kon_mat4_t kon_mat4Scale(kon_mat4_t m, kon_vec3_t v) { return kon_mat4Mul(m, kon_mat4FromScale(v)); }
static inline kon_mat4_t kon_mat4TranslateWorld(kon_mat4_t m, kon_vec3_t v) { return kon_mat4Mul(kon_mat4FromTranslation(v), m); }

/* pre-multiplying would also swing the position around the world origin, so put it back */
static inline kon_mat4_t kon_mat4RotateWorld(kon_mat4_t m, kon_vec3_t angles) {
	kon_mat4_t r = kon_mat4Mul(kon_mat4FromRotation(angles), m);
	r.m[12] = m.m[12];
	r.m[13] = m.m[13];
	r.m[14] = m.m[14];
	return r;
}

static inline kon_mat4_t kon_mat4ScaleWorld(kon_mat4_t m, kon_vec3_t v) {
	kon_mat4_t r = kon_mat4Mul(kon_mat4FromScale(v), m);
	r.m[12] = m.m[12];
	r.m[13] = m.m[13];
	r.m[14] = m.m[14];
	return r;
}

static inline kon_mat4_t kon_mat4LookAt(kon_vec3_t eye, kon_vec3_t target, kon_vec3_t up) {
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
static inline kon_mat4_t kon_mat4Perspective(float fovY, float aspect, float nearZ, float farZ) {
	kon_mat4_t r = {{0}};
	float f = 1.0f / tanf(fovY * 0.5f);
	r.m[0] = f / aspect;
	r.m[5] = f;
	r.m[10] = (farZ + nearZ) / (nearZ - farZ);
	r.m[11] = -1.0f;
	r.m[14] = (2.0f * farZ * nearZ) / (nearZ - farZ);
	return r;
}

static inline kon_mat4_t kon_mat4Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) {
	kon_mat4_t r = kon_mat4Identity();
	r.m[0] = 2.0f / (right - left);
	r.m[5] = 2.0f / (top - bottom);
	r.m[10] = -2.0f / (farZ - nearZ);
	r.m[12] = -(right + left) / (right - left);
	r.m[13] = -(top + bottom) / (top - bottom);
	r.m[14] = -(farZ + nearZ) / (farZ - nearZ);
	return r;
}

#endif /* KONMATH_H */

#ifndef FONTS_H_
#define FONTS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	/* contains the width*/
	const uint8_t width;

	/* the actual character, represented bitwise for each pixel (1 = black, 0 = white)
	 *
	 *       <=== width ===>
	 *
	 *       a0  b0  ...  n0           ^
	 *       a1  b1  ...  n1           |
	 *       a2  b2  ...  n2           |
	 *       a3  b3  ...  n3           |
	 *       a4  b4  ...  n4 --+       |
	 *       a5  b5  ...  n5   |       |
	 *       a6  b6  ...  n6   |       |
	 *       a7  b7  ...  n7   |       |
	 * +-----------------------+       |
	 * |     o0  p0  ...  t0           |
	 * |     o1  p1  ...  t1           |
	 * |     o2  p2  ...  t2           h
	 * |     o3  p3  ...  t3           e
	 * +-->  o4  p4  ...  t4 --+       i  (global for font)
	 *       o5  p5  ...  t5   |       g  (in pixels)
	 *       o6  p6  ...  t6   |       h
	 *       o7  p7  ...  t7   |       t
	 * +-----------------------+       |
	 * |     u0  v0  ...  z0           |
	 * |     u1  v1  ...  z1           |
	 * |     u2  v2  ...  z2           |
	 * |     u3  v3  ...  z3           |
	 * +-->  u4  v4  ...  z4 --+       |
	 *       u5  v5  ...  z5   |       |
	 *       u6  v6  ...  z6   |       |
	 *       u7  v7  ...  z7   |       +
	 *                        ...
	 *
	 * The width of the character defines the number of bytes in each row,
	 * ceil(height/8) defines the number of bytes per column.
	 * The bytes a..z f the above figure are arranged in the format
	 * {a, b, c, ... , n, o, p, ... , t, u, v, ... , z}
	 *
	 * An image with size (width = 4, height = 9) will thus have 8 bytes
	 * {a, b, c, d, e, f, g, h} where a,b,c,d represent the pixels of
	 * the first eight rows and e, f, g, h represent the last row of
	 * pixels with the remaining bits == 0
	 */
	const uint8_t* pixeldata;
} character_t;

/** a font definition for ASCII characters.
 */
typedef struct {
	const uint8_t height;
	const uint8_t byteHeight; // how many rows of bytes in the pixeldata
	const character_t* chars[128];
} font_t;

/** returns the width of a given text in a given font */
int getTextWidth(char* string, font_t font);

#ifdef __cplusplus
}
#endif

#endif // #ifndef FONTS_H_

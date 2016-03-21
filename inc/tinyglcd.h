#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdio.h"

typedef enum
{
	or,
	xor
} glcdStyle_t;

void tglcd_init();

// returns first x coordinate after the text
int tglcd_drawTextOR(int x, int y, const font_t& font, const char* text, int clipToX = 128, int clipToY = 64);
int tglcd_drawTextXOR(int x, int y, const font_t& font, const char* text, int clipToX = 128, int clipToY = 64);

void tglcd_drawTextXCenteredOR(int x, int y, int width, const font_t& font, const char* text);
void tglcd_drawTextXCenteredXOR(int x, int y, int width, const font_t& font, const char* text);

void tglcd_drawTextRightAlignedOR(int right, int y, const font_t& font, const char* text);
void tglcd_drawTextRightAlignedXOR(int right, int y, const font_t& font, const char* text);

void tglcd_drawImageOR(int x, int y, const image_t& image, int clipToX = 128, int clipToY = 64);
void tglcd_drawImageXOR(int x, int y, const image_t& image, int clipToX = 128, int clipToY = 64);

void tglcd_drawMultiImageOR(int x, int y, const multiImage_t& image, int index, int clipToX = 128, int clipToY = 64);
void tglcd_drawMultiImageXOR(int x, int y, const multiImage_t& image, int index, int clipToX = 128, int clipToY = 64);

void tglcd_clearToWhite(int x, int y, int width, int height);
void tglcd_clearToBlack(int x, int y, int width, int height);

void tglcd_drawBoxWithBorder(int x, int y, int width, int height);

void tglcd_drawScrollBar(int x, int totalContentSize, int contentSizeVisible, int position);
void tglcd_drawScrollBarIfNeeded(int x, int totalContentSize, int contentSizeVisible, int position);

// draws an integer
void tglcd_drawIntOR(int x, int y, const font_t& font, int value, bool alwaysIncludeSign, int clipToX = 128, int clipToY = 64);
void tglcd_drawIntXOR(int x, int y, const font_t& font, int value, bool alwaysIncludeSign, int clipToX = 128, int clipToY = 64);
void tglcd_drawIntXCenteredXOR(int x, int y, int width, const font_t& font, int value, bool alwaysIncludeSign);
void tglcd_drawIntXCenteredOR(int x, int y, int width, const font_t& font, int value, bool alwaysIncludeSign);
void tglcd_drawIntRightAlignedOR(int right, int y, const font_t& font, int value, bool alwaysIncludeSign);
void tglcd_drawIntRightAlignedXOR(int right, int y, const font_t& font, int value, bool alwaysIncludeSign);

void tglcd_setPixel(int x, int y, bool shouldBeBlack);

void tglcd_loadWholeScreenFromBuffer(const uint8_t* pixeldata);

void tglcd_swapBuffers();


#endif

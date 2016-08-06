#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdio.h"
#include <tinyglcd_config.h>
#include <tinyglcd_fonts.h>
#include <tinyglcd_images.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*typedef enum
     {
     or,
     xor
     } glcdStyle_t;*/
    
    void tglcd_init();
    
    void tglcd_clearDrawBuffer();
    
    // returns first x coordinate after the text
    int tglcd_drawTextOR(int x, int y, const font_t* font, const char* text, int clipToX, int clipToY);
    int tglcd_drawTextXOR(int x, int y, const font_t* font, const char* text, int clipToX, int clipToY);
    
    void tglcd_drawTextXCenteredOR(int x, int y, int width, const font_t* font, const char* text);
    void tglcd_drawTextXCenteredXOR(int x, int y, int width, const font_t* font, const char* text);
    
    void tglcd_drawTextRightAlignedOR(int right, int y, const font_t* font, const char* text);
    void tglcd_drawTextRightAlignedXOR(int right, int y, const font_t* font, const char* text);
    
    void tglcd_drawImageOR(int x, int y, const image_t* image, int clipToX, int clipToY);
    void tglcd_drawImageXOR(int x, int y, const image_t* image, int clipToX, int clipToY);
    
    void tglcd_drawMultiImageOR(int x, int y, const multiImage_t* image, int index, int clipToX, int clipToY);
    void tglcd_drawMultiImageXOR(int x, int y, const multiImage_t* image, int index, int clipToX, int clipToY);
    
    void tglcd_clearToWhite(int x, int y, int width, int height);
    void tglcd_clearToBlack(int x, int y, int width, int height);
    
    void tglcd_drawBoxWithBorder(int x, int y, int width, int height);
    
    void tglcd_drawScrollBar(int x, int totalContentSize, int contentSizeVisible, int position);
    void tglcd_drawScrollBarIfNeeded(int x, int totalContentSize, int contentSizeVisible, int position);
    
    // draws an integer
    void tglcd_drawIntOR(int x, int y, const font_t* font, int value, int alwaysIncludeSign, int clipToX, int clipToY);
    void tglcd_drawIntXOR(int x, int y, const font_t* font, int value, int alwaysIncludeSign, int clipToX, int clipToY);
    void tglcd_drawIntXCenteredXOR(int x, int y, int width, const font_t* font, int value, int alwaysIncludeSign);
    void tglcd_drawIntXCenteredOR(int x, int y, int width, const font_t* font, int value, int alwaysIncludeSign);
    void tglcd_drawIntRightAlignedOR(int right, int y, const font_t* font, int value, int alwaysIncludeSign);
    void tglcd_drawIntRightAlignedXOR(int right, int y, const font_t* font, int value, int alwaysIncludeSign);
    
    void tglcd_setPixel(int x, int y, int shouldBeBlack);
    
    void tglcd_loadWholeScreenFromBuffer(const uint8_t* pixeldata);
    
    void tglcd_swapBuffers();
    const uint8_t* tglcd_getCurrentDisplayBuffer();
    
#ifdef __cplusplus
}
#endif


#endif

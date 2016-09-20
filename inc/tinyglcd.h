#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdio.h"
#include <tinyglcd_config.h>
#include <tinyglcd_fonts.h>
#include <tinyglcd_images.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    void tglcd_init();
    
    void tglcd_clearScreen();
    
    int tglcd_getTextWidth(const font_t* font, const char* text);
    
    // returns first x coordinate after the text
    int tglcd_drawTextOR(int x, int y, const font_t* font, const char* text, int clipToX, int clipToY);
    int tglcd_drawTextXOR(int x, int y, const font_t* font, const char* text, int clipToX, int clipToY);
    void tglcd_drawTextXCenteredOR(int x, int y, int width, const font_t* font, const char* text, int clipToY);
    void tglcd_drawTextXCenteredXOR(int x, int y, int width, const font_t* font, const char* text, int clipToY);
    void tglcd_drawTextRightAlignedOR(int right, int y, const font_t* font, const char* text, int maximumWidth, int clipToY);
    void tglcd_drawTextRightAlignedXOR(int right, int y, const font_t* font, const char* text, int maximumWidth, int clipToY);
    // draws multiple lines of text. Line breaks are done on the \n character. if lineHeight == 0, the default height of the
    // font is used.
    void tglcd_drawMultiLineTextOR(int x, int y, int width, const font_t* font, const char* text, int clipToY, int lineHeight);
    void tglcd_drawMultiLineTextXOR(int x, int y, int width, const font_t* font, const char* text, int clipToY, int lineHeight);
    
    void tglcd_drawImageOR(int x, int y, const image_t* image, int clipToX, int clipToY);
    void tglcd_drawImageXOR(int x, int y, const image_t* image, int clipToX, int clipToY);
    void tglcd_drawMultiImageOR(int x, int y, const multiImage_t* image, int index, int clipToX, int clipToY);
    void tglcd_drawMultiImageXOR(int x, int y, const multiImage_t* image, int index, int clipToX, int clipToY);
    
    void tglcd_clearToWhite(int x, int y, int width, int height);
    void tglcd_clearToBlack(int x, int y, int width, int height);
    
    void tglcd_drawBoxWithBorder(int x, int y, int width, int height);
    
    void tglcd_drawDottedVerticalLineOR(int x, int y, int height);
    void tglcd_drawDottedHorizontalLineOR(int x, int y, int width);
    void tglcd_drawVerticalLineOR(int x, int y, int height);
    void tglcd_drawHorizontalLineOR(int x, int y, int width);
    void tglcd_drawDottedVerticalLineXOR(int x, int y, int height);
    void tglcd_drawDottedHorizontalLineXOR(int x, int y, int width);
    void tglcd_drawVerticalLineXOR(int x, int y, int height);
    void tglcd_drawHorizontalLineXOR(int x, int y, int width);
    
    // draws an integer
    void tglcd_drawIntOR(int x, int y, const font_t* font, int value, int alwaysIncludeSign, int clipToX, int clipToY);
    void tglcd_drawIntXOR(int x, int y, const font_t* font, int value, int alwaysIncludeSign, int clipToX, int clipToY);
    void tglcd_drawIntXCenteredXOR(int x, int y, int width, const font_t* font, int value, int clipToY, int alwaysIncludeSign);
    void tglcd_drawIntXCenteredOR(int x, int y, int width, const font_t* font, int value, int clipToY, int alwaysIncludeSign);
    void tglcd_drawIntRightAlignedOR(int right, int y, const font_t* font, int value, int maximumWidth, int clipToY, int alwaysIncludeSign);
    void tglcd_drawIntRightAlignedXOR(int right, int y, const font_t* font, int value, int maximumWidth, int clipToY, int alwaysIncludeSign);
    
    void tglcd_setPixel(int x, int y, int shouldBeBlack);
    
    void tglcd_grayOutEntireScreen();
    
    void tglcd_loadWholeScreenFromBuffer(const uint8_t* pixeldata);
    
    void tglcd_swapBuffers();
    const uint8_t* tglcd_getCurrentDisplayBuffer();
    
#ifdef __cplusplus
}

/** this class conveniently exposes the C-interface as a C++ class,
 * making use of default parameters to shrink down the function calls.
 */
class TGLCDPainter
{
public:       
    void setPixel(int x, int y, bool shouldBeBlack)
    {
        tglcd_setPixel(x, y, shouldBeBlack);
    }
    
    void loadWholeScreenFromBuffer(const uint8_t* pixeldata)
    {
        tglcd_loadWholeScreenFromBuffer(pixeldata);
    }
    
    void clearScreen()
    {
        tglcd_clearScreen();
    }
    
    void clearToWhite(int x = 0, int y = 0, int width = TINYGLCD_SCREEN_WIDTH, int height = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_clearToWhite(x, y, width, height);
    }
    
    void clearToBlack(int x = 0, int y = 0, int width = TINYGLCD_SCREEN_WIDTH, int height = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_clearToBlack(x, y, width, height);
    }
    
    void drawBoxWithBorder(int x = 0, int y = 0, int width = TINYGLCD_SCREEN_WIDTH, int height = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawBoxWithBorder(x, y, width, height);
    }
    
    int getTextWidth(const font_t* font, const char* text)
    {
        return tglcd_getTextWidth(font, text);
    }
    
    int drawTextOR(int x,
                   int y,
                   const font_t* font,
                   const char* text,
                   int clipToX = TINYGLCD_SCREEN_WIDTH,
                   int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        return tglcd_drawTextOR(x, y, font, text, clipToX, clipToY);
    }
    
    int drawTextXOR(int x,
                    int y,
                    const font_t* font,
                    const char* text,
                    int clipToX = TINYGLCD_SCREEN_WIDTH,
                    int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        return tglcd_drawTextXOR(x, y, font, text, clipToX, clipToY);
    }
    
    
    void drawTextXCenteredOR(int x, int y, int width, const font_t* font, const char* text, int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawTextXCenteredOR(x, y, width, font, text, clipToY);
    }
    
    void drawTextXCenteredXOR(int x, int y, int width, const font_t* font, const char* text, int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawTextXCenteredXOR(x, y, width, font, text, clipToY);
    }
    
    void drawTextRightAlignedOR(int right, int y, const font_t* font, const char* text, int maximumWidth = 0, int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        if (maximumWidth == 0)
            maximumWidth = right;
        tglcd_drawTextRightAlignedOR(right, y, font, text, maximumWidth, clipToY);
    }
    
    void drawTextRightAlignedXOR(int right, int y, const font_t* font, const char* text, int maximumWidth = 0, int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        if (maximumWidth == 0)
            maximumWidth = right;
        tglcd_drawTextRightAlignedXOR(right, y, font, text, maximumWidth, clipToY);
    }
    
    void drawMultiLineTextOR(int x,
                             int y,
                             int width,
                             const font_t* font,
                             const char* text,
                             int clipToY = TINYGLCD_SCREEN_HEIGHT,
                             int lineHeight = 0)
    {
        tglcd_drawMultiLineTextOR(x, y, width, font, text, clipToY, lineHeight);
    }
    
    void drawMultiLineTextXOR(int x,
                              int y,
                              int width,
                              const font_t* font,
                              const char* text,
                              int clipToY = TINYGLCD_SCREEN_HEIGHT,
                              int lineHeight = 0)
    {
        tglcd_drawMultiLineTextXOR(x, y, width, font, text, clipToY, lineHeight);
    }
    
    void drawImageOR(int x,
                     int y,
                     const image_t* image,
                     int clipToX = TINYGLCD_SCREEN_WIDTH,
                     int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawImageOR(x, y, image, clipToX, clipToY);
    }
    
    void drawImageXOR(int x,
                      int y,
                      const image_t* image,
                      int clipToX = TINYGLCD_SCREEN_WIDTH,
                      int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawImageXOR(x, y, image, clipToX, clipToY);
    }
    
    void drawMultiImageOR(int x,
                          int y,
                          const multiImage_t* image,
                          int index,
                          int clipToX = TINYGLCD_SCREEN_WIDTH,
                          int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawMultiImageOR(x, y, image, index, clipToX, clipToY);
    }
    
    void drawMultiImageXOR(int x,
                           int y,
                           const multiImage_t* image,
                           int index,
                           int clipToX = TINYGLCD_SCREEN_WIDTH,
                           int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawMultiImageXOR(x, y, image, index, clipToX, clipToY);
    }
    
    // draws an integer
    void drawIntOR(int x,
                   int y,
                   const font_t* font,
                   int value,
                   bool alwaysIncludeSign = false,
                   int clipToX = TINYGLCD_SCREEN_WIDTH,
                   int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawIntOR(x, y, font, value, alwaysIncludeSign?1:0, clipToX, clipToY);
    }
    
    void drawIntXOR(int x,
                    int y,
                    const font_t* font,
                    int value,
                    bool alwaysIncludeSign = false,
                    int clipToX = TINYGLCD_SCREEN_WIDTH,
                    int clipToY = TINYGLCD_SCREEN_HEIGHT)
    {
        tglcd_drawIntXOR(x, y, font, value, alwaysIncludeSign?1:0, clipToX, clipToY);
    }
    
    void drawIntXCenteredOR(int x,
                            int y,
                            int width,
                            const font_t* font,
                            int value,
                            int clipToY = TINYGLCD_SCREEN_HEIGHT,
                            bool alwaysIncludeSign = false)
    {
        tglcd_drawIntXCenteredOR(x, y, width, font, value, clipToY, alwaysIncludeSign?1:0);
    }
    
    void drawIntXCenteredXOR(int x,
                             int y,
                             int width,
                             const font_t* font,
                             int value,
                             int clipToY = TINYGLCD_SCREEN_HEIGHT,
                             bool alwaysIncludeSign = false)
    {
        tglcd_drawIntXCenteredXOR(x, y, width, font, value, clipToY, alwaysIncludeSign?1:0);
    }
    
    void drawIntRightAlignedOR(int right,
                               int y,
                               const font_t* font,
                               int value,
                               int maximumWidth = 0,
                               int clipToY = TINYGLCD_SCREEN_HEIGHT,
                               bool alwaysIncludeSign = false)
    {
        if (maximumWidth == 0)
            maximumWidth = right;
        tglcd_drawIntRightAlignedOR(right, y, font, value, maximumWidth, clipToY, alwaysIncludeSign?1:0);
    }
    
    void drawIntRightAlignedXOR(int right,
                                int y,
                                const font_t* font,
                                int value,
                                int maximumWidth = 0,
                                int clipToY = TINYGLCD_SCREEN_HEIGHT,
                                bool alwaysIncludeSign = false)
    {
        if (maximumWidth == 0)
            maximumWidth = right;
        tglcd_drawIntRightAlignedXOR(right, y, font, value, maximumWidth, clipToY, alwaysIncludeSign?1:0);
    }
    
    void drawDottedVerticalLineOR(int x, int y, int height)
    {
        tglcd_drawDottedVerticalLineOR(x, y, height);
    }
    
    void drawDottedHorizontalLineOR(int x, int y, int width)
    {
        tglcd_drawDottedHorizontalLineOR(x, y, width);
    }
    
    void drawVerticalLineOR(int x, int y, int height)
    {
        tglcd_drawVerticalLineOR(x, y, height);
    }
    
    void drawHorizontalLineOR(int x, int y, int width)
    {
        tglcd_drawHorizontalLineOR(x, y, width);
    }
    
    void drawDottedVerticalLineXOR(int x, int y, int height)
    {
        tglcd_drawDottedVerticalLineXOR(x, y, height);
    }
    
    void drawDottedHorizontalLineXOR(int x, int y, int width)
    {
        tglcd_drawDottedHorizontalLineXOR(x, y, width);
    }
    
    void drawVerticalLineXOR(int x, int y, int height)
    {
        tglcd_drawVerticalLineXOR(x, y, height);
    }
    
    void drawHorizontalLineXOR(int x, int y, int width)
    {
        tglcd_drawHorizontalLineXOR(x, y, width);
    }
    
    void grayOutEntireScreen()
    {
        tglcd_grayOutEntireScreen();
    }
    
    
};

#endif


#endif

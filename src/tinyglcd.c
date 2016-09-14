#include <tinyglcd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TINYGLCD_BUFFER_HEIGHT ((TINYGLCD_SCREEN_HEIGHT+7) >> 3)
#define TINYGLCD_BUFFER_SIZE (TINYGLCD_SCREEN_WIDTH * TINYGLCD_BUFFER_HEIGHT)

static uint8_t* currentDrawBuffer;
static uint8_t* currentSendBuffer;
static uint8_t bufferA[TINYGLCD_BUFFER_SIZE];
static uint8_t bufferB[TINYGLCD_BUFFER_SIZE];


/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int i, j;
    char c;
    
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoaEnforceSign(int n, char s[])
{
    int i, sign;
    
    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    
    if (sign < 0)
        s[i++] = '-';
    else
        s[i++] = '+';
    s[i] = '\0';
    reverse(s);
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;
    
    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


void tglcd_clearBuffer(uint8_t* b)
{
	for (int i = 0; i < TINYGLCD_BUFFER_SIZE; i++)
		b[i] = 0x00;
}

void tglcd_init()
{
    tglcd_clearBuffer(bufferA);
    tglcd_clearBuffer(bufferB);
    currentDrawBuffer = bufferA;
    currentSendBuffer = bufferB;
}

void tglcd_clearScreen()
{
    tglcd_clearBuffer(currentDrawBuffer);
}

void tglcd_loadWholeScreenFromBuffer(const uint8_t* pixeldata)
{
	memcpy(currentDrawBuffer, pixeldata, TINYGLCD_BUFFER_SIZE);
}



void tglcd_setPixel(int x, int y, int shouldBeBlack)
{
	if (y >= TINYGLCD_SCREEN_HEIGHT)
		return;
	if (x >= TINYGLCD_SCREEN_WIDTH)
		return;

	int yByte = y / 8;
	int yBit = y - yByte * 8;
	int index = yByte * TINYGLCD_SCREEN_WIDTH + x;

	if (shouldBeBlack)
		currentDrawBuffer[index] |= (0x01 << yBit);
	else
		currentDrawBuffer[index] &= ~(0x01 << yBit);
}

void tglcd_drawPixelbufferXOR(int xPos, int yPos, const uint8_t* pxbuffer, int width, int height, int byteHeight, int clipToX, int clipToY)
{
	int startPage = yPos / 8;
	int yBit = yPos - startPage*8;

	/*if (yPos + height >= clipToY)
	{
		height = clipToY - yPos;
	}*/

	for (int x = 0; x < width; x++)
	{
        /* take care of clipping in the x direction */
		if (x + xPos >= clipToX)
			break;
		if (x + xPos < 0)
			continue;

		for (int yByte = 0; yByte < byteHeight; yByte++)
		{
            /* take care of clipping in the y direction */
            int clipByte = clipToY / 8;
            int clipBit = clipToY - clipByte * 8;
            
            /* everything after this will be clipped away */
            if (startPage + yByte > clipByte)
                break;
            
            if (clipByte == yByte + startPage)
            {
                uint8_t mask = 0xFF >> ( 8 - clipBit );
                currentDrawBuffer[(yByte + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] ^= ((uint8_t) pxbuffer[yByte*width + x] << yBit) & mask;
            }
            else if (clipByte == yByte + startPage + 1)
            {
                currentDrawBuffer[(yByte + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] ^= (uint8_t) pxbuffer[yByte*width + x] << yBit;
                if ((yBit != 0) && (yByte + 1 + startPage < 8))
                {
                    uint8_t mask = 0xFF >> ( 8 - clipBit );
                    currentDrawBuffer[(yByte + 1 + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] ^= ((uint8_t) pxbuffer[yByte*width + x] >> (8 - yBit)) & mask;
                }
            }
            else
            {
                currentDrawBuffer[(yByte + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] ^= (uint8_t) pxbuffer[yByte*width + x] << yBit;
                if ((yBit != 0) && (yByte + 1 + startPage < 8))
                {
                    currentDrawBuffer[(yByte + 1 + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] ^= (uint8_t) pxbuffer[yByte*width + x] >> (8 - yBit);
                }
            }
        }
	}
}

void tglcd_drawPixelbufferOR(int xPos, int yPos, const uint8_t* pxbuffer, int width, int height, int byteHeight, int clipToX, int clipToY)
{
	int startPage = yPos / 8;
	int yBit = yPos - startPage*8;

	if (yPos + height >= clipToY)
	{
		height = clipToY - yPos;
	}

	for (int x = 0; x < width; x++)
	{
		if (x + xPos >= clipToX)
			break;
		if (x + xPos < 0)
			continue;

		for (int yByte = 0; yByte < byteHeight; yByte++)
		{
			if (yByte + startPage >= 8)
				break;

			currentDrawBuffer[(yByte + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] |= (uint8_t) pxbuffer[yByte*width + x] << yBit;
			if ((yBit != 0) && (yByte + 1 + startPage < 8))
			{
				currentDrawBuffer[(yByte + 1 + startPage)*TINYGLCD_SCREEN_WIDTH + x + xPos] |= (uint8_t) pxbuffer[yByte*width + x] >> (8 - yBit);
			}
		}
	}
}

int tglcd_drawTextOR(int x, int y, const font_t* font, const char* text, int clipToX, int clipToY)
{
	while (*text)
	{
		if (x >= clipToX)
			break;
		const character_t* ch = font->chars[*text & 0x7F];
		tglcd_drawPixelbufferOR(x, y, ch->pixeldata, ch->width, font->height, font->byteHeight, clipToX, clipToY);
		x += ch->width;
		text++;
	}
	return x;
}

int tglcd_drawTextXOR(int x, int y, const font_t* font, const char* text, int clipToX, int clipToY)
{
	while (*text)
	{
		if (x >= clipToX)
			break;
		const character_t* ch = font->chars[*text & 0x7F];
		tglcd_drawPixelbufferXOR(x, y, ch->pixeldata, ch->width, font->height, font->byteHeight, clipToX, clipToY);
		x += ch->width;
		text++;
	}
	return x;
}

void tglcd_drawTextXCenteredOR(int x, int y, int width, const font_t* font, const char* text)
{
	int textWidth = 0;
	const char* dummy = text;
	while (*dummy)
	{
		const character_t* ch = font->chars[*dummy & 0x7F];
		textWidth += ch->width;
		dummy++;
	}

	int offsetX = (width - textWidth) / 2;

	while (*text)
	{
		const character_t* ch = font->chars[*text & 0x7F];
		tglcd_drawPixelbufferOR(x + offsetX, y, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, TINYGLCD_SCREEN_HEIGHT);
		x += ch->width;
		text++;
	}
}

void tglcd_drawTextXCenteredXOR(int x, int y, int width, const font_t* font, const char* text)
{
	int textWidth = 0;
	const char* dummy = text;
	while (*dummy)
	{
		const character_t* ch = font->chars[*dummy & 0x7F];
		textWidth += ch->width;
		dummy++;
	}

	int offsetX = (width - textWidth) / 2;

	while (*text)
	{
		const character_t* ch = font->chars[*text & 0x7F];
		tglcd_drawPixelbufferXOR(x + offsetX, y, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, TINYGLCD_SCREEN_HEIGHT);
		x += ch->width;
		text++;
	}
}

void tglcd_drawTextRightAlignedOR(int right, int y, const font_t* font, const char* text)
{
	int textWidth = 0;
	const char* dummy = text;
	while (*dummy)
	{
		const character_t* ch = font->chars[*dummy & 0x7F];
		textWidth += ch->width;
		dummy++;
	}

	int x = right - textWidth + 1;

	while (*text)
	{
		const character_t* ch = font->chars[*text & 0x7F];
		tglcd_drawPixelbufferOR(x, y, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, TINYGLCD_SCREEN_HEIGHT);
		x += ch->width;
		text++;
	}
}

void tglcd_drawTextRightAlignedXOR(int right, int y, const font_t* font, const char* text)
{
	int textWidth = 0;
	const char* dummy = text;
	while (*dummy)
	{
		const character_t* ch = font->chars[*dummy & 0x7F];
		textWidth += ch->width;
		dummy++;
	}

	int x = right - textWidth + 1;

	while (*text)
	{
		const character_t* ch = font->chars[*text & 0x7F];
		tglcd_drawPixelbufferXOR(x, y, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, TINYGLCD_SCREEN_HEIGHT);
		x += ch->width;
		text++;
	}
}

void tglcd_drawMultiLineTextOR(int x, int y, int width, const font_t* font, const char* text, int clipToY, int lineHeight)
{
    int realLineHeight = (lineHeight == 0)?font->height + 2:lineHeight;
    int currentY = y;
    if (currentY > clipToY)
        return;
    int currentX = 0;
    int currentLine = 0;
    const char* unwrittenText = text;
    
    while (*unwrittenText)
    {
        // find next space symbol (or the end of the text)
        const char* wordStart = unwrittenText;
        int textWidth = 0;
        while (*unwrittenText != ' ' && *unwrittenText != '\n' && *unwrittenText)
        {
            const character_t* ch = font->chars[*unwrittenText & 0x7F];
            textWidth += ch->width;
            unwrittenText++;
        }
        
        // will the end of the next word be off the screen?
        if ((currentX + textWidth > width) || (*unwrittenText == '\n'))
        {
            currentLine++;
            currentX = 0;
            currentY += realLineHeight;
            if (currentY > clipToY)
                break;
        }
        
        // draw the word
        const char* i = wordStart;
        while (i != unwrittenText)
        {
            const character_t* ch = font->chars[*i & 0x7F];
            tglcd_drawPixelbufferOR(x + currentX, currentY, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, clipToY);
            currentX += ch->width;
            i++;
        }
        
        // add a space symbol
        if (*unwrittenText == ' ')
        {
            const character_t* ch = font->chars[*unwrittenText & 0x7F];
            tglcd_drawPixelbufferOR(x + currentX, currentY, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, clipToY);
            currentX += ch->width;
            unwrittenText++;
        }
        // skip the newline symbol
        else if (*unwrittenText == '\n')
            unwrittenText++;
    }
}

void tglcd_drawMultiLineTextXOR(int x, int y, int width, const font_t* font, const char* text, int clipToY, int lineHeight)
{
    int realLineHeight = (lineHeight == 0)?font->height + 2:lineHeight;
    int currentY = y;
    if (currentY > clipToY)
        return;
    int currentX = 0;
    int currentLine = 0;
    const char* unwrittenText = text;
    
    while (*unwrittenText)
    {
        // find next space symbol (or the end of the text)
        const char* wordStart = unwrittenText;
        int textWidth = 0;
        while (*unwrittenText != ' ' && *unwrittenText != '\n' && *unwrittenText)
        {
            const character_t* ch = font->chars[*unwrittenText & 0x7F];
            textWidth += ch->width;
            unwrittenText++;
        }
        
        // will the end of the next word be off the screen?
        if ((currentX + textWidth > width) || (*unwrittenText == '\n'))
        {
            currentLine++;
            currentX = 0;
            currentY += realLineHeight;
            if (currentY > clipToY)
                break;
        }
        
        // draw the word
        const char* i = wordStart;
        while (i != unwrittenText)
        {
            const character_t* ch = font->chars[*i & 0x7F];
            tglcd_drawPixelbufferXOR(x + currentX, currentY, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, clipToY);
            currentX += ch->width;
            i++;
        }
        
        // add a space symbol
        if (*unwrittenText == ' ')
        {
            const character_t* ch = font->chars[*unwrittenText & 0x7F];
            tglcd_drawPixelbufferXOR(x + currentX, currentY, ch->pixeldata, ch->width, font->height, font->byteHeight, TINYGLCD_SCREEN_WIDTH, clipToY);
            currentX += ch->width;
            unwrittenText++;
        }
        // skip the newline symbol
        else if (*unwrittenText == '\n')
            unwrittenText++;
    }
}

void tglcd_drawImageOR(int x, int y, const image_t* image, int clipToX, int clipToY)
{
	tglcd_drawPixelbufferOR(x, y, image->pixeldata, image->width, image->height, image->byteHeight, clipToX, clipToY);
}

void tglcd_drawImageXOR(int x, int y,const image_t* image, int clipToX, int clipToY)
{
	tglcd_drawPixelbufferXOR(x, y, image->pixeldata, image->width, image->height, image->byteHeight, clipToX, clipToY);
}

void tglcd_drawMultiImageOR(int x, int y, const multiImage_t* image, int index, int clipToX, int clipToY)
{
	if (index > image->numImages)
		return;
	tglcd_drawPixelbufferOR(x, y, image->pixeldata[index], image->width, image->height, image->byteHeight, clipToX, clipToY);
}

void tglcd_drawMultiImageXOR(int x, int y, const multiImage_t* image, int index, int clipToX, int clipToY)
{
	if (index > image->numImages)
		return;
	tglcd_drawPixelbufferXOR(x, y, image->pixeldata[index], image->width, image->height, image->byteHeight, clipToX, clipToY);
}

void tglcd_clearToWhite(int xPos, int yPos, int width, int height)
{
    if (xPos < 0)
    {
        width += xPos;
        xPos = 0;
    }
    if (yPos < 0)
    {
        height += yPos;
        yPos = 0;
    }
    
    if (height + yPos > TINYGLCD_SCREEN_HEIGHT)
    {
        height = TINYGLCD_SCREEN_HEIGHT - 1 - yPos;
    }
    
    int firstByte = ( yPos ) >> 3;
    int firstBit = yPos - (firstByte << 3);
    int lastByte = ( yPos + height ) >> 3;
    int lastBit = yPos + height - (lastByte << 3);
    
    if (firstByte != lastByte)
    {
        uint8_t mask = ~(0xFF << firstBit);
        for ( int x = xPos; x < xPos + width; x++ )
        {
            currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) mask;
        }
        
        for ( int hbyte = firstByte + 1; hbyte < lastByte; hbyte++ )
        {
            int baseIndex = hbyte * TINYGLCD_SCREEN_WIDTH;
            for ( int x = xPos; x < xPos + width; x++ )
            {
                if ( x >= TINYGLCD_SCREEN_WIDTH )
                    break;
                currentDrawBuffer[baseIndex + x] = 0x00;
            }
        }
        
        mask = ~(0xFF >> (8 - lastBit));
        for ( int x = xPos; x < xPos + width; x++ )
        {
            currentDrawBuffer[lastByte * TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) mask;
        }
    }
    else
    {
        uint8_t mask = ~(0xFF << firstBit);
        mask = mask >> (8 - lastBit);
        for ( int x = xPos; x < xPos + width; x++ )
        {
            currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) mask;
        }
    }
}


void tglcd_clearToBlack(int xPos, int yPos, int width, int height)
{
	if (xPos < 0)
	{
		width += xPos;
		xPos = 0;
	}
	if (yPos < 0)
	{
		height += yPos;
		yPos = 0;
	}

	if (height + yPos > TINYGLCD_SCREEN_HEIGHT)
	{
		height = TINYGLCD_SCREEN_HEIGHT - 1 - yPos;
	}

	int firstByte = ( yPos ) >> 3;
    int firstBit = yPos - (firstByte << 3);
    int lastByte = ( yPos + height ) >> 3;
    int lastBit = yPos + height - (lastByte << 3);

    if (firstByte != lastByte)
    {
        uint8_t mask = 0xFF << firstBit;
        for ( int x = xPos; x < xPos + width; x++ )
        {
            currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
        }
        
        for ( int hbyte = firstByte + 1; hbyte < lastByte; hbyte++ )
        {
            int baseIndex = hbyte * TINYGLCD_SCREEN_WIDTH;
            for ( int x = xPos; x < xPos + width; x++ )
            {
                if ( x >= TINYGLCD_SCREEN_WIDTH )
                    break;
                currentDrawBuffer[baseIndex + x] = 0xFF;
            }
        }
        
        mask = 0xFF >> (8 - lastBit);
        for ( int x = xPos; x < xPos + width; x++ )
        {
            currentDrawBuffer[lastByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
        }
    }
    else
    {
        uint8_t mask = 0xFF << firstBit;
        mask = mask >> (8 - lastBit);
        for ( int x = xPos; x < xPos + width; x++ )
        {
            currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
        }
    }
}

void tglcd_drawDottedVerticalLineOR(int x, int y, int height)
{
    if (x < 0)
        return;
    
    if (y < 0)
    {
        height += y;
        y = 0;
    }
    
    if (height + y > TINYGLCD_SCREEN_HEIGHT)
    {
        height = TINYGLCD_SCREEN_HEIGHT - 1 - y;
    }
    
    int firstByte = y >> 3;
    int firstBit = y - (firstByte << 3);
    int lastByte = ( y + height ) >> 3;
    int lastBit = y + height - (lastByte << 3);
    
    // created the dots in the masks
    uint8_t maskMask = (y & 0x01)?0b10101010:0b01010101;
    
    if (firstByte != lastByte)
    {
        uint8_t mask = 0xFF << firstBit;
        mask &= maskMask;
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
        
        for ( int hbyte = firstByte + 1; hbyte < lastByte; hbyte++ )
        {
            int baseIndex = hbyte * TINYGLCD_SCREEN_WIDTH;
            currentDrawBuffer[baseIndex + x] = maskMask;
        }
        
        mask = 0xFF >> (8 - lastBit);
        mask &= maskMask;
        currentDrawBuffer[lastByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
    }
    else
    {
        uint8_t mask = 0xFF << firstBit;
        mask = mask >> (8 - lastBit);
        mask &= maskMask;
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
    }
}

void tglcd_drawDottedHorizontalLineOR(int x, int y, int width)
{
    int byte = y >> 3;
    int bit = y - (byte << 3);
    uint8_t mask = 0x01 << bit;
    
    for (int i = x; i < x + width; i+=2)
        currentDrawBuffer[byte * TINYGLCD_SCREEN_WIDTH + i] |= (uint8_t) mask;
}

void tglcd_drawVerticalLineOR(int x, int y, int height)
{
    if (x < 0)
        return;
    
    if (y < 0)
    {
        height += y;
        y = 0;
    }
    
    if (height + y > TINYGLCD_SCREEN_HEIGHT)
    {
        height = TINYGLCD_SCREEN_HEIGHT - 1 - y;
    }
    
    int firstByte = ( y ) >> 3;
    int firstBit = y - (firstByte << 3);
    int lastByte = ( y + height ) >> 3;
    int lastBit = y + height - (lastByte << 3);
    
    if (firstByte != lastByte)
    {
        uint8_t mask = 0xFF << firstBit;
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
        
        for ( int hbyte = firstByte + 1; hbyte < lastByte; hbyte++ )
        {
            int baseIndex = hbyte * TINYGLCD_SCREEN_WIDTH;
            currentDrawBuffer[baseIndex + x] = 0xFF;
        }
        
        mask = 0xFF >> (8 - lastBit);
        currentDrawBuffer[lastByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
    }
    else
    {
        uint8_t mask = 0xFF << firstBit;
        mask = mask >> (8 - lastBit);
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) mask;
    }
}

void tglcd_drawHorizontalLineOR(int x, int y, int width)
{
    int byte = y >> 3;
    int bit = y - (byte << 3);
    uint8_t mask = 0x01 << bit;
    
    for (int i = x; i < x + width; i++)
        currentDrawBuffer[byte * TINYGLCD_SCREEN_WIDTH + i] |= (uint8_t) mask;
}

void tglcd_drawDottedVerticalLineXOR(int x, int y, int height)
{
    if (x < 0)
        return;
    
    if (y < 0)
    {
        height += y;
        y = 0;
    }
    
    if (height + y > TINYGLCD_SCREEN_HEIGHT)
    {
        height = TINYGLCD_SCREEN_HEIGHT - 1 - y;
    }
    
    int firstByte = y >> 3;
    int firstBit = y - (firstByte << 3);
    int lastByte = ( y + height ) >> 3;
    int lastBit = y + height - (lastByte << 3);
    
    // created the dots in the masks
    uint8_t maskMask = (y & 0x01)?0b10101010:0b01010101;
    
    if (firstByte != lastByte)
    {
        uint8_t mask = 0xFF << firstBit;
        mask &= maskMask;
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] ^= (uint8_t) mask;
        
        for ( int hbyte = firstByte + 1; hbyte < lastByte; hbyte++ )
        {
            int baseIndex = hbyte * TINYGLCD_SCREEN_WIDTH;
            currentDrawBuffer[baseIndex + x] ^= maskMask;
        }
        
        mask = 0xFF >> (8 - lastBit);
        mask &= maskMask;
        currentDrawBuffer[lastByte * TINYGLCD_SCREEN_WIDTH + x] ^= (uint8_t) mask;
    }
    else
    {
        uint8_t mask = 0xFF << firstBit;
        mask = mask >> (8 - lastBit);
        mask &= maskMask;
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] ^= (uint8_t) mask;
    }
}

void tglcd_drawDottedHorizontalLineSOR(int x, int y, int width)
{
    int byte = y >> 3;
    int bit = y - (byte << 3);
    uint8_t mask = 0x01 << bit;
    
    for (int i = x; i < x + width; i+=2)
        currentDrawBuffer[byte * TINYGLCD_SCREEN_WIDTH + i] ^= (uint8_t) mask;
}

void tglcd_drawVerticalLineXOR(int x, int y, int height)
{
    if (x < 0)
        return;
    
    if (y < 0)
    {
        height += y;
        y = 0;
    }
    
    if (height + y > TINYGLCD_SCREEN_HEIGHT)
    {
        height = TINYGLCD_SCREEN_HEIGHT - 1 - y;
    }
    
    int firstByte = ( y ) >> 3;
    int firstBit = y - (firstByte << 3);
    int lastByte = ( y + height ) >> 3;
    int lastBit = y + height - (lastByte << 3);
    
    if (firstByte != lastByte)
    {
        uint8_t mask = 0xFF << firstBit;
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] ^= (uint8_t) mask;
        
        for ( int hbyte = firstByte + 1; hbyte < lastByte; hbyte++ )
        {
            int baseIndex = hbyte * TINYGLCD_SCREEN_WIDTH;
            currentDrawBuffer[baseIndex + x] ^= 0xFF;
        }
        
        mask = 0xFF >> (8 - lastBit);
        currentDrawBuffer[lastByte * TINYGLCD_SCREEN_WIDTH + x] ^= (uint8_t) mask;
    }
    else
    {
        uint8_t mask = 0xFF << firstBit;
        mask = mask >> (8 - lastBit);
        currentDrawBuffer[firstByte * TINYGLCD_SCREEN_WIDTH + x] ^= (uint8_t) mask;
    }
}

void tglcd_drawHorizontalLineXOR(int x, int y, int width)
{
    int byte = y >> 3;
    int bit = y - (byte << 3);
    uint8_t mask = 0x01 << bit;
    
    for (int i = x; i < x + width; i++)
        currentDrawBuffer[byte * TINYGLCD_SCREEN_WIDTH + i] ^= (uint8_t) mask;
}

void tglcd_drawBoxWithBorder(int xPos, int yPos, int width, int height)
{
    if (xPos < 0)
    {
        width += xPos;
		xPos = 0;
	}
	if (yPos < 0)
	{
		height += yPos;
		yPos = 0;
	}

	if (height + yPos >= TINYGLCD_SCREEN_HEIGHT)
	{
		height = TINYGLCD_SCREEN_HEIGHT - 1 - yPos;
	}
	if (width + xPos > TINYGLCD_SCREEN_WIDTH)
	{
		width = TINYGLCD_SCREEN_WIDTH - 1 - xPos;
	}

	uint64_t clearMask = 0;
	uint64_t sideMask = 0; // for the side borders
	uint64_t middleMask = 0; // for top and bottom border
	for (int i = 0; i < height; i++)
	{
		clearMask = clearMask << 1;
		clearMask |= ((uint64_t)1<<yPos);
	}
	middleMask = ((uint64_t)1<<yPos);
	middleMask |= ((uint64_t)1<<(yPos + height - 1));
	sideMask = clearMask;
	clearMask = ~clearMask;

	if (width >= 2)
	{
		// draw left side
		currentDrawBuffer[0*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) sideMask;
		currentDrawBuffer[1*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 8 );
		currentDrawBuffer[2*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 16);
		currentDrawBuffer[3*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 24);
		currentDrawBuffer[4*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 32);
		currentDrawBuffer[5*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 40);
		currentDrawBuffer[6*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 48);
		currentDrawBuffer[7*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 56);
		// draw right side
		currentDrawBuffer[0*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) sideMask;
		currentDrawBuffer[1*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 8 );
		currentDrawBuffer[2*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 16);
		currentDrawBuffer[3*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 24);
		currentDrawBuffer[4*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 32);
		currentDrawBuffer[5*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 40);
		currentDrawBuffer[6*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 48);
		currentDrawBuffer[7*TINYGLCD_SCREEN_WIDTH + xPos + width - 1] |= (uint8_t) (sideMask >> 56);

		for (int x = xPos + 1; x < xPos + width - 1; x++)
		{
			currentDrawBuffer[0*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) clearMask;
			currentDrawBuffer[1*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 8 );
			currentDrawBuffer[2*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 16);
			currentDrawBuffer[3*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 24);
			currentDrawBuffer[4*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 32);
			currentDrawBuffer[5*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 40);
			currentDrawBuffer[6*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 48);
			currentDrawBuffer[7*TINYGLCD_SCREEN_WIDTH + x] &= (uint8_t) (clearMask >> 56);

			currentDrawBuffer[0*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) middleMask;
			currentDrawBuffer[1*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 8 );
			currentDrawBuffer[2*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 16);
			currentDrawBuffer[3*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 24);
			currentDrawBuffer[4*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 32);
			currentDrawBuffer[5*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 40);
			currentDrawBuffer[6*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 48);
			currentDrawBuffer[7*TINYGLCD_SCREEN_WIDTH + x] |= (uint8_t) (middleMask >> 56);
		}
	}
	else if (width == 1)
	{
		// draw just one single black bar
		// draw left side
		currentDrawBuffer[0*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) sideMask;
		currentDrawBuffer[1*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 8 );
		currentDrawBuffer[2*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 16);
		currentDrawBuffer[3*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 24);
		currentDrawBuffer[4*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 32);
		currentDrawBuffer[5*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 40);
		currentDrawBuffer[6*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 48);
		currentDrawBuffer[7*TINYGLCD_SCREEN_WIDTH + xPos] |= (uint8_t) (sideMask >> 56);
	}
}

void tglcd_drawIntXOR(int x, int y, const font_t* font, int value, int alwaysIncludeSign, int clipToX, int clipToY)
{
	char buffer[6];
	if (alwaysIncludeSign)
		itoaEnforceSign(value, buffer);
	else
		itoa(value, buffer);

	tglcd_drawTextXOR(x, y, font, buffer, clipToX, clipToY);
}

void tglcd_drawIntOR(int x, int y, const font_t* font, int value, int alwaysIncludeSign, int clipToX, int clipToY)
{
	char buffer[6];
	if (alwaysIncludeSign)
		itoaEnforceSign(value, buffer);
	else
		itoa(value, buffer);

	tglcd_drawTextOR(x, y, font, buffer, clipToX, clipToY);
}

void tglcd_drawIntXCenteredXOR(int x, int y, int width, const font_t* font, int value, int alwaysIncludeSign)
{
	char buffer[6];
	if (alwaysIncludeSign)
		itoaEnforceSign(value, buffer);
	else
		itoa(value, buffer);

	tglcd_drawTextXCenteredXOR(x, y, width, font, buffer);
}

void tglcd_drawIntXCenteredOR(int x, int y, int width, const font_t* font, int value, int alwaysIncludeSign)
{
	char buffer[6];
	if (alwaysIncludeSign)
		itoaEnforceSign(value, buffer);
	else
		itoa(value, buffer);

	tglcd_drawTextXCenteredOR(x, y, width, font, buffer);
}


void tglcd_drawIntRightAlignedOR(int right, int y, const font_t* font, int value, int alwaysIncludeSign)
{
	char buffer[6];
	if (alwaysIncludeSign)
		itoaEnforceSign(value, buffer);
	else
		itoa(value, buffer);

	tglcd_drawTextRightAlignedOR(right, y, font, buffer);
}

void tglcd_drawIntRightAlignedXOR(int right, int y, const font_t* font, int value, int alwaysIncludeSign)
{
	char buffer[6];
	if (alwaysIncludeSign)
		itoaEnforceSign(value, buffer);
	else
		itoa(value, buffer);

	tglcd_drawTextRightAlignedXOR(right, y, font, buffer);
}

void tglcd_swapBuffers()
{
	uint8_t* tmp = currentDrawBuffer;
	currentDrawBuffer = currentSendBuffer;
	currentSendBuffer = tmp;
}


const uint8_t* tglcd_getCurrentDisplayBuffer()
{
    return currentSendBuffer;
}

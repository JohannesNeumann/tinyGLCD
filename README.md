# tinyGLCD
Tiny C library for drawing images and fonts on a graphical LCD for use in embedded environments

- for graphical LCDs with bi-state pixels (on/off, no greyscale)
- independent from display type and communication
- dual buffers for drawing on one display buffer while a DMA writes the second one to a display
- fonts and images included as `const uint8_t` variables which can sit in flash memory
- including C++ image conversion tool to read gif images and transform them into C-source code fonts and images

### usage example:
```
// init LCD
myLCD_init();
glcd_init();

while (1)
{
  glcd_clearToWhite();
  glcd_drawBoxWithBorder(10, 32, 100, 20);
  glcd_drawTextXCentered(15, 35, 90, &myFont, "Hello World!");
  glcd_swapBuffers();
  
  myLCD_writeBuffer(glcd_getDisplayBuffer());
}
```

/*
* The MIT License (MIT)
* Copyright (c) 2018-2020 Ada Brzoza
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __LCD_H__
#define __LCD_H__

#include "wm8994/wm8994.h"
#include "stm32746g_discovery_lcd.h"
#include "Utilities/Fonts/fonts.h"


#define LCD_X_SIZE			RK043FN48H_WIDTH
#define LCD_Y_SIZE			RK043FN48H_HEIGHT
#define	LCD_FONT_5					5
#define	LCD_FONT_6					6
#define	LCD_FONT_7					7
#define	LCD_FONT_9					9
#define	LCD_FONT_10					10
#define	LCD_FONT_15					15
#define	LCD_FONT_18					18
#define	LCD_FONT_40					40

#define LCD_TRANSPARENT_INVERSION	2
#define	LCD_TRANSPARENT				1
#define	LCD_OPAQUE					0

#define LCD_OBJECT_FILL				1
#define LCD_OBJECT_NO_FILL			0
#define LCD_OBJECT_INVERT			1
#define LCD_OBJECT_NO_INVERT		0

#define LCD_MAX_STR_LEN				50

void lcdInit(void);
void lcdClear(void);
void lcdUpdate(void);
void lcdInvertPixel(int32_t x, int32_t y);
void lcdPixel(int32_t x, int32_t y, uint32_t color);
void lcdRectangle(int32_t x1, int32_t x2, int32_t y1, int32_t y2, uint8_t fill);
void lcdHorizontalByte(uint8_t data, int32_t x, int32_t y);
void lcdVerticalByte(uint8_t data, int32_t x, int32_t y);
void lcdVerticalLine(int32_t x, int32_t y1, int32_t y2);
void lcdHorizontalLine(int32_t x1, int32_t x2, int32_t y);
void lcdInvertArea(int32_t x1, int32_t x2, int32_t y1, int32_t y2);
void lcdClearArea(int32_t x1, int32_t x2, int32_t y1, int32_t y2);
void lcdSetTransparent(uint8_t transparent);
uint8_t lcdGetTransparent(void);
void lcdSetColor(uint32_t color);
uint32_t lcdGetColor(void);
void lcdSetFont(uint32_t font);
uint32_t lcdGetFont(void);
uint32_t lcdGetFontSize(uint32_t font);
void lcdWidthTest(uint8_t value);
uint8_t lcdWidthTestStatus(void);

int32_t lcdChr(uint8_t chr, int32_t x, int32_t y);
int32_t lcdChrWidth(uint8_t chr);

int lcdGetTextCursor(int* x, int* y);
int lcdSetTextCursor(int x, int y);
int lcdSetTextLeftAlignX(int x);	//on newline will put chars in next line starting at this x

void lcd(const char* str, ...);
int32_t lcdCentered(const char* text, int32_t x, int32_t y);

void lcdSetBackgroundColor(uint32_t color);
uint32_t lcdGetBackgroundColor(void);


#endif


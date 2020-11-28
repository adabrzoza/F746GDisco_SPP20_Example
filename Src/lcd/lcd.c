/*
* The MIT License (MIT)
* Copyright (c) 2019-2020 Ada Brzoza
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

#include  <errno.h>
#include  <sys/unistd.h>
#include  <string.h>
#include <stdarg.h>


#include "stm32746g_discovery_lcd.h"
#include "Utilities/Fonts/fonts.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_audio.h"
#include "term_io.h"
#include "dbgu.h"
#include "ansi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd.h"
#include "lcd_chr.h"



uint32_t lcd_image_fg[LCD_Y_SIZE][LCD_X_SIZE] __attribute__((section(".sdram")));
uint32_t lcd_image_bg[LCD_Y_SIZE][LCD_X_SIZE] __attribute__((section(".sdram")));
uint32_t lcd_shadow[LCD_Y_SIZE][LCD_X_SIZE] __attribute__((section(".sdram")));

uint32_t lcdFont;
uint32_t lcdColor;
uint32_t lcdBackgroundColor;
uint8_t lcdTransparent;
static uint8_t widthTest;

char tempStr[LCD_MAX_STR_LEN];
int textCursorX = 0;
int textCursorY = 0;
int textLeftAlignX = 0;

static int lcdHal_Init(void);




static int lcdHal_Init(void)
{
	/* LCD Initialization */ 
	BSP_LCD_Init();

	/* LCD Initialization */ 
	BSP_LCD_LayerDefaultInit(0, (unsigned int)lcd_image_bg);
	BSP_LCD_LayerDefaultInit(1, (unsigned int)lcd_image_fg);
	//BSP_LCD_LayerDefaultInit(1, (unsigned int)lcd_shadow);

	/* Enable the LCD */ 
	BSP_LCD_DisplayOn(); 

	/* Select the LCD Background Layer  */
	BSP_LCD_SelectLayer(0);

	/* Clear the Background Layer */ 
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	BSP_LCD_SetColorKeying(1,LCD_COLOR_WHITE);

	/* Select the LCD Foreground Layer  */
	BSP_LCD_SelectLayer(1);

	/* Clear the Foreground Layer */ 
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);

	/* Configure the transparency for foreground and background :
	 Increase the transparency */
	BSP_LCD_SetTransparency(0, 255);
	BSP_LCD_SetTransparency(1, 255);
	
	//sample_bg();
	
	return 0;
}

uint32_t* lcdShadow = (uint32_t*)lcd_shadow;

void lcdInit(void)
{
	lcdHal_Init();
	lcdClear();
	
	lcdSetBackgroundColor(LCD_COLOR_WHITE);
	lcdSetFont(LCD_FONT_10);
	lcdSetColor(LCD_COLOR_DARKBLUE);
	lcdSetTransparent(LCD_OPAQUE);
	
	xprintf("images in SDRAM: shadow @ %08X, fg @ %08X, bg @ %08X\n",(unsigned int)lcd_shadow,(unsigned int)lcd_image_fg,(unsigned int)lcd_image_bg);
	xprintf("one image size: %d = 0x%08X\n",(4*LCD_X_SIZE*LCD_Y_SIZE),(4*LCD_X_SIZE*LCD_Y_SIZE));
	xprintf("resolution: X=%d, Y=%d\n",LCD_X_SIZE,LCD_Y_SIZE);
	
	lcdUpdate();
}

void lcdClear(void)
{
	//memset(lcdShadow,lcdBackgroundColor,LCD_X_SIZE*LCD_Y_SIZE*sizeof(uint32_t));
	int i=LCD_X_SIZE*LCD_Y_SIZE;
	uint32_t* ptr = lcdShadow;
	while(i--)
	{
		*ptr = lcdBackgroundColor;
		ptr++;
	}
}

void lcdUpdate(void)
{
	int i=LCD_X_SIZE*LCD_Y_SIZE;
	uint32_t* src = (uint32_t*)lcdShadow;
	uint32_t* dst = (uint32_t*)lcd_image_fg;
	while(i--)
	{
		*dst++ = *src++;
		
		/*switch(opt)
		{
			case 0:
				*dst++ = LCD_COLOR_RED;
				break;
			case 1:
				*dst++ = LCD_COLOR_GREEN;
				break;
			case 2:
				*dst++ = LCD_COLOR_BLUE;
				break;
			case 3:
				*dst++ = LCD_COLOR_DARKMAGENTA;
				break;
		}*/
		
		
		/*uint32_t d = 500;
		while(d--) asm volatile("nop");*/
		
		//vTaskDelay(1);
	}
}

void lcdInvertPixel(int32_t x, int32_t y)
{
	if(x>(LCD_X_SIZE-1)) return;
	if(y>(LCD_Y_SIZE-1)) return;
	if(x<0) return;
	if(y<0) return;
	uint32_t before = lcdShadow[LCD_X_SIZE*y + x];
	uint32_t after = 0xFF000000 | (~before);
	lcdShadow[LCD_X_SIZE*y + x] = after;
	//printf("*inv: %08X to %08X\n",(unsigned int)before,(unsigned int)after);
}


void lcdPixel(int32_t x, int32_t y, uint32_t color)
{
	if(x>(LCD_X_SIZE-1)) return;
	if(y>(LCD_Y_SIZE-1)) return;
	if(x<0) return;
	if(y<0) return;
	lcdShadow[LCD_X_SIZE*y + x] = color;
}

void lcdRectangle(int32_t x1, int32_t x2, int32_t y1, int32_t y2, uint8_t fill)
{
	lcdHorizontalLine(x1,x2,y1);
	lcdHorizontalLine(x1,x2,y2);
	lcdVerticalLine(x1,y1,y2);
	lcdVerticalLine(x2,y1,y2);
	
	if(fill)
	{
		int32_t x;
		for(x=x1+1;x<x2;x++)
		{
			lcdVerticalLine(x,y1,y2);
		}
	}
}

//draws horizontal byte contents (1 = color, 0 = background/unchanged, depending on transparency settings)
//coordinates x,y are coords of the first left pixel in given byte, so (0xFF,0,0) will place horizontal 8-pix line from pixels x=0 to x=7 and y=0
void lcdHorizontalByte(uint8_t data, int32_t x, int32_t y)
{
	if(x>LCD_X_SIZE) return;
	if(y>LCD_Y_SIZE) return;
	if(x<-8) return;
	if(y<0) return;
	int32_t curX=x;
	uint8_t i;
	switch(lcdTransparent)
	{
		case LCD_OPAQUE:
			for(i=0;i<8;i++)
			{
				if(data & 0x80) lcdPixel(curX,y,lcdColor); else lcdPixel(curX,y,lcdBackgroundColor);
				data <<= 1;
				curX++;
			}
			return;
		case LCD_TRANSPARENT:
			for(i=0;i<8;i++)
			{
				if(data & 0x80) lcdPixel(curX,y,lcdColor);
				data <<= 1;
				curX++;
			}
			return;
		case LCD_TRANSPARENT_INVERSION:
			for(i=0;i<8;i++)
			{
				if(data & 0x80) lcdInvertPixel(curX,y);
				data <<= 1;
				curX++;
			}
			return;
	}//switch(lcdTransparent)
}

//draws a vertical byte starting at x,y and ending at x,y+7
void lcdVerticalByte(uint8_t data, int32_t x, int32_t y)
{
	if(x>LCD_X_SIZE) return;
	if(y>LCD_Y_SIZE) return;
	if(x<0) return;
	if(y<-8) return;
	int32_t curY=y;
	uint8_t i;
	switch(lcdTransparent)
	{
		case LCD_OPAQUE:
			for(i=0;i<8;i++)
			{
				if(data & 0x01) lcdPixel(x,curY,lcdColor); else lcdPixel(x,curY,lcdBackgroundColor);
				data >>= 1;
				curY++;
			}
			return;
		case LCD_TRANSPARENT:
			for(i=0;i<8;i++)
			{
				if(data & 0x01) lcdPixel(x,curY,lcdColor);
				data >>= 1;
				curY++;
			}
			return;
		case LCD_TRANSPARENT_INVERSION:
			for(i=0;i<8;i++)
			{
				if(data & 0x01) lcdInvertPixel(x,curY);
				data >>= 1;
				curY++;
			}
			return;
	}//switch(lcdTransparent)
}

//draws a vertical line
void lcdVerticalLine(int32_t x, int32_t y1, int32_t y2)
{
	int32_t currentY;
	if(lcdTransparent == LCD_TRANSPARENT_INVERSION)
		for(currentY = y1; currentY<=y2; currentY++) lcdInvertPixel(x,currentY);
	else
		for(currentY = y1; currentY<=y2; currentY++) lcdPixel(x,currentY,lcdColor);
}

//draws a horizontal line
void lcdHorizontalLine(int32_t x1, int32_t x2, int32_t y)
{
	int32_t currentX;
	if(lcdTransparent == LCD_TRANSPARENT_INVERSION)
	{
		for(currentX=x1;currentX<=x2;currentX++) lcdInvertPixel(currentX,y);
	}
	else
	{
		for(currentX=x1;currentX<=x2;currentX++) lcdPixel(currentX,y,lcdColor);
	}
}

//inverts colors in selected area
void lcdInvertArea(int32_t x1, int32_t x2, int32_t y1, int32_t y2)
{
	int32_t currentX, currentY;
	
	for(currentY=y1;currentY<=y2;currentY++)
		for(currentX=x1;currentX<=x2;currentX++)
			lcdInvertPixel(currentX,currentY);
}

//clears area to background color
void lcdClearArea(int32_t x1, int32_t x2, int32_t y1, int32_t y2)
{
	int32_t currentX, currentY;
	
	for(currentY=y1;currentY<=y2;currentY++)
		for(currentX=x1;currentX<=x2;currentX++)
			lcdPixel(currentX,currentY,lcdBackgroundColor);
	
}


void lcdSetTransparent(uint8_t transparent)
{
	lcdTransparent = transparent;
}

uint8_t lcdGetTransparent(void)
{
	return(lcdTransparent);
}


void lcdSetColor(uint32_t color)
{
	lcdColor = color;
}


uint32_t lcdGetColor(void)
{
	return(lcdColor);
}


void lcdSetFont(uint32_t font)
{
	lcdFont = font;
}


uint32_t lcdGetFont(void)
{
	return(lcdFont);
}

uint32_t lcdGetFontSize(uint32_t font)
{
	switch(font)
	{
		case LCD_FONT_9: return(10);
		case LCD_FONT_5:
		case LCD_FONT_6:
		case LCD_FONT_7:
		case LCD_FONT_10:
		case LCD_FONT_15:
		case LCD_FONT_18:
		case LCD_FONT_40:
			return(font);
		default:
			return(0);
	}
}


void lcdWidthTest(uint8_t value)
{
	if(value) widthTest=1; else widthTest=0;
}

uint8_t lcdWidthTestStatus(void)
{
	return widthTest;
}


int lcdSetTextCursor(int x, int y)
{
	textCursorX = x;
	textCursorY = y;
	return 0;
}

int lcdGetTextCursor(int* x, int* y)
{
	*x = textCursorX;
	*y = textCursorY;
	return 0;
}

int lcdSetTextLeftAlignX(int x)
{
	textLeftAlignX = x;
	return 0;
}

static void lcd_chr_raw(char c)
{
	//display char here
	//xprintf("disp chr @ x=%d, y=%d, c=%c\n",textCursorX,textCursorY,c);
	if(c=='\n')
	{
		textCursorY=textCursorY + (float)lcdGetFont()*1.2;
		textCursorX = textLeftAlignX;
	}
	else
	{
		textCursorX = lcdChr(c,textCursorX,textCursorY);
	}
	
}

static void lcd_string_raw(const char* str)
{
	//display string on lcd here
	while (*str)
		lcd_chr_raw(*str++);
}


static void lcdxitoa (long val, int radix, int len)
{
	uint8_t c, r, sgn = 0, pad = ' ';
	uint8_t s[20], i = 0;
	uint32_t v;


	if (radix < 0) {
		radix = -radix;
		if (val < 0) {
			val = -val;
			sgn = '-';
		}
	}
	v = val;
	r = radix;
	if (len < 0) {
		len = -len;
		pad = '0';
	}
	if (len > 20) return;
	do {
		c = (uint8_t)(v % r);
		if (c >= 10) c += 7;
		c += '0';
		s[i++] = c;
		v /= r;
	} while (v);
	if (sgn) s[i++] = sgn;
	while (i < len)
		s[i++] = pad;
	do
		lcd_chr_raw(s[--i]);
	while (i);
}

void lcd(const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l;


	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			lcd_chr_raw(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}
		if (s) w = -w;
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			lcd_string_raw(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			lcd_chr_raw((char)va_arg(arp, int));
			continue;
		}
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if (!r) break;
		if (l) {
			lcdxitoa((long)va_arg(arp, long), r, w);
		} else {
			if (r > 0)
				lcdxitoa((unsigned long)va_arg(arp, int), r, w);
			else
				lcdxitoa((long)va_arg(arp, int), r, w);
		}
	}

	va_end(arp);
	
}

void lcdSetBackgroundColor(uint32_t color)
{
	lcdBackgroundColor = color;
}


uint32_t lcdGetBackgroundColor(void)
{
	return(lcdBackgroundColor);
}

int32_t lcdCentered(const char* text, int32_t x, int32_t y)
{
	lcdWidthTest(1);
	textCursorX = x;
	textCursorY = y;
	
	lcd(text);
	
	int32_t len = textCursorX - x;
	
	lcdWidthTest(0);
	
	textCursorX = x-(len/2);
	textCursorY = y;
	
	lcd(text);
	
	x = textCursorX;
	
	return(x);
}


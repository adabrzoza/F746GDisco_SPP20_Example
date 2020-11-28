#ifndef __TOUCHSCREEN_H__
#define __TOUCHSCREEN_H__

typedef struct {
	uint16_t x;
	uint16_t y;
} T_TouchTiles;

typedef struct {
	uint16_t XTilesNb;
	uint16_t YTilesNb;
	uint16_t XTileSize;
	uint16_t YTileSize;
} T_TouchTilesCfg;

uint8_t panelRead(uint16_t *x, uint16_t *y);
void panelSetGrid(uint16_t xdiv, uint16_t ydiv);
int panelGridTap(T_Panel* pPanel);
int panelInit(void);

#endif



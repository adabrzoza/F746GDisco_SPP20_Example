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



uint16_t panelGridX, panelGridY;
uint16_t panelGridSizeX, panelGridSizeY;


static TS_StateTypeDef  TS_State;


static int computePos(TS_StateTypeDef *pState)
{
	//do actually nothing ;)
	//this function is left for compatibility with older implementations
	pState = pState;
	return 0;
}



uint8_t panelRead(uint16_t *x, uint16_t *y)
{
	BSP_TS_GetState(&TS_State);

	if (TS_State.touchDetected)
	{
		if(computePos(&TS_State)==0)
		{
			printf("pnl read: x=%d , y=%d\n",TS_State.touchX[0],TS_State.touchY[0]);
			*x = TS_State.touchX[0];
			*y = TS_State.touchY[0];
			return(1);
		}
	}
	else
	{
		return(0);
	}
	return 0;
}


void panelSetGrid(uint16_t xdiv, uint16_t ydiv)
{
	panelGridX = xdiv;
	panelGridY = ydiv;
	panelGridSizeX = LCD_X_SIZE / panelGridX;
	panelGridSizeY = LCD_Y_SIZE / panelGridY;
}


int panelInit(void)
{
	uint8_t  status = 0;
	status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	if(status != TS_OK)
	{
		xprintf("panelInit: status=%d (error)\n",status);
		return -1;
	}
	xprintf("panelInit returns OK\n");
	return 0;
}

int panelGridTap(T_Panel* pPanel)
{
	if(panelRead(&pPanel->x,&pPanel->y)==0) return 0;
	pPanel->x = pPanel->x / panelGridSizeX;
	pPanel->y = pPanel->y / panelGridSizeY;
	return 1;
}


#ifndef __SPP_CFG_H__
#define __SPP_CFG_H__

#include "spp.h"
#include "spp_cmdid.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_uart.h"
#include "term_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dbgu.h"

//mandatory config

//#define SPP_DELAY_MS(a)     HAL_Delay(a)
#define SPP_DELAY_MS(a)     vTaskDelay(a)

#define SPP_RX_PIN_IS_HIGH (HAL_GPIO_ReadPin(RS485_RX_GPIO_Port,RS485_RX_Pin)==GPIO_PIN_SET)

//#define SPP_RE_GPIO   RS485_RE_GPIO_Port
//#define SPP_RE_PIN    RS485_RE_Pin

//maximum iterations of exponential back-off
//resulting maximum back-off is 2^(SPP_BACK_OFF_MAX-1) in milliseconds
#define SPP_BACK_OFF_MAX  12

//one byte in 19200 8N1 takes 10 b / 19200 b/s = 0.000520833 s
#define SPP_DELAY_AFTER_COLLISION  52

#define SPP_PAYLOAD_LEN   40

//0x10, 0x20 or 0x30
#define SPP_ADDRESS       0x20

//ignore wrong CRC sums
#define SPP_IGNORE_CRC            0

//ignore destination address
#define SPP_SNIFF_MODE            0

#define SPP_LED_BUSY_ON   HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
#define SPP_LED_BUSY_OFF  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);

#define SPP_LED_TX_ON     SPP_LED_BUSY_ON
#define SPP_LED_TX_OFF    SPP_LED_BUSY_OFF


#define SPP_MSG           xprintf


//port-specific config

#define SPP_DE_GPIO         RS485_DIR_GPIO_Port
//#define SPP_DE_GPIO         RS485_DE_GPIO_Port
#define SPP_DE_PIN          RS485_DIR_Pin



#endif



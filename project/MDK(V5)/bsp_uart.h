#ifndef __BSP_UART_
#define __BSP_UART_
#include "stdint.h"


void usart_send_data(uint8_t ucch);
void usart_send_String(uint8_t *ucstr);

uint8_t Serial_GetRxFlag(void);

uint8_t Serial_GetRxData(void);

#endif


#ifndef __HWT101_H
#define __HWT101_H

#include "stm32f4xx.h"

/* ?????????? ---------------------------------------*/
void UART4_Init(uint32_t baudrate);
void Serial_SendByte_UART4(uint8_t Byte);
void UART4_Put_Char(uint8_t DataToSend);
void UART4_Put_String(uint8_t *Str);
void UART4_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);
void MyUART4_SendArr(uint16_t *str, uint8_t count);
void sendcmd(char cmd[]);

/* IMU ????? ---------------------------------------------*/
struct SAcc   { short a[3]; short T; };
struct SGyro  { short w[3]; short T; };
extern char angle_buf[30];
struct SAngle { short Angle[3]; short T; };

#endif

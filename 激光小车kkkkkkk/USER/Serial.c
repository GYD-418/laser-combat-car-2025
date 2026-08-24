#include "Serial.h"
#include <stdio.h>
#include "stm32f4xx.h"                  // Device header

#include <stdio.h>   
#if EN_USART1_RX

uint8_t Serial_RxPacket_1[7] = {0};
uint8_t Serial_RxFlag_1 = 0;

#pragma import(__use_no_semihosting)
struct __FILE 
{ 
    int handle; 
};
FILE __stdout;

void _sys_exit(int x) 
{ 
    x = x; 
}

int fputc(int ch, FILE *f)
{
    while((USART1->SR & USART_SR_TXE) == 0); // ????????
    USART1->DR = (uint8_t) ch;
    return ch;
}
int aa=0;
void uart_init(u32 baudrate)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // ????
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // GPIO ?? PA9(TX), PA10(RX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    // USART ???
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // ????
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

// ??????
void USART1_IRQHandler(void) {
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		uint8_t RxData = (uint8_t)USART_ReceiveData(USART1);  // ??

		if (RxState == 0) {
			if (RxData == 0xAA) {
				pRxPacket = 0;
				RxState = 1;
			}
		}
		else if (RxState == 1) {
			Serial_RxPacket_1[pRxPacket] = RxData;
			pRxPacket++;
			if (pRxPacket > 6) {
				RxState = 2;
			}
		}
		else if (RxState == 2) {
			if (RxData == 0xFE) {
				RxState = 0;
				Serial_RxFlag_1 = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	
#if SYSTEM_SUPPORT_OS
	OSIntExit();  
#endif
}

#endif

#include <string.h>
#include "HWT101.h"
#include "stm32f4xx.h"

/*------------------------- ???? -------------------------*/
struct SAcc   stcAcc;
struct SGyro  stcGyro;
struct SAngle stcAngle;

static uint8_t TxBuffer[256];
static uint8_t TxCounter = 0;
static uint8_t count     = 0;

extern void CopeSerial2Data(uint8_t ucData);
char angle_buf[30];

/*===================================================================
 * 1. UART4 ??? (PC10 = TX, PC11 = RX)
 *==================================================================*/
void UART4_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* ???? */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* GPIO ?? */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);   /* TX */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);   /* RX */

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* UART ?? */
    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);

    /* ??:?? RXNE,TXE ?????? */
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_ClearFlag(UART4, USART_FLAG_TC);

    /* NVIC */
    NVIC_InitStructure.NVIC_IRQChannel                   = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* ???? */
    USART_Cmd(UART4, ENABLE);
}

/*===================================================================
 * 2. ???? 1 ??
 *==================================================================*/
void Serial_SendByte_UART4(uint8_t Byte)
{
    USART_SendData(UART4, Byte);
    while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
}

/*===================================================================
 * 3. UART4 ??????
 *==================================================================*/
void UART4_IRQHandler(void)
{
    /* ?????? -> ???? */
    if (USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
    {
        USART_SendData(UART4, TxBuffer[TxCounter++]);
        USART_ClearITPendingBit(UART4, USART_IT_TXE);

        if (TxCounter == count) USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
    }
    /* ???? */
    else if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        CopeSerial2Data((uint8_t)UART4->DR);
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }

    /* ????? */
    USART_ClearITPendingBit(UART4, USART_IT_ORE);
}

/*===================================================================
 * 4. ??????(? TxBuffer)
 *==================================================================*/
void UART4_Put_Char(uint8_t data)
{
    TxBuffer[count++] = data;
    USART_ITConfig(UART4, USART_IT_TXE, ENABLE);   /* ?????? */
}

void UART4_Put_String(uint8_t *str)
{
    while (*str)
    {
        if (*str == '\r')      UART4_Put_Char(0x0D);
        else if (*str == '\n') UART4_Put_Char(0x0A);
        else                   UART4_Put_Char(*str);
        str++;
    }
}

/*===================================================================
 * 5. ? JY ???? 5 ????
 *==================================================================*/
void sendcmd(char cmd[])
{
    uint8_t i;                           /* C89:???????? */
    for (i = 0; i < 5; i++)
        UART4_Put_Char(cmd[i]);
}

/*===================================================================
 * 6. ???????
 *==================================================================*/
void CopeSerial2Data(uint8_t ucData)
{
    static uint8_t ucRxBuffer[250];
    static uint8_t ucRxCnt = 0;

    ucRxBuffer[ucRxCnt++] = ucData;

    if (ucRxBuffer[0] != 0x55) { ucRxCnt = 0; return; }
    if (ucRxCnt < 11)         { return; }

    switch (ucRxBuffer[1])
    {
        case 0x51: memcpy(&stcAcc,   &ucRxBuffer[2], 8); break;
        case 0x52: memcpy(&stcGyro,  &ucRxBuffer[2], 8); break;
        case 0x53: memcpy(&stcAngle, &ucRxBuffer[2], 8); break;
    }
    ucRxCnt = 0;
}

/*===================================================================
 * 7. ?? 16bit ????
 *==================================================================*/
void UART4_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch)
{
    uint8_t High = ch;                   /* ??? 8 ?,???????? */
    USART_SendData(pUSARTx, High);
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

void MyUART4_SendArr(uint16_t *str, uint8_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        UART4_SendHalfWord(UART4, *(str + i));
    }
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
}

#include "delay.h"

static uint8_t  fac_us = 0;  // us ??????
static uint16_t fac_ms = 0;  // ms ??????

// ??? SysTick ???
void SysTick_Init(void)
{
    // ?? HCLK / 8 ?????
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    
    fac_us = SystemCoreClock / 8000000;      // us ????
    fac_ms = (uint16_t)fac_us * 1000;        // ms ????
}

// ????
void delay_us(uint32_t nus)
{
    uint32_t temp;

    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // ????? 0

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

// ????
void delay_ms(uint32_t nms)
{
    uint32_t temp;

    while (nms--) {
        SysTick->LOAD = fac_ms;
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        do {
            temp = SysTick->CTRL;
        } while ((temp & 0x01) && !(temp & (1 << 16)));

        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0x00;
    }
}

#include "step.h"
#include "stm32f4xx.h"
#include "PID.h"
#include "delay.h"
#include <string.h>


#define STEP_PIN   GPIO_Pin_1  
#define DIR_PIN    GPIO_Pin_3  
uint8_t current_dir = 0;



void Timer6_Init(int AR , int PS)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;    //结构体
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  //开启时钟
	
//	TIM_InternalClockConfig(TIM6);     //选择内部时钟

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //指定时钟分频 采样频率 一分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式 向上计数
	TIM_TimeBaseInitStructure.TIM_Period = AR - 1;   //周期 ARR自动重装器的值  0——65535         定时频率 = 72M / (PSC+1) / (ARR+1)  10k频率计10000个数
	TIM_TimeBaseInitStructure.TIM_Prescaler = PS - 1;  //PSC预分频器的值         0——65535      预分频多 自动重装少 频率低计较少的数
//	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;  //重复计数器的值
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);    //初始化时基单元
	
//	TIM_ClearFlag(TIM6, TIM_FLAG_Update);     //手动清除更新中断标志位  防止立刻更新中断进入中断从1开始
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);//使能中断
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;   //中断通道  定时器2   NVIC通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    //响应优先级
	NVIC_Init(&NVIC_InitStructure);         //初始化结构体
	
	TIM_Cmd(TIM6, ENABLE);
}

void Timer7_Init(int AR , int PS)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;    //结构体	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);  //开启时钟
	
//	TIM_InternalClockConfig(TIM7);     //选择内部时钟

	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //指定时钟分频 采样频率 一分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式 向上计数
	TIM_TimeBaseInitStructure.TIM_Period = AR - 1;   //周期 ARR自动重装器的值  0——65535         定时频率 = 72M / (PSC+1) / (ARR+1)  10k频率计10000个数
	TIM_TimeBaseInitStructure.TIM_Prescaler = PS - 1;  //PSC预分频器的值         0——65535      预分频多 自动重装少 频率低计较少的数
//	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;  //重复计数器的值
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStructure);    //初始化时基单元
	
//	TIM_ClearFlag(TIM7, TIM_FLAG_Update);     //手动清除更新中断标志位  防止立刻更新中断进入中断从1开始
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);//使能中断
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;   //中断通道  定时器2   NVIC通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;    //响应优先级
	NVIC_Init(&NVIC_InitStructure);         //初始化结构体
	
	TIM_Cmd(TIM7, ENABLE);
}


void Stepper_Init(uint16_t speed_Hz)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;
    NVIC_InitTypeDef nvic;

   
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

   
    gpio.GPIO_Pin = STEP_PIN;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);

  
    gpio.GPIO_Pin = DIR_PIN;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);


    tim.TIM_Prescaler = 84 - 1;  
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;

    if (speed_Hz == 0) speed_Hz = 1;
    tim.TIM_Period = (1000000 / speed_Hz) - 1;  
    TIM_TimeBaseInit(TIM5, &tim);

    
    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_Pulse = (tim.TIM_Period + 1) / 2;  
    TIM_OC2Init(TIM5, &oc);
    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);

    
    TIM_ARRPreloadConfig(TIM5, ENABLE);

    
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

   
    nvic.NVIC_IRQChannel = TIM5_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

   
    TIM_Cmd(TIM5, ENABLE);
    TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
}

void Stepper_SetDir(uint8_t dir)
{
    if (dir)
        GPIO_ResetBits(GPIOA, DIR_PIN);
    else
        GPIO_SetBits(GPIOA, DIR_PIN);
		current_dir = dir; 
}



void Stepper_SetSpeed(uint32_t speed_Hz)
{
    uint32_t period;

    if (speed_Hz == 0)
    {
        TIM_Cmd(TIM5, DISABLE);
        return;
    }

    period = 1000000UL / speed_Hz;
    TIM5->ARR = period - 1;
    TIM5->CCR2 = period / 2;
    TIM_Cmd(TIM5, ENABLE);
}



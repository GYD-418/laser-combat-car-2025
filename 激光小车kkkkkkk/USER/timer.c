#include "timer.h"
#include "stm32f4xx.h"                  // Device header

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
void Timer6_Init(int AR, int PS) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    // ??????
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = AR - 1;       // ARR = AR - 1
    TIM_TimeBaseInitStructure.TIM_Prescaler = PS - 1;    // PSC = PS - 1
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM6, TIM_FLAG_Update); // ??????
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    // ???????(??main???????)
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // ??????
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM6, ENABLE);
}

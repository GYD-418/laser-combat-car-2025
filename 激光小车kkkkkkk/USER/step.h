#ifndef __STEP_H
#define __STEP_H

#include "stm32f4xx.h"  // Device header

extern volatile float speed_cmd;
extern volatile float speed_acc;
extern volatile int32_t step_target;
extern volatile int32_t step_current;

void Stepper_InitHW(void);                     // ??? TIM3 ??
void Stepper_Init(uint16_t speed_Hz);          // ? ?? PWM ???????

void Stepper_SetTarget(int32_t target);        // ??????
void Stepper_StepOnce(void);                   // ??????(??)
extern uint8_t current_dir;

void Stepper_SetDir(uint8_t dir);              // ????
void Stepper_SetSpeed(uint32_t speed_Hz);      // ?? PWM ??(??)
void Timer6_Init(int AR , int PS);
void Timer7_Init(int AR , int PS);
#endif

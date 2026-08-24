#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
void Stepper_Move(int steps, uint8_t dir);
void Servo_SetPulse(uint16_t pulse_us);
void Motor_STEP_Init(void);
void Motor_DIR_Init(void);
void TIM3_PWM1_Init(void);
void TIM3_PWM2_Init(void);
void TIM3_PWM3_Init(void);
void TIM3_PWM4_Init(void);
void TIM4_PWM1_Init(void);

#endif

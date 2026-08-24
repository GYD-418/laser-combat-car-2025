#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "PWM.h"
#include "PID.h"
#include "step.h"
#include "oled.h"
#include "motor.h"

#define CAMERA_WIDTH  350
#define CAMERA_HEIGHT 270
#define STEP_PIN   GPIO_Pin_1  // PA1 - TIM5_CH2
#define DIR_PIN    GPIO_Pin_3  // PA3 - ??IO
float ballX_y = 0;
float ballY_y = 0;
#define yy 0.1f  

uint8_t hexToDec(uint8_t hexValue) {
    uint8_t decValue = 0;
    uint8_t base = 1;
    while (hexValue != 0) {
        uint8_t digit = hexValue % 10;
        decValue += digit * base;
        base *= 16;
        hexValue /= 10;
    }
		
    return decValue;
}
uint8_t RxData;			
float ballX ; 
float ballY ; 
float pulse_to_angle(uint16_t pulse_us) {
    return ((float)(pulse_us - 500) * 180.0f) / 2000.0f;
}//PWM转化成角度

uint16_t angle_to_pulse(float angle) {
    return (uint16_t)(angle * 2000.0f / 180.0f + 500.0f);
}//角度转化成PWM

float deltaY;
float servo_output = 0;
float servo_target = 0;
float current_angle,target_angle,output_angle,new_angle;
float servo_position = 0;
uint16_t servo_pulse = 1200;//舵机脉冲
volatile int32_t step_current = 0;  //现在值
volatile int32_t step_target = 0;   //目标值
int game_over,time,kkkk;
uint32_t no_data_counter = 0;
PID_t stepPID;
PID_t servoPID;
volatile float speed_cmd = 2.95f;//PID输出速度
extern uint8_t current_dir;
u8 t=0;
int test=1,xzq;
int32_t	maichongkk,deltaX;
int main(void)
{
	  uart_init(115200);
		SysTick_Init();
    PWM_Init();
		Stepper_Init(100);         
    Stepper_SetDir(0);         
		Motor_STEP_Init();
		Motor_DIR_Init();
		TIM3_PWM1_Init();
		TIM3_PWM2_Init();
		TIM3_PWM3_Init();
		TIM3_PWM4_Init();
		TIM4_PWM1_Init();
    Servo_SetPulse(1200);	
		delay_ms(1000);
    PID_Init(&stepPID, 1.3f, 0.000001, 0.7f, -800.0f, 800.0f);  //PID_Init(PID_t *pid, float kp, float ki, float kd, float outMin, float outMax)00001159f
    PID_Init(&servoPID, 0.18f, 0.0f, 0.0f, -800.0f, 800.0f);

    Timer6_Init(84000,1);
		Timer7_Init(168,50000);
	//		MOTOR_Move_1(-1000,100,100,300);
//		MOTOR_Move_2(1000,100,100,300);
//		MOTOR_Move_3(-1000,100,100,300);
//		MOTOR_Move_5(1000,100,100,300);
//		GPIO_SetBits(GPIOD,GPIO_Pin_1);//激光器关闭
			GPIO_ResetBits(GPIOD,GPIO_Pin_1);//激光器打开
//		GPIO_SetBits(GPIOD,GPIO_Pin_14);//蜂鸣器
//		GPIO_SetBits(GPIOD,GPIO_Pin_15);//绿灯

    while (1)
    {
			switch(test)
			{
				case 1://直走任务
				if(Speed_Test_State.T_state == Acting)
				{
					static int count = 0;
					count ++;
					if(count <2)
					{
						MOTOR_Move_1(-20000,180,150,300);
						MOTOR_Move_2(20000,180,150,300);
						MOTOR_Move_3(-20000,180,150,300);
						MOTOR_Move_5(20000,180,150,300);
					}
					//之走完后更新状态
					delay_ms(2);
					if(motor_sta_1 == STOP)
					{
						Speed_Test_State.T_state = End;
						delay_ms(100);
						TIM_Cmd(TIM3,DISABLE);
						TIM_SetCounter(TIM3,0);	
						TIM_Cmd(TIM4,DISABLE);
						TIM_SetCounter(TIM4,0);	
						GPIO_ResetBits(GPIOD,GPIO_Pin_1);			//打开激光器
					}
				}
				else if(Speed_Test_State.T_state == End)
				{
					Speed_Test_State.T_state = Acting;
					test = 2;
				}
				break;
				case 2://转弯任务
				if(Speed_Test_State.T_state == Acting)
				{
					static int count = 0;
					count ++;
					if(count <2)
					{
						MOTOR_Move_1(2000,300,100,300);
						MOTOR_Move_2(2000,300,100,300);
						MOTOR_Move_3(-2000,300,100,300);
						MOTOR_Move_5(-2000,300,100,300);
					}
					//之走完后更新状态
					delay_ms(2);
					if(motor_sta_1 == STOP)
					{
						Speed_Test_State.T_state = End;
						delay_ms(100);
						TIM_Cmd(TIM3,DISABLE);
						TIM_SetCounter(TIM3,0);	
						TIM_Cmd(TIM4,DISABLE);
						TIM_SetCounter(TIM4,0);	
					}		
				}
				else if(Speed_Test_State.T_state == End)
				{
					Speed_Test_State.T_state = Acting;
					test = 3;
				}
				break;
				case 4://直走任务
				if(Speed_Test_State.T_state == Acting)
				{
					xzq=1;
					if(game_over==1) 			//胜利指示
					{
						GPIO_SetBits(GPIOD,GPIO_Pin_15);
						GPIO_SetBits(GPIOD,GPIO_Pin_14);
						delay_ms(1000);
						GPIO_ResetBits(GPIOD,GPIO_Pin_14);
						GPIO_ResetBits(GPIOD,GPIO_Pin_15);
						delay_ms(1000);
						game_over=0;
					}
					if (Serial_RxFlag_1 == 1)  //接收到串口坐标
					{
						Serial_RxFlag_1 = 0;
						no_data_counter = 0; 
						ballX=(hexToDec(Serial_RxPacket_1[1])*100)+(hexToDec(Serial_RxPacket_1[2])*10)+(hexToDec(Serial_RxPacket_1[3])*1);
						ballY=(hexToDec(Serial_RxPacket_1[4])*100)+(hexToDec(Serial_RxPacket_1[5])*10)+(hexToDec(Serial_RxPacket_1[6])*1);
									
						deltaX = ballX - (CAMERA_WIDTH);
						deltaY = ballY - (CAMERA_HEIGHT);
						
						step_target = (int32_t)(deltaX * 3.5f);
						if( (ballX>230&&ballX<390) && (ballY>157&&ballY<175) ) //打击目标范围
						{
							 kkkk=1;
						}else{
							kkkk=0;
						}				
					}
					
				}
				else if(Speed_Test_State.T_state == End)
				{
					Speed_Test_State.T_state = Acting;
					test = 4;					
				}
				break;
				case 3:				//直走任务
					if(Speed_Test_State.T_state == Acting)
				{
					static int count = 0;
					count ++;
					if(count <2)
					{
						MOTOR_Move_1(-13000,180,150,350);//10000
						MOTOR_Move_2(13000,180,150,350);
						MOTOR_Move_3(-13000,180,150,350);
						MOTOR_Move_5(13000,180,150,350);
					}
					//之走完后更新状态
					delay_ms(2);
					if(motor_sta_1 == STOP)
					{
						Speed_Test_State.T_state = End;
						delay_ms(100);
						TIM_Cmd(TIM3,DISABLE);
						TIM_SetCounter(TIM3,0);	
						TIM_Cmd(TIM4,DISABLE);
						TIM_SetCounter(TIM4,0);	
					}
				}
				else if(Speed_Test_State.T_state == End)
				{
					Speed_Test_State.T_state = Acting;
					test = 4;
				}
				break;
				
				default:
					break;
			}
				
		}
}
       


void TIM6_DAC_IRQHandler(void)//PID计算
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        int32_t error = step_target - step_current;
				speed_cmd = PID_Update(&stepPID, step_target, step_current);
				if(ballX==0)
				{
					speed_cmd=0;
					step_target = 0;
					step_current = 0;
					error=0;
				}
       
        if (speed_cmd > 1000) speed_cmd = 1000;//保护作用
        if (speed_cmd < -1000) speed_cmd = -1000;

						if (speed_cmd > 0)
					{
							Stepper_SetDir(0);
							Stepper_SetSpeed((uint32_t)speed_cmd);
						
					}
					else if (speed_cmd < 0)
					{
							Stepper_SetDir(1);
							Stepper_SetSpeed((uint32_t)(-speed_cmd));
					}
					else
					{
						step_current=0;
						Stepper_SetSpeed(0); 
					}
       if(xzq==1)
			 {
						if(ballX!=0)
						{
								if(deltaX<-10)
								{
									maichongkk=(int32_t)(deltaX * 8.0f);
								}
								else if(deltaX>10)
								{
									maichongkk=(int32_t)(deltaX * 8.0f);
								}		
							MOTOR_Move_1(-maichongkk,170,170,200);
							MOTOR_Move_2(maichongkk,170,170,200);
							MOTOR_Move_3(-maichongkk,170,170,200);
							MOTOR_Move_5(maichongkk,170,170,200);		
						}
			 }
    }
		 TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
}


void TIM5_IRQHandler(void)
{
	
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
			
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        if (current_dir == 0 && step_target!=0)
            step_current++;
        if (current_dir == 1 && step_target!=0)
            step_current--;
     }
}
    

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)== SET)
  {	  
				if(ballY>160 && ballY<170)
				{
					Servo_SetPulse(servo_pulse);
				}
				if(ballY<125&&ballY!=0){servo_pulse-=3;}
				else if(ballY>205){servo_pulse+=3;}
				else if(ballY>125&&ballY<165){servo_pulse-=1;}
				else if(ballY>175&&ballY<205){servo_pulse+=1;}
				Servo_SetPulse(servo_pulse);
				if (servo_pulse > 2300) servo_pulse = 2300;//限幅
				if (servo_pulse < 1100)  servo_pulse = 1100;
		if( (ballX>230&&ballX<390) && (ballY>157&&ballY<175) )
				{
					 kkkk=1;
				}else{
					kkkk=0;
				}
		if(kkkk==1)
		{
			time++;
		}else if(kkkk==0){time=0;}
		if(time>=4)
		{
			game_over=1;
		}
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);																	// 清楚定时器中断		
	}
}



//#include "stm32f4xx.h"                  // Device header

//#include "Servo.h"
//#include "delay.h"

//extern float servo_rotation_value;
//extern float servo_pitch_value;
//extern uint8_t servo_rotation_direction;
//extern uint8_t servo_pitch_direction;
///**
//  * ?    ?:?????
//  * ?    ?:?
//  * ? ? ?:?
//  */
//void Servo_Init(void)
//{
//	PWM_Init();									//????????PWM
//}
///**
//  * ?    ?:??????
//  * ?    ?:Angle ????????,??:0~180
//  * ? ? ?:?
//  */
//void Servo_SetAngle1(float Angle)
//{
//	Angle = (Angle > MAX_ANGLE_X) ? MAX_ANGLE_X : ((Angle < MIN_ANGLE_X) ? MIN_ANGLE_X : Angle);  
//	PWM_SetCompare1(Angle / 180 * 2000 + 500);	//?????
//												//???????,??????????????
//}
//void Servo_SetAngle2(float Angle)
//{
//	Angle = (Angle > MAX_ANGLE_Y) ? MAX_ANGLE_Y : ((Angle < MIN_ANGLE_Y) ? MIN_ANGLE_Y : Angle);  
//	PWM_SetCompare2(Angle / 180 * 2000 + 500);	//?????
//												//???????,??????????????
//}

////void Servo_SetPulse(uint16_t pulse_us)
////{
////    TIM_SetCompare1(TIM2, pulse_us);
////}
////uint16_t PWM_GetCompare2()  
////{  
////    return (uint16_t)(TIM2->CCR2);  
////}
///**  
// * ??:??????1  
// * ???:?????????,????  
// */  
//float ReadServoAngle1(void)
//{
//    uint16_t pwmValue = PWM_GetCompare1(); // ???PWM???
//    float angle = (pwmValue - 500) / 2000.0f * 180.0f; // ????
//    return angle;
//}

///**  
// * ??:??????2  
// * ???:?????????,????  
// */  
////float ReadServoAngle2(void)  
////{  
////    uint16_t pwmValue = PWM_GetCompare2(); // ??????TIM2?CH2??PWM????  
////    float angle = (pwmValue - 500) / 2000.0f * 180; // ??????  
////    return angle;  
////}
////void camera_move_increment(float x, float y) {  
////    if (!servo_rotation_direction) { // ??servo_rotation_direction?false  
////        x = -x; // x??  
////    }  
////    if (!servo_pitch_direction) { // ??servo_pitch_direction?false  
////        y = -y; // y??  
////    }  
////    //????,??X?,Y??? 
////    if (MIN_ANGLE_X < servo_rotation_value + x && servo_rotation_value + x < MAX_ANGLE_X) {  
////        servo_rotation_value += x; // ?????  
////				Servo_SetAngle1(servo_rotation_value); // ???????  
////    }  
////    if (MIN_ANGLE_Y < servo_pitch_value + y && servo_pitch_value + y < MAX_ANGLE_Y) { // ????????????????  
////        servo_pitch_value += y; // ?????  
////				Servo_SetAngle2(servo_pitch_value); // ???????  
////    }
////    Delay_ms(10);		
////}  
//////???????????
//////target_x,target_y-???????
//////camera_x,camera_y-???????
////void camera_move_to_target_close_loop(int target_x, int target_y, int laser_x, int laser_y) {  
////	camera_move_increment((float)(target_x - laser_x)*0.01, (float)(target_y - laser_y)*0.01);//??P???
////	//camera_move_increment((float)PID_Compute(&panPID,(target_x - laser_x)), (float)PID_Compute(&tiltPID,(target_y - laser_y)));//PID???
////}

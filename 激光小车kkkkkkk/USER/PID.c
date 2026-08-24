// PID.c
#include "PID.h"

void PID_Init(PID_t *pid, float kp, float ki, float kd, float outMin, float outMax)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0;
    pid->last_error = 0;
    pid->outMin = outMin;
    pid->outMax = outMax;
}

float PID_Update(PID_t *pid, float target, float current)
{
	
	
    float error = target - current;
	  float derivative = error - pid->last_error;
    pid->integral += error;

    
    pid->output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    
    if (pid->output > pid->outMax) pid->output = pid->outMax;
    if (pid->output < pid->outMin) pid->output = pid->outMin;

    pid->last_error = error;
    return pid->output;
}

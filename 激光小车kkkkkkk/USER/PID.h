
#ifndef __PID_H
#define __PID_H

typedef struct {
    float Kp;
    float Ki;
    float Kd;

    float target;   // ???
    float current;  // ???
    float error;    // ????
    
    float integral;
    float last_error;

    float output;
    float outMax;
    float outMin;
} PID_t;
extern PID_t stepPID;  // ??,???
void PID_Init(PID_t* pid, float kp, float ki, float kd, float outMin, float outMax);
float PID_Update(PID_t* pid, float target, float current);

#endif

#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#include "headfile.h"

extern int stepper_motor_rpm, stepper_motor_a;

void stepper_motor_control(float angle1, float angle2);
void stepper_motor_set_zero(bool is_store);
void stepper_motor_reset_zero(void);
void stepper_motor_control_vel(float v1, float v2);
extern bool debounce_flag;
extern float current_angle[2];

#endif
#ifndef __TRACK_H
#define __TRACK_H

#include "headfile.h"

void servo_pid_init(float *pid1, float *pid2);
void servo_pid_control(void);
void stepper_motor_control_pid(float bias_angle);
void set_laser_point(int *p);
void set_target_point(int *p);
float get_point_error(void);
void add_servo_angle(float a1);
void set_pid1(float *pid);

#endif
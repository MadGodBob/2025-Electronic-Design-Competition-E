#ifndef __TASK_H
#define __TASK_H

#include "headfile.h"

#define u8 uint8_t
// ¼¤¹â
extern bool laser_flag;
#define laser_off() {DL_GPIO_setPins(PORTA_PORT, PORTA_LASER_PIN);laser_flag=false;}
#define laser_on() {DL_GPIO_clearPins(PORTA_PORT, PORTA_LASER_PIN);laser_flag=true;}

void task_init(void);
void task_main(void);
void change_task_init(void);
void change_task(void);
void activate_task(void);
void task1(void);
void task2(void);
void task3(void);
void task4(void);
void task5(void);
void task6(void);
void task7(void);
void task8(void);
void task9(void);
void display_task(void);
void car_drive(void);
void set_encoder_count(void);
void circle_detect(void);
void set_default_laser_point(int *p);
void set_ellipse(int a, int b);

extern float encoder_count;
extern float roll, pitch, yaw;
extern float turn_speed;

#endif
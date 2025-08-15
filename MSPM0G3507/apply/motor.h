#ifndef __MOTOR_H
#define __MOTOR_H

#include "headfile.h"

#define SPEED_RECORD_NUM 5  //速度记录值，用于均值滤波
typedef struct {
uint8_t direct;     //方向
int32_t countnum;   //总计数值
int32_t lastcount;  //上一次计数值
float speed;        //电机速度
float speed_Record[SPEED_RECORD_NUM];  //记录电机近20次测速的结果
}  encoder_t;

typedef struct {
uint8_t dir;          //方向
float pwmDuty; //PWM占空比
float speed;   //目标速度(最大1000)
float encoder_speed;    //转换到编码器速度时的速度
float pos ;   //目标位置
encoder_t encoder; //编码器
float error;
float last_error;
float integral;
} motor_t;

void motor_init(void);
void Set_Freq(uint32_t freq);
void Set_duty(float duty,uint8_t channel);
void change_motor_mode(uint8_t mode1, uint8_t mode2);
void motor_speed(float speed1, float speed2);
void motor_stop(void);
void motor_disable(void);
void ENCODER1A_INST_IRQHandler(void);
void ENCODER2A_INST_IRQHandler(void);
void CLOCK_INST_IRQHandler(void);
void set_bias(float bias);
void racecar(float speed, float angle, float k, float turn_D);
void single_motor_pid(motor_t *motor);
void motor_clear_i_d(void);

extern motor_t motor_l, motor_r;
extern int global_count;
extern float PID[3];

#endif
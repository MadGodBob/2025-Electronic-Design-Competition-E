#ifndef __MOTOR_H
#define __MOTOR_H

#include "headfile.h"

#define SPEED_RECORD_NUM 5  //�ٶȼ�¼ֵ�����ھ�ֵ�˲�
typedef struct {
uint8_t direct;     //����
int32_t countnum;   //�ܼ���ֵ
int32_t lastcount;  //��һ�μ���ֵ
float speed;        //����ٶ�
float speed_Record[SPEED_RECORD_NUM];  //��¼�����20�β��ٵĽ��
}  encoder_t;

typedef struct {
uint8_t dir;          //����
float pwmDuty; //PWMռ�ձ�
float speed;   //Ŀ���ٶ�(���1000)
float encoder_speed;    //ת�����������ٶ�ʱ���ٶ�
float pos ;   //Ŀ��λ��
encoder_t encoder; //������
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
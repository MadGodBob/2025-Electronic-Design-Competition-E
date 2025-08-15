/**********************************************************
***	激光追踪例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include "headfile.h"
#include "track.h"

int laser_point[2] = {0};       // 激光点在相机中的坐标（固定不变）

// pid步进电机
struct Servo{
    float pid_1[3];
    float pid_2[3];
    float error1;
    float error2;
    float last_error1;
    float last_error2;
    float error1_integral;
    float error2_integral;
    int current_point[2];
    int target_point[2];
    float angle1;
    float angle2;
    float feedforward1;
    float feedforward2;     // 前馈pid的前馈部分
};
struct Servo servo;

// 初始化PID参数
void servo_pid_init(float *pid1, float *pid2)
{
    float k = 100.0;
    for(u8 i=0;i<3;i++)
    {
        servo.pid_1[i] = pid1[i]/k;
        servo.pid_2[i] = pid2[i]/k;
    }
    servo.error1=0;
    servo.error2=0;
    servo.last_error1=0;
    servo.last_error2=0;
    servo.error1_integral=0;
    servo.error2_integral=0;
    servo.current_point[0] = 0;
    servo.current_point[1] = 0;
    servo.target_point[0] = 0;
    servo.target_point[1] = 0;
    servo.feedforward1 = 0;
    servo.feedforward2 = 0;
}

// 设置步进电机1的PID
void set_pid1(float *pid)
{
    float k = 100.0;
    for(u8 i=0;i<3;i++)
    {
        servo.pid_1[i] = pid[i]/k;
    }
}

// PID控制，将当前坐标current_point朝目标坐标target_point靠近
void servo_pid_control(void)
{
    if(servo.target_point[0] == 0 && servo.target_point[1] == 0) return ;
    
    servo.error1 = servo.target_point[0] - servo.current_point[0];
    servo.error1_integral += servo.error1;
    if(servo.error1 == 0.0) {servo.error1_integral = 0;}
    servo.angle1 += servo.pid_1[0]*servo.error1 + servo.pid_1[1]*servo.error1_integral + servo.pid_1[2]*(servo.error1-servo.last_error1);
    servo.last_error1 = servo.error1;
    
    servo.error2 = servo.target_point[1] - servo.current_point[1];
    servo.error2_integral += servo.error2;
    if(servo.error2 == 0.0) {servo.error2_integral = 0;}
    servo.angle2 += servo.pid_2[0]*servo.error2 + servo.pid_2[1]*servo.error2_integral + servo.pid_2[2]*(servo.error2-servo.last_error2);
    servo.last_error2 = servo.error2;
}

// 获取到当前坐标和目标坐标的误差（距离的平方）
float get_point_error(void)
{
    if(servo.error1 == 0 && servo.error2 == 0) {return 1000000;}
    return pow((float)(servo.error1+servo.error2), 2);
}

void stepper_motor_control_pid(float bias_angle)
{
//    // 陀螺仪纠正使用pid
//    static float output_angle=0.0;
//    static float bias_angle_error=0.0;
//    static float last_bias_angle_error=0.0;
//    float bias_angle_pid[3] = {0.5, 0.0, 0.2};
//    bias_angle_error = bias_angle - 0.0;    // 0度为目标角度
////    bias_angle_error = bias_angle - (-servo.angle1);    // 云台输出的角度的负值为目标角度
//    if(fabs(bias_angle_error) > 1.0){
//        output_angle += bias_angle_error*bias_angle_pid[0] + last_bias_angle_error*bias_angle_pid[2];
//    }
//    last_bias_angle_error = bias_angle_error;
    
    servo_pid_control();
    stepper_motor_control(bias_angle + servo.angle1, servo.angle2);
}

void set_laser_point(int *p)
{
    servo.current_point[0] = p[0];
    servo.current_point[1] = p[1];
}

void set_target_point(int *p)
{
    servo.target_point[0] = p[0];
    servo.target_point[1] = p[1];
}

void add_servo_angle(float a1)
{
    servo.angle1 += a1;
}
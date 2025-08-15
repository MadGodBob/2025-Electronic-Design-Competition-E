/**********************************************************
***	小车运动学解耦，空间定位例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
// 对小车进行三维坐标运动建模，估算其相对于与标靶中心点的坐标和姿态
#include "location.h"
#include "kalman_filter.h"

#define square_side 100.0       // 巡线正方形的边长cm
#define distance_from_target 50.0   // 与标靶纸的垂直距离
#define square_perimeter 4*square_side  // 正方形周长
#define encoder_to_cm_k 100.0/1700.0    // 编码器读数 * encoder_to_cm_k = cm为单位的长度
#define cm_to_process_k 0.97     // 处理时按实际cm的cm_to_process_k比例计算

float relative_l = 0.0;         // 距离current_point的位移
float absolute_point[2] = {0.0};    // 小车绝对坐标系下的坐标
float angle_to_imu_zero_yaw = 0.0;  // 陀螺仪归零方向与y轴负方向的夹角，逆时针为正
float current_point_encoder_count = 0.0;    // 小车已经经过的点位对应的编码器读数
enum CurrentPoint current_point = B;    // 小车已经过的点位

// 对角度估计进行卡尔曼滤波
KalmanFilter angle_filter;

// 结算小车当前的位置，获得其相对于与标靶中心点的夹角angle
// 夹角angle定义为 小车朝向 与 垂直于标靶方向并以面向标靶的方向为正 的夹角
float get_angle(void)
{
    set_relative_l();
    convert_absolute_to_xy();
    convert_xy_to_angle();
    angle_to_imu_zero_yaw *= 180/3.1415926;
    
    // 这里是因为D点直角弯的平滑难以处理，故直接拿出来判断加角度了
    if(current_point == D){
        angle_to_imu_zero_yaw *= 0.5;
    }
    
    // 卡尔曼滤波器初始化
    static bool kalman_filter_init = false;
    if(!kalman_filter_init) {
        Kalman_Init(&angle_filter, 0.05f, 1.0f);
        kalman_filter_init = true;
    }
    // 使用卡尔曼滤波器对估算角angle进行滤波
    angle_to_imu_zero_yaw = Kalman_Update(&angle_filter, angle_to_imu_zero_yaw);
    
//    display_6_8_string(0, 3, "relative_l:");
//    display_6_8_number(80, 3, relative_l);
//    display_6_8_string(0, 3, "angle:");
//    display_6_8_number(60, 3, angle_to_imu_zero_yaw);
    
    return -angle_to_imu_zero_yaw;
}

// 绝对坐标系转化为小车面向标靶中心时的角度（陀螺仪归零方向与y轴负方向的夹角，逆时针为正）
void convert_xy_to_angle(void)
{
    angle_to_imu_zero_yaw = atan(-absolute_point[0]/absolute_point[1]);
}

// 根据相对位移转化为绝对坐标系(标靶正下方为原点，标靶水平方向为x轴，垂直方向为y轴，x正方向为逆时针)
// 同时对特定线段上的x y进行合适的修改，以适应实际情况
// DB线段上令y变大一点，BA上令x变大一点
void convert_absolute_to_xy(void)
{
    switch(current_point)
    {
        case A: absolute_point[0] = square_side/2;
                absolute_point[1] = distance_from_target + relative_l;
                absolute_point[1] -= 20;
                break;
        case B: absolute_point[0] = relative_l - square_side/2;
                absolute_point[1] = distance_from_target;
                absolute_point[0] += 30;
                break;
        case C: absolute_point[0] = square_side/2 - relative_l;
                absolute_point[1] = distance_from_target + square_side;
                absolute_point[0] += 20;
                break;
        case D: absolute_point[0] = -square_side/2;
                absolute_point[1] = distance_from_target + square_side - relative_l;
                break;
    }
}

// 小车循到直角弯，判定为下一个点
enum CurrentPoint switch_to_next_point(enum CurrentPoint P)
{
    switch(P)
    {
        case A: return C;
        case C: return D;
        case D: return B;
        case B: return A;
    }
}

void init_relative_l(float l)
{
    l /= encoder_to_cm_k;
    l = encoder_count - l;
    current_point_encoder_count = l;
}

// 使用编码器读数更新相对位移relative_l
void set_relative_l(void)
{
    float encoder_count = (motor_l.encoder.countnum + motor_r.encoder.countnum)/2.0;
    relative_l = (encoder_count - current_point_encoder_count)*encoder_to_cm_k*cm_to_process_k;
}

// 到达了新点位(直角弯)，同时相对位移relative_l归零
void approach_point(void)
{
    current_point_encoder_count = (motor_l.encoder.countnum + motor_r.encoder.countnum)/2.0;
    current_point = switch_to_next_point(current_point);
    relative_l = 0;
}

void set_current_point(enum CurrentPoint P)
{
    current_point = P;
}
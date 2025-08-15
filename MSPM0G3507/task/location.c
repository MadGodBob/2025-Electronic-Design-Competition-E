/**********************************************************
***	С���˶�ѧ����ռ䶨λ����
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
// ��С��������ά�����˶���ģ��������������������ĵ���������̬
#include "location.h"
#include "kalman_filter.h"

#define square_side 100.0       // Ѳ�������εı߳�cm
#define distance_from_target 50.0   // ����ֽ�Ĵ�ֱ����
#define square_perimeter 4*square_side  // �������ܳ�
#define encoder_to_cm_k 100.0/1700.0    // ���������� * encoder_to_cm_k = cmΪ��λ�ĳ���
#define cm_to_process_k 0.97     // ����ʱ��ʵ��cm��cm_to_process_k��������

float relative_l = 0.0;         // ����current_point��λ��
float absolute_point[2] = {0.0};    // С����������ϵ�µ�����
float angle_to_imu_zero_yaw = 0.0;  // �����ǹ��㷽����y�Ḻ����ļнǣ���ʱ��Ϊ��
float current_point_encoder_count = 0.0;    // С���Ѿ������ĵ�λ��Ӧ�ı���������
enum CurrentPoint current_point = B;    // С���Ѿ����ĵ�λ

// �ԽǶȹ��ƽ��п������˲�
KalmanFilter angle_filter;

// ����С����ǰ��λ�ã������������������ĵ�ļн�angle
// �н�angle����Ϊ С������ �� ��ֱ�ڱ�з����������еķ���Ϊ�� �ļн�
float get_angle(void)
{
    set_relative_l();
    convert_absolute_to_xy();
    convert_xy_to_angle();
    angle_to_imu_zero_yaw *= 180/3.1415926;
    
    // ��������ΪD��ֱ�����ƽ�����Դ�����ֱ���ó����жϼӽǶ���
    if(current_point == D){
        angle_to_imu_zero_yaw *= 0.5;
    }
    
    // �������˲�����ʼ��
    static bool kalman_filter_init = false;
    if(!kalman_filter_init) {
        Kalman_Init(&angle_filter, 0.05f, 1.0f);
        kalman_filter_init = true;
    }
    // ʹ�ÿ������˲����Թ����angle�����˲�
    angle_to_imu_zero_yaw = Kalman_Update(&angle_filter, angle_to_imu_zero_yaw);
    
//    display_6_8_string(0, 3, "relative_l:");
//    display_6_8_number(80, 3, relative_l);
//    display_6_8_string(0, 3, "angle:");
//    display_6_8_number(60, 3, angle_to_imu_zero_yaw);
    
    return -angle_to_imu_zero_yaw;
}

// ��������ϵת��ΪС������������ʱ�ĽǶȣ������ǹ��㷽����y�Ḻ����ļнǣ���ʱ��Ϊ����
void convert_xy_to_angle(void)
{
    angle_to_imu_zero_yaw = atan(-absolute_point[0]/absolute_point[1]);
}

// �������λ��ת��Ϊ��������ϵ(������·�Ϊԭ�㣬���ˮƽ����Ϊx�ᣬ��ֱ����Ϊy�ᣬx������Ϊ��ʱ��)
// ͬʱ���ض��߶��ϵ�x y���к��ʵ��޸ģ�����Ӧʵ�����
// DB�߶�����y���һ�㣬BA����x���һ��
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

// С��ѭ��ֱ���䣬�ж�Ϊ��һ����
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

// ʹ�ñ����������������λ��relative_l
void set_relative_l(void)
{
    float encoder_count = (motor_l.encoder.countnum + motor_r.encoder.countnum)/2.0;
    relative_l = (encoder_count - current_point_encoder_count)*encoder_to_cm_k*cm_to_process_k;
}

// �������µ�λ(ֱ����)��ͬʱ���λ��relative_l����
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
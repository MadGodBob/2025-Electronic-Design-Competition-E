/**********************************************************
***	����������ƽӿ�����
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include "headfile.h"
#include "stepper_motor.h"

// ���Ʋ�������Ĵ��ڽ��г�ʼ��
void stepper_motor_init(void)
{
    NVIC_ClearPendingIRQ(UART_2_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_2_INST_INT_IRQN);
}

// �����������
void stepper_motor_reset_zero(void)
{
    Emm_V5_Origin_Trigger_Return(0x01, 0, false, UART_2_INST);
    Emm_V5_Origin_Trigger_Return(0x02, 0, false, UART_0_INST);
}


// λ�ù�������
void stepper_motor_set_zero(bool is_store)
{
    Emm_V5_Origin_Set_O(0x01, is_store, UART_2_INST);
    Emm_V5_Origin_Set_O(0x02, is_store, UART_0_INST);
}

// �ٶ�ģʽ���Ʋ��������ʵ�ⲻ��λ��ģʽ��
void stepper_motor_control_vel(float v1, float v2)
{
    #define ROUND(x) (int)(x + 0.5f)
    Emm_V5_Vel_Control(0x01, v1 > 0 ? 1 : 0, ROUND(fabs(v1)), 250, false, UART_2_INST);
    if(v2 != 0.0){
        Emm_V5_Vel_Control(0x02, v2 < 0 ? 1 : 0, ROUND(fabs(v2)), 250, false, UART_0_INST);
    }
}

// λ��ģʽ���Ʋ��������ͬʱ������λ���������������
// ��λ�������������Ƕ���-180~180ѭ������������ǵ�������-180������360��540��
int stepper_motor_rpm = 5000, stepper_motor_a = 256;
bool debounce_flag = false;  // ������־
float current_angle[2] = {0};
void stepper_motor_control(float angle1, float angle2)
{
    #define ROUND(x) (int)(x + 0.5f)
    #define angle2_max 100
    static float last_angle1 = 0.0;
    // �����
    float detla = angle1 - last_angle1;
    int n;
    if(detla < -180)
    {
        n = ROUND(fabs(detla)/360);
        n = (n > 0) ? n : 1;
        angle1 += 360*n;
    }
    else if(detla > 180)
    {
        n = ROUND(fabs(detla)/360);
        n = (n > 0) ? n : 1;
        angle1 -= 360*n;
    }
    // ��������
    static float last_unwarpped_angle1 = 0.0;
    if(debounce_flag){
        if(last_unwarpped_angle1 != 0.0 && fabs(angle1 - last_unwarpped_angle1) > 10)
        {
            // ���ֶ�������һ�����
            angle1 = last_unwarpped_angle1;
        }
        else{
            last_unwarpped_angle1 = angle1;
        }
    }
    
    last_angle1 = angle1;
    // ���������λ��
    angle2 = (angle2 > angle2_max) ? angle2_max : angle2;
    angle2 = (angle2 < -angle2_max) ? -angle2_max : angle2;
    // �����ƽ������һ����ͬ���򲻿��ƣ���ֹ����
    static float last_output_angle1=0.0, last_output_angle2=0.0;
//    if(angle1 != last_output_angle1) {
        Emm_V5_Pos_Control(0x01, angle1 > 0 ? 1 : 0, stepper_motor_rpm, stepper_motor_a, fabs(angle1)*12800/360, true, false, UART_2_INST);
//        last_output_angle1 = angle1;
//    }
    if(angle2 != last_output_angle2) {
        Emm_V5_Pos_Control(0x02, angle2 < 0 ? 1 : 0, 5000, 0, fabs(angle2)*2000/360, true, false, UART_0_INST);
        last_output_angle2 = angle2;
    }
    
    current_angle[0] = angle1;
    current_angle[1] = angle2;
}
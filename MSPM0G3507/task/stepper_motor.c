/**********************************************************
***	步进电机控制接口例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include "headfile.h"
#include "stepper_motor.h"

// 控制步进电机的串口进行初始化
void stepper_motor_init(void)
{
    NVIC_ClearPendingIRQ(UART_2_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_2_INST_INT_IRQN);
}

// 步进电机回零
void stepper_motor_reset_zero(void)
{
    Emm_V5_Origin_Trigger_Return(0x01, 0, false, UART_2_INST);
    Emm_V5_Origin_Trigger_Return(0x02, 0, false, UART_0_INST);
}


// 位置归零设置
void stepper_motor_set_zero(bool is_store)
{
    Emm_V5_Origin_Set_O(0x01, is_store, UART_2_INST);
    Emm_V5_Origin_Set_O(0x02, is_store, UART_0_INST);
}

// 速度模式控制步进电机（实测不如位置模式）
void stepper_motor_control_vel(float v1, float v2)
{
    #define ROUND(x) (int)(x + 0.5f)
    Emm_V5_Vel_Control(0x01, v1 > 0 ? 1 : 0, ROUND(fabs(v1)), 250, false, UART_2_INST);
    if(v2 != 0.0){
        Emm_V5_Vel_Control(0x02, v2 < 0 ? 1 : 0, ROUND(fabs(v2)), 250, false, UART_0_INST);
    }
}

// 位置模式控制步进电机，同时进行相位解包裹、消抖处理
// 相位解包裹就是输入角度是-180~180循环，但是输出是递增，从-180递增到360、540等
int stepper_motor_rpm = 5000, stepper_motor_a = 256;
bool debounce_flag = false;  // 防抖标志
float current_angle[2] = {0};
void stepper_motor_control(float angle1, float angle2)
{
    #define ROUND(x) (int)(x + 0.5f)
    #define angle2_max 100
    static float last_angle1 = 0.0;
    // 解包裹
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
    // 防抖处理
    static float last_unwarpped_angle1 = 0.0;
    if(debounce_flag){
        if(last_unwarpped_angle1 != 0.0 && fabs(angle1 - last_unwarpped_angle1) > 10)
        {
            // 出现抖动，上一个输出
            angle1 = last_unwarpped_angle1;
        }
        else{
            last_unwarpped_angle1 = angle1;
        }
    }
    
    last_angle1 = angle1;
    // 电机二限制位移
    angle2 = (angle2 > angle2_max) ? angle2_max : angle2;
    angle2 = (angle2 < -angle2_max) ? -angle2_max : angle2;
    // 若控制结果与上一个相同，则不控制，防止抖动
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
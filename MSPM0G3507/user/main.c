/**********************************************************
***	2025年电赛E题开源例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include "ti_msp_dl_config.h"
#include "headfile.h"

// 电机1的rx接PA8   电机2的rx接PB2     openmv的TX接PB3
int main(void)
{
    SYSCFG_DL_init();	      //系统资源配置初始化
    OLED_Init();			//显示屏初始化
    uart_init();            // 串口0初始化
    motor_init();           // 电机PWM初始化
    jy901_init();           // 陀螺仪初始化
    cali_yaw();             // 标定陀螺仪，将当前朝向记为零度
    stepper_motor_reset_zero(); // 步进电机回零
    task_init();            // 任务主逻辑初始化
    motor_stop();           // 轮子速度归零
    
    // 中断优先级，使对陀螺仪数据的处理处于最高优先级（有没有无所谓）
    NVIC_SetPriority(UART_1_INST_INT_IRQN, 1);
    
    int instant_count=0, fps=0, last_fps=0;
    while(1)
    {             
        // 显示fps
        fps++;
        if(global_count - instant_count >= 100) {
            instant_count = global_count;
            if(fps != last_fps){
//                display_6_8_string(70, 1, "FPS:");
                display_6_8_number(100, 1, fps);
                last_fps = fps;
            }
            fps = 0;
        }
        
        // 获取灰传数据
        get_gray();
        
//        // 显示十二路灰传的结果
//        for(uint8_t i=0;i<12;i++){
//            display_6_8_number(i*6, 0, gray_data[i]);
//        }
                
        // 显示陀螺仪的yaw数据
//        Serial_JY901S_Zero_Yaw();
//        getAngle(&roll, &pitch, &yaw);
        display_6_8_string(0, 2, "yaw:");
        display_6_8_number(40, 2, yaw);
        
        // 运行任务主逻辑
        task_main();
        
        // pid控制轮子电机
        single_motor_pid(&motor_l);
        single_motor_pid(&motor_r);
        // 电机PWM设置
        Set_duty(motor_l.pwmDuty, 0);
        Set_duty(motor_r.pwmDuty, 1);
        
//        // 显示按键
//        displayKeys();
        
        // 显示两个轮子的编码器读数
        display_6_8_number(0, 1, motor_l.encoder.speed);
        display_6_8_number(50, 1, motor_r.encoder.speed);
        if(fabs(motor_l.encoder.speed) < 0.001){
            display_6_8_string(0+6, 1, "     ");
        }
        if(fabs(motor_r.encoder.speed) < 0.001){
            display_6_8_string(50+6, 1, "     ");
        }
        
        // 蜂鸣器循环喂养函数，用于实现非阻塞式的蜂鸣器
        Beep_feed();

    }
}
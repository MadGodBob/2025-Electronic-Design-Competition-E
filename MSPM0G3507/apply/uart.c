/**********************************************************
***	OPENMV串口数据处理例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include <stdio.h>
#include "headfile.h"
#include "uart.h"
#include "string.h"

// 串口0初始化
void uart_init(void)
{
    // 清空中断的状态
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    // 使能中断
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
}

/***
    OPENMV数据传输协议：
    矩形中心点坐标     #0123|0456$     =>      123和456
    激光点坐标         (0123 0456)     =>      123和456  
    椭圆的长轴和短轴   @0123,0456%     =>      123和456
***/
// 起始符、隔断符、结束符
#define FRAME_START   '#'
#define FRAME_DELIM   '|'
#define FRAME_END     '$'
#define LASER_START   '('
#define LASER_DELIM   ' '
#define LASER_END     ')'
#define CIRCLE_START   '@'
#define CIRCLE_DELIM   ','
#define CIRCLE_END     '%'
#define NUM_DIGITS    4
#define IS_DIGIT(x)   ((x) >= '0' && (x) <= '9')

// 全局变量用于存储三种不同类型的数据
int target_point[2] = {0};    // 存储 #0123|0456$ 格式数据
int laser_data[2] = {0};       // 存储 (0123 0456) 格式数据
int circle_data[2] = {0};      // 存储 @0123,0456% 格式数据

void UART_0_INST_IRQHandler(void) 
{
    static enum { 
        WAIT_START,      // 等待起始符
        RECV_NUM1,       // 接收第一个数字
        WAIT_DELIM,      // 等待分隔符
        RECV_NUM2,       // 接收第二个数字
        WAIT_END         // 等待结束符
    } state = WAIT_START;
    
    static int num1, num2;        // 直接存储解析后的整数
    static uint8_t digit_pos = 0;
    static uint8_t current_num;   // 当前正在解析的数字（0=num1, 1=num2）
    static uint8_t frame_type;    // 当前帧的类型（0=#, 1=(, 2=@）

    uint8_t data = DL_UART_Main_receiveData(UART_0_INST);

    switch (state) {
        case WAIT_START:
            if (data == FRAME_START) {
                frame_type = 0;
                num1 = 0;
                num2 = 0;
                digit_pos = 0;
                current_num = 0;
                state = RECV_NUM1;
            }
            else if (data == LASER_START) {
                frame_type = 1;
                num1 = 0;
                num2 = 0;
                digit_pos = 0;
                current_num = 0;
                state = RECV_NUM1;
            }
            else if (data == CIRCLE_START) {
                frame_type = 2;
                num1 = 0;
                num2 = 0;
                digit_pos = 0;
                current_num = 0;
                state = RECV_NUM1;
            }
            // 其他字符忽略，保持WAIT_START状态
            break;

        case RECV_NUM1:
            if (IS_DIGIT(data)) {
                if (current_num == 0) {
                    num1 = num1 * 10 + (data - '0');
                } else {
                    num2 = num2 * 10 + (data - '0');
                }
                digit_pos++;
                if (digit_pos >= NUM_DIGITS) {
                    // 根据帧类型决定下一个状态
                    if (frame_type == 0) {
                        state = WAIT_DELIM;  // #格式使用|分隔符
                    } else if (frame_type == 1) {
                        state = WAIT_DELIM; // (格式使用空格分隔符
                    } else if (frame_type == 2) {
                        state = WAIT_DELIM;  // @格式使用,分隔符
                    }
                }
            } else {
                state = WAIT_START; // 非数字，重置状态机
            }
            break;

        case WAIT_DELIM:
            // 检查分隔符是否与帧类型匹配
            if ((frame_type == 0 && data == FRAME_DELIM) ||
                (frame_type == 1 && data == LASER_DELIM) ||
                (frame_type == 2 && data == CIRCLE_DELIM)) {
                digit_pos = 0;
                current_num = 1; // 切换到解析 num2
                state = RECV_NUM2;
            } else {
                state = WAIT_START; // 分隔符不匹配，重置
            }
            break;

        case RECV_NUM2:
            if (IS_DIGIT(data)) {
                num2 = num2 * 10 + (data - '0');
                digit_pos++;
                if (digit_pos >= NUM_DIGITS) {
                    state = WAIT_END;
                }
            } else {
                state = WAIT_START; // 非数字，重置
            }
            break;

        case WAIT_END:
            // 检查结束符是否与帧类型匹配
            if ((frame_type == 0 && data == FRAME_END) ||
                (frame_type == 1 && data == LASER_END) ||
                (frame_type == 2 && data == CIRCLE_END)) {
                // 根据帧类型存储数据
                switch (frame_type) {
                    case 0:  // #格式
                        target_point[0] = num1;
                        target_point[1] = num2;
                        break;
                    case 1:  // (格式
                        laser_data[0] = num1;
                        laser_data[1] = num2;
                        if(laser_flag) set_default_laser_point(laser_data);
                        break;
                    case 2:  // @格式
                        circle_data[0] = num1;
                        circle_data[1] = num2;
                        set_ellipse(num1, num2);
                        break;
                }
            }
            state = WAIT_START; // 无论是否成功，都重置状态机
            break;
    }
}

void SendString(char *str)
{
    while(*str != '\0')
    {
        DL_UART_Main_transmitDataBlocking(UART_0_INST, *str++);
    }
}

void SendNumber(float num)
{
    char str[9];
    if(num < 0) {num=-num;SendString("-");}
    sprintf(str, "%f", num);
    SendString(str);
}
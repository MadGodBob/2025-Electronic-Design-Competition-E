/**********************************************************
***	OPENMV�������ݴ�������
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include <stdio.h>
#include "headfile.h"
#include "uart.h"
#include "string.h"

// ����0��ʼ��
void uart_init(void)
{
    // ����жϵ�״̬
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    // ʹ���ж�
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
}

/***
    OPENMV���ݴ���Э�飺
    �������ĵ�����     #0123|0456$     =>      123��456
    ���������         (0123 0456)     =>      123��456  
    ��Բ�ĳ���Ͷ���   @0123,0456%     =>      123��456
***/
// ��ʼ�������Ϸ���������
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

// ȫ�ֱ������ڴ洢���ֲ�ͬ���͵�����
int target_point[2] = {0};    // �洢 #0123|0456$ ��ʽ����
int laser_data[2] = {0};       // �洢 (0123 0456) ��ʽ����
int circle_data[2] = {0};      // �洢 @0123,0456% ��ʽ����

void UART_0_INST_IRQHandler(void) 
{
    static enum { 
        WAIT_START,      // �ȴ���ʼ��
        RECV_NUM1,       // ���յ�һ������
        WAIT_DELIM,      // �ȴ��ָ���
        RECV_NUM2,       // ���յڶ�������
        WAIT_END         // �ȴ�������
    } state = WAIT_START;
    
    static int num1, num2;        // ֱ�Ӵ洢�����������
    static uint8_t digit_pos = 0;
    static uint8_t current_num;   // ��ǰ���ڽ��������֣�0=num1, 1=num2��
    static uint8_t frame_type;    // ��ǰ֡�����ͣ�0=#, 1=(, 2=@��

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
            // �����ַ����ԣ�����WAIT_START״̬
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
                    // ����֡���;�����һ��״̬
                    if (frame_type == 0) {
                        state = WAIT_DELIM;  // #��ʽʹ��|�ָ���
                    } else if (frame_type == 1) {
                        state = WAIT_DELIM; // (��ʽʹ�ÿո�ָ���
                    } else if (frame_type == 2) {
                        state = WAIT_DELIM;  // @��ʽʹ��,�ָ���
                    }
                }
            } else {
                state = WAIT_START; // �����֣�����״̬��
            }
            break;

        case WAIT_DELIM:
            // ���ָ����Ƿ���֡����ƥ��
            if ((frame_type == 0 && data == FRAME_DELIM) ||
                (frame_type == 1 && data == LASER_DELIM) ||
                (frame_type == 2 && data == CIRCLE_DELIM)) {
                digit_pos = 0;
                current_num = 1; // �л������� num2
                state = RECV_NUM2;
            } else {
                state = WAIT_START; // �ָ�����ƥ�䣬����
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
                state = WAIT_START; // �����֣�����
            }
            break;

        case WAIT_END:
            // ���������Ƿ���֡����ƥ��
            if ((frame_type == 0 && data == FRAME_END) ||
                (frame_type == 1 && data == LASER_END) ||
                (frame_type == 2 && data == CIRCLE_END)) {
                // ����֡���ʹ洢����
                switch (frame_type) {
                    case 0:  // #��ʽ
                        target_point[0] = num1;
                        target_point[1] = num2;
                        break;
                    case 1:  // (��ʽ
                        laser_data[0] = num1;
                        laser_data[1] = num2;
                        if(laser_flag) set_default_laser_point(laser_data);
                        break;
                    case 2:  // @��ʽ
                        circle_data[0] = num1;
                        circle_data[1] = num2;
                        set_ellipse(num1, num2);
                        break;
                }
            }
            state = WAIT_START; // �����Ƿ�ɹ���������״̬��
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
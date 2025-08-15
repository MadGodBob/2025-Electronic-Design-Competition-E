/**********************************************************
***	JY901���ݴ�������
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include "jy901.h" 
#include "stdio.h"

// ����1��ʼ��
void jy901_init(void)
{
    // ����жϵ�״̬
	NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    // ʹ���ж�
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
}

// ��jy901����һ����������
void Serial_JY901S_Zero_Yaw(void)
{
    send_data(0xFF);
    send_data(0xAA);
    send_data(0x03);
    send_data(0x0C);
    send_data(0x00);
}

// ���㺽���
void cali_yaw(void)
{
    send_data(0xFF);
    send_data( 0xAA);
    send_data(0x69);
    send_data(0x88);
    send_data(0xB5);
    delay_ms(100);
    send_data(0xFF);
    send_data(0xAA);
    send_data(0x01);
    send_data(0x04);
    send_data(0x00);
    delay_ms(100);
    send_data(0xFF);
    send_data(0xAA);
    send_data(0x00);
    send_data(0x00);
    send_data(0x00);
}

void send_data(u8 d)
{
    DL_UART_Main_transmitDataBlocking(UART_1_INST, d);
}

/******** ����1 �жϷ����� ***********/
char gEchoData;
volatile uint16_t recv_index = 0, flag, recv_data[11], recv_number[2], num_recv_index=0, num_len[2], num_sign[2] = {1, 1};
volatile char number_str1[4] = {0, 0, 0, 0}, number_str2[4] = {0, 0, 0, 0};
#define DATA_LENGTH 10
uint8_t angle_data[DATA_LENGTH], data_index=0, rx_flag=0;
void UART_1_INST_IRQHandler(void)
{
	
    gEchoData = DL_UART_Main_receiveData(UART_1_INST);
    
    uint8_t RxData = gEchoData;
    if(RxData == 0x53){
        // ���տ�ʼ
        rx_flag=1;
        data_index=0;
    }
    else if(RxData == 0x55){
        // ���ս���
        rx_flag=0;
    }
    
    if(rx_flag){
        angle_data[data_index++] = RxData;
    }
}

// ���ڻ������ݴ�����������ŷ����roll pitch yaw
void getAngle(float* roll, float* pitch, float* yaw)
{
    float last_yaw=0.0;
    short RollH=(short)angle_data[2], RollL=(short)angle_data[1];
    short PitchH=(short)angle_data[4], PitchL=(short)angle_data[3];
    short YawH=(short)angle_data[6], YawL=(short)angle_data[5];
        
    *roll = (float)((RollH<<8)|RollL)/32768.0*180.0;
    *pitch = (float)((PitchH<<8)|PitchL)/32768.0*180.0;
    *yaw = (float)((YawH<<8)|YawL)/32768.0*180.0;
    
    *roll = *roll > 180 ? *roll - 360 : *roll;
    *pitch = *pitch > 180 ? *pitch - 360 : *pitch;
    
    // ��������ֹ����
    float max_bias_yaw = 5;
    float bias_yaw = fabs(*yaw - last_yaw);
//    if(!(bias_yaw < max_bias_yaw && last_yaw != 0.0)) {return ;}
    *yaw = *yaw > 180 ? *yaw - 360 : *yaw;
    last_yaw = *yaw;
}

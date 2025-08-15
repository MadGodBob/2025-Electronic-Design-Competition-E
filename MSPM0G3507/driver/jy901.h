 #ifndef __JY901_H__
 #define __JY901_H__
 
#include "headfile.h"

void jy901_init(void);
void USART1_IRQHandler(void);
void Serial_JY901S_Zero_Yaw(void);
void cali_yaw(void);
void getAngle(float* roll, float* pitch, float* yaw);
void send_data(u8 d);
 
 
#endif

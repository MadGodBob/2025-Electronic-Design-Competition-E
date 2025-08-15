#ifndef __UART_H
#define __UART_H

void uart_init(void);
void UART_0_INST_IRQHandler(void);
void SendString(char *str);
void SendNumber(float num);
extern int target_point[2], laser_data[2], circle_data[2];

#endif
#include "headfile.h"
#include "beep.h"

volatile int beep_time;

// 1��0����
void Beep(uint8_t s)
{
    if(s)
        DL_GPIO_setPins(PORTA_PORT, PORTA_BEEP_PIN);
    else
        DL_GPIO_clearPins(PORTA_PORT, PORTA_BEEP_PIN);
}

// ���������֡�һ�����ٶ�ȡ���ڳ����ٶ�
void Beep_up(void)
{
    led(0, 0);
    beep_time = 10;
}

void Beep_feed(void)
{
    if(beep_time>0){
        beep_time--;
        Beep(1);
    }
    else{
        led(1, 0);
        Beep(0);
    }
}
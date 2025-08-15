#include "headfile.h"
#include "led.h"

void led_drive(uint8_t s, GPIO_Regs* gpio, uint32_t pins)
{
    if(s)
        DL_GPIO_setPins(gpio, pins);
    else
        DL_GPIO_clearPins(gpio, pins);
}

void led(uint8_t s, uint16_t index)
{
    switch(index)
    {
        case 0:led_drive(s, LED_PORT, LED_PIN_LED_PIN);break;
//        case 1:led_drive(s, LED_PIN_LED_BLUE_PORT, LED_PIN_LED_BLUE_PIN);break;
//        case 2:led_drive(s, LED_PIN_LED_RED_PORT, LED_PIN_LED_RED_PIN);break;
//        case 3:led_drive(s, LED_PIN_LED_GREEN_PORT, LED_PIN_LED_GREEN_PIN);break;
        default:break;
    }
}

void led_up(void)
{
    led_drive(0, LED_PORT, LED_PIN_LED_PIN);
    delay_ms(50);
    led_drive(1, LED_PORT, LED_PIN_LED_PIN);
}

//void led_blue_up(void)
//{
//    led_drive(1, LED_PIN_LED_BLUE_PORT, LED_PIN_LED_BLUE_PIN);
//    delay_ms(200);
//    led_drive(0, LED_PIN_LED_BLUE_PORT, LED_PIN_LED_BLUE_PIN);
//}

//void led_red_up(void)
//{
//    led_drive(1, LED_PIN_LED_RED_PORT, LED_PIN_LED_RED_PIN);
//    delay_ms(200);
//    led_drive(0, LED_PIN_LED_RED_PORT, LED_PIN_LED_RED_PIN);
//}

//void led_green_up(void)
//{
//    led_drive(1, LED_PIN_LED_GREEN_PORT, LED_PIN_LED_GREEN_PIN);
//    delay_ms(200);
//    led_drive(0, LED_PIN_LED_GREEN_PORT, LED_PIN_LED_GREEN_PIN);
//}
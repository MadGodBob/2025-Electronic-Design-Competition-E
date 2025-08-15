#include "headfile.h"
#include "key.h"

#define key0 DL_GPIO_readPins(KEY_KEY_0_PORT, KEY_KEY_0_PIN) ? 0 : 1
#define key1 DL_GPIO_readPins(KEY_KEY_1_PORT, KEY_KEY_1_PIN) ? 0 : 1
#define key2 DL_GPIO_readPins(KEY_KEY_2_PORT, KEY_KEY_2_PIN) ? 0 : 1
#define key3 DL_GPIO_readPins(KEY_KEY_3_PORT, KEY_KEY_3_PIN) ? 0 : 1

char keys[4];
volatile int last_key_state = 0;

// 获取按键状态。按键优先级0>1>2>3，即优先级高的按键按下，其以后的按键不处理
// 基础按键个数为4，再次基础上增加按键长按功能，其返回值为5。长按时间取决于主程序执行频率
// 任何一个按键长按都可触发
int readKeys(void)
{
    uint8_t pressed = 0, result=0;
    static int count=0;
    keys[0] = key0;
    keys[1] = key1;
    keys[2] = key2;
    keys[3] = key3;
    for(u8 i=0;i<4;i++){
        if(keys[i]) {
            pressed = 1;
            result = i+1;
            break;
        }
    }
    if(pressed) {count++;}
    else {count=0;}
    if(count==1) {return result;}
    if(count > 20) {return 5;}
    return 0;
}

void displayKeys(void)
{
    for(u8 i=0;i<4;i++)
    {
        display_6_8_string(i*8 + 80, 0, (keys[i])?"1":"0");
    }
}
#include "headfile.h"
#include "key.h"

#define key0 DL_GPIO_readPins(KEY_KEY_0_PORT, KEY_KEY_0_PIN) ? 0 : 1
#define key1 DL_GPIO_readPins(KEY_KEY_1_PORT, KEY_KEY_1_PIN) ? 0 : 1
#define key2 DL_GPIO_readPins(KEY_KEY_2_PORT, KEY_KEY_2_PIN) ? 0 : 1
#define key3 DL_GPIO_readPins(KEY_KEY_3_PORT, KEY_KEY_3_PIN) ? 0 : 1

char keys[4];
volatile int last_key_state = 0;

// ��ȡ����״̬���������ȼ�0>1>2>3�������ȼ��ߵİ������£����Ժ�İ���������
// ������������Ϊ4���ٴλ��������Ӱ����������ܣ��䷵��ֵΪ5������ʱ��ȡ����������ִ��Ƶ��
// �κ�һ�������������ɴ���
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
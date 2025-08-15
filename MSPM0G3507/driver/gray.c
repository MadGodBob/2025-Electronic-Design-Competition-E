#include "gray.h"

uint8_t gray_data[GRAY_NUM] = {0};

// 灰度传感器数据获取
void get_gray(void)
{
	gray_data[0] = gray1;
    gray_data[1] = gray2;
    gray_data[2] = gray3;
    gray_data[3] = gray4;
    gray_data[4] = gray5;
    gray_data[5] = gray6;
    gray_data[6] = gray7;
    gray_data[7] = gray8;
    gray_data[8] = gray9;
    gray_data[9] = gray10;
    gray_data[10] = gray11;
    gray_data[11] = gray12;
}

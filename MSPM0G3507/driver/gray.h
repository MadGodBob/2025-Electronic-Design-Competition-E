#ifndef __GRAY_H__
#define __GRAY_H__

#include "ti_msp_dl_config.h"

#define GRAY_NUM 12

#define gray1  DL_GPIO_readPins(GRAY_GRAY_1_PORT, GRAY_GRAY_1_PIN) ? 0 : 1
#define gray2  DL_GPIO_readPins(GRAY_GRAY_2_PORT, GRAY_GRAY_2_PIN) ? 0 : 1
#define gray3  DL_GPIO_readPins(GRAY_GRAY_3_PORT, GRAY_GRAY_3_PIN) ? 0 : 1
#define gray4  DL_GPIO_readPins(GRAY_GRAY_4_PORT, GRAY_GRAY_4_PIN) ? 0 : 1
#define gray5  DL_GPIO_readPins(GRAY_GRAY_5_PORT, GRAY_GRAY_5_PIN) ? 0 : 1
#define gray6  DL_GPIO_readPins(GRAY_GRAY_6_PORT, GRAY_GRAY_6_PIN) ? 0 : 1
#define gray7  DL_GPIO_readPins(GRAY_GRAY_7_PORT, GRAY_GRAY_7_PIN) ? 0 : 1
#define gray8  DL_GPIO_readPins(GRAY_GRAY_8_PORT, GRAY_GRAY_8_PIN) ? 0 : 1
#define gray9  DL_GPIO_readPins(GRAY_GRAY_9_PORT, GRAY_GRAY_9_PIN) ? 0 : 1
#define gray10 DL_GPIO_readPins(GRAY_GRAY_10_PORT, GRAY_GRAY_10_PIN) ? 0 : 1
#define gray11 DL_GPIO_readPins(GRAY_GRAY_11_PORT, GRAY_GRAY_11_PIN) ? 0 : 1
#define gray12 DL_GPIO_readPins(GRAY_GRAY_12_PORT, GRAY_GRAY_12_PIN) ? 0 : 1

void get_gray(void);
extern uint8_t gray_data[GRAY_NUM];

#endif

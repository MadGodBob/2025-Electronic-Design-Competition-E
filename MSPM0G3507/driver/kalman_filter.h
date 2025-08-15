#ifndef __KALMAN_FILTER_H
#define __KALMAN_FILTER_H

#include "headfile.h"

/**
 * @brief 卡尔曼滤波结构体
 */
typedef struct {
    float Q;    // 过程噪声协方差
    float R;    // 测量噪声协方差
    float P;    // 估计误差协方差
    float K;    // 卡尔曼增益
    float X;    // 上一时刻的最优估计值
} KalmanFilter;

void Kalman_Init(KalmanFilter* kf, float Q, float R);
float Kalman_Update(KalmanFilter* kf, float measurement);

#endif
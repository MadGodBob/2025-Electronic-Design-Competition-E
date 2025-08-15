#ifndef __KALMAN_FILTER_H
#define __KALMAN_FILTER_H

#include "headfile.h"

/**
 * @brief �������˲��ṹ��
 */
typedef struct {
    float Q;    // ��������Э����
    float R;    // ��������Э����
    float P;    // �������Э����
    float K;    // ����������
    float X;    // ��һʱ�̵����Ź���ֵ
} KalmanFilter;

void Kalman_Init(KalmanFilter* kf, float Q, float R);
float Kalman_Update(KalmanFilter* kf, float measurement);

#endif
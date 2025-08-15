/**********************************************************
***	�������˲���
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include "kalman_filter.h"

/**
 * @brief ��ʼ���������˲���
 * @param kf �������˲���ָ��
 * @param Q ��������Э����
 * @param R ��������Э����
 * @param initial_value ��ʼ����ֵ
 */
void Kalman_Init(KalmanFilter* kf, float Q, float R) {
    kf->Q = Q;      // ��С�� Q ��ʾ�������ģ��Ԥ�⣬�˲�������ƽ��
    kf->R = R;      // ����Ϊ�ӽ���Ĳ�������ֵ
    kf->P = 1.0f;   // ��ʼ�������Э���������Ϊ1�����ʵ���������
    kf->K = 0.0f;
    kf->X = 0.0f;
}

/**
 * @brief �������˲�����
 * @param kf �������˲���ָ��
 * @param measurement ����ֵ
 * @return �˲���Ĺ���ֵ
 */
float Kalman_Update(KalmanFilter* kf, float measurement) {
    if(kf->X == 0.0f) {
        kf->X = measurement;
        return measurement;
    }
    // Ԥ�ⲽ��
    // �������ϵͳģ���Ǻ㶨�ģ�����Ԥ��ֵ����
    // X_k = X_k-1
    // P_k = P_k-1 + Q
    kf->P = kf->P + kf->Q;
    
    // ���²���
    // ���㿨��������
    // K = P_k / (P_k + R)
    kf->K = kf->P / (kf->P + kf->R);
    
    // ���¹���ֵ
    // X_k = X_k + K * (measurement - X_k)
    kf->X = kf->X + kf->K * (measurement - kf->X);
    
    // ���¹������Э����
    // P_k = (1 - K) * P_k
    kf->P = (1 - kf->K) * kf->P;
    
    return kf->X;
}
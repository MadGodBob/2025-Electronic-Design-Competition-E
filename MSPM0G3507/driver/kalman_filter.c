/**********************************************************
***	卡尔曼滤波器
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include "kalman_filter.h"

/**
 * @brief 初始化卡尔曼滤波器
 * @param kf 卡尔曼滤波器指针
 * @param Q 过程噪声协方差
 * @param R 测量噪声协方差
 * @param initial_value 初始估计值
 */
void Kalman_Init(KalmanFilter* kf, float Q, float R) {
    kf->Q = Q;      // 较小的 Q 表示你更信任模型预测，滤波结果会更平滑
    kf->R = R;      // 设置为接近你的测量误差的值
    kf->P = 1.0f;   // 初始估计误差协方差，可以设为1或根据实际情况调整
    kf->K = 0.0f;
    kf->X = 0.0f;
}

/**
 * @brief 卡尔曼滤波函数
 * @param kf 卡尔曼滤波器指针
 * @param measurement 测量值
 * @return 滤波后的估计值
 */
float Kalman_Update(KalmanFilter* kf, float measurement) {
    if(kf->X == 0.0f) {
        kf->X = measurement;
        return measurement;
    }
    // 预测步骤
    // 这里假设系统模型是恒定的，所以预测值不变
    // X_k = X_k-1
    // P_k = P_k-1 + Q
    kf->P = kf->P + kf->Q;
    
    // 更新步骤
    // 计算卡尔曼增益
    // K = P_k / (P_k + R)
    kf->K = kf->P / (kf->P + kf->R);
    
    // 更新估计值
    // X_k = X_k + K * (measurement - X_k)
    kf->X = kf->X + kf->K * (measurement - kf->X);
    
    // 更新估计误差协方差
    // P_k = (1 - K) * P_k
    kf->P = (1 - kf->K) * kf->P;
    
    return kf->X;
}
/**********************************************************
***	灰传数据处理例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include "headfile.h"
#include "gray_process.h"

#define gray_amount 12              // 灰传数量12
#define turn_min_gray_amount 4      // 判定小车进行直角转弯时灰传探测到的最小数量
#define turn_maintain_length 3      // 小车退出直角转弯时扫到的线在距离中心n个长度以内
float turn_left_complete_count = 300;    // 小车完成转弯时走过的里程计读数
float line_follow_config[gray_amount] = {0.0};        // 巡线参数
uint8_t gray_init=0;
TurnType turn_type=straight;
int turn_left_start_count=0;

void set_line_follow_config(float *config)
{
    gray_init = 1;
    for(int i=0;i<gray_amount;i++)
    {
        line_follow_config[i] = config[i];
    }
}

// 输入当前的灰传结果，判断是否要维持一段时间的直角转弯还是退出直角转弯
void turn_maintain(float output)
{
    bool strong_ = false;
    if(turn_type != left) return ;
    // 检测小车灰传是否回正
    if(output < line_follow_config[gray_amount/2+turn_maintain_length-1]){
        strong_ = true; // 强制变成直行状态
    }
//    if(output > line_follow_config[gray_amount/2-turn_maintain_length]){
//        return ;
//    }
    // 若编码器长度未到达指定长度 且 strong_=false，维持转弯状态
    if(encoder_count - turn_left_start_count < turn_left_complete_count && !strong_){
        return ;
    }
    // 条件满足，进入直行状态
    turn_type = straight;
    
    // 先强制直行，防止转弯过渡
    motor_speed(turn_speed, turn_speed);
    clear_last_output();
    single_motor_pid(&motor_l);
    single_motor_pid(&motor_r);
    Set_duty(motor_l.pwmDuty, 0);
    Set_duty(motor_r.pwmDuty, 1);
    
    Beep_up();
}

// 通过灰传捕获的数据，输出小车目前应该转的角度
float last_output = 0.0;
float get_gray_output(void)
{
    uint8_t gray_count=0;
    float output_sum=0;
    if(!gray_init) return 0.0;         // 未初始化参数则不输出结果
    for(uint8_t i=0;i<gray_amount;i++)
    {
        if(gray_data[i] == 0)
        {
            gray_count++;
            output_sum += line_follow_config[i];
        }
    }
    output_sum /= gray_count;
    
    if(gray_count>=turn_min_gray_amount && turn_type!=left){    // 进入直角转弯
        if(output_sum > (line_follow_config[5]+line_follow_config[6])/2){
            turn_left_start_count = encoder_count;
            turn_type = left;
        }
    }
    else if(gray_count>0){          // 直线巡线
        turn_maintain(output_sum);
        last_output = output_sum;
        return output_sum;
    }
    else{       // 没有回传输出，可能脱离巡线或正在转弯，输出上一次的结果
        if(turn_type == left) {last_output = line_follow_config[0];}
        turn_maintain(last_output);
        return last_output;
    }
}

void clear_last_output(void)
{
    last_output = 0.0;
}

void set_turn_left_complete_count(int c)
{
    turn_left_complete_count = c;
}

void set_turn_type(TurnType t)
{
    turn_type = t;
}
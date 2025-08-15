/**********************************************************
***	�Ҵ����ݴ�������
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include "headfile.h"
#include "gray_process.h"

#define gray_amount 12              // �Ҵ�����12
#define turn_min_gray_amount 4      // �ж�С������ֱ��ת��ʱ�Ҵ�̽�⵽����С����
#define turn_maintain_length 3      // С���˳�ֱ��ת��ʱɨ�������ھ�������n����������
float turn_left_complete_count = 300;    // С�����ת��ʱ�߹�����̼ƶ���
float line_follow_config[gray_amount] = {0.0};        // Ѳ�߲���
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

// ���뵱ǰ�ĻҴ�������ж��Ƿ�Ҫά��һ��ʱ���ֱ��ת�仹���˳�ֱ��ת��
void turn_maintain(float output)
{
    bool strong_ = false;
    if(turn_type != left) return ;
    // ���С���Ҵ��Ƿ����
    if(output < line_follow_config[gray_amount/2+turn_maintain_length-1]){
        strong_ = true; // ǿ�Ʊ��ֱ��״̬
    }
//    if(output > line_follow_config[gray_amount/2-turn_maintain_length]){
//        return ;
//    }
    // ������������δ����ָ������ �� strong_=false��ά��ת��״̬
    if(encoder_count - turn_left_start_count < turn_left_complete_count && !strong_){
        return ;
    }
    // �������㣬����ֱ��״̬
    turn_type = straight;
    
    // ��ǿ��ֱ�У���ֹת�����
    motor_speed(turn_speed, turn_speed);
    clear_last_output();
    single_motor_pid(&motor_l);
    single_motor_pid(&motor_r);
    Set_duty(motor_l.pwmDuty, 0);
    Set_duty(motor_r.pwmDuty, 1);
    
    Beep_up();
}

// ͨ���Ҵ���������ݣ����С��ĿǰӦ��ת�ĽǶ�
float last_output = 0.0;
float get_gray_output(void)
{
    uint8_t gray_count=0;
    float output_sum=0;
    if(!gray_init) return 0.0;         // δ��ʼ��������������
    for(uint8_t i=0;i<gray_amount;i++)
    {
        if(gray_data[i] == 0)
        {
            gray_count++;
            output_sum += line_follow_config[i];
        }
    }
    output_sum /= gray_count;
    
    if(gray_count>=turn_min_gray_amount && turn_type!=left){    // ����ֱ��ת��
        if(output_sum > (line_follow_config[5]+line_follow_config[6])/2){
            turn_left_start_count = encoder_count;
            turn_type = left;
        }
    }
    else if(gray_count>0){          // ֱ��Ѳ��
        turn_maintain(output_sum);
        last_output = output_sum;
        return output_sum;
    }
    else{       // û�лش��������������Ѳ�߻�����ת�䣬�����һ�εĽ��
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
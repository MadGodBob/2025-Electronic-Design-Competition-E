/**********************************************************
***	2025�����E�⿪Դ����
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include "motor.h"

#define MOTOR_SPEED_RERATIO 28    //������ٱ�
#define PULSE_PRE_ROUND 13       //ÿȦ������
#define ENCODER_MULTIPLE 1.0     //��������Ƶ
#define PULSE_PER_CYCLE  (MOTOR_SPEED_RERATIO*PULSE_PRE_ROUND*ENCODER_MULTIPLE)//ÿȦ������
#define RADIUS_OF_TYRE 3.3  //����ֱ�� cm
#define LINE_SPEED_C  RADIUS_OF_TYRE * 2 * 3.14  //�����ܳ� cm
#define MAXSPEED 120.0        //������ռ�ձ�ʱ������ٶ�

float PID[3] = {0.3, 0.005, 0.8};
motor_t motor_l, motor_r;
uint32_t period;

// �����ʼ��
void motor_init(void)
{
    Set_Freq(1000);
    DL_Timer_startCounter(MOTOR_INST);     // PWM��ʼ��
    NVIC_EnableIRQ(ENCODER1A_INST_INT_IRQN);
    DL_Timer_startCounter(ENCODER1A_INST);
    NVIC_EnableIRQ(ENCODER2A_INST_INT_IRQN);
    DL_Timer_startCounter(ENCODER2A_INST); // ʹ�ܱ��������벶���ж�
    NVIC_EnableIRQ(CLOCK_INST_INT_IRQN);
    DL_Timer_startCounter(CLOCK_INST);     // ʹ��ʱ�Ӷ�ʱ���ж�
    motor_stop();
    motor_speed(0.0, 0.0);
    motor_l.encoder.countnum = 0;
    motor_r.encoder.countnum = 0;
    motor_l.pwmDuty = 0;
    motor_r.pwmDuty = 0;
    
    motor_l.error = 0;
    motor_l.last_error = 0;
    motor_l.integral = 0;
    motor_r.error = 0;
    motor_r.last_error = 0;
    motor_r.integral = 0;
}

// ����PWMƵ��
void Set_Freq(uint32_t freq)
{
	period = MOTOR_INST_CLK_FREQ/freq;
    DL_TimerA_setLoadValue(MOTOR_INST, period);
}

// ���PID�ۻ���I��
void motor_clear_i_d(void)
{
    motor_l.integral = 0;
    motor_r.integral = 0;
}

// ����PWMռ�ձ�
void Set_duty(float duty,uint8_t channel)
{
    uint32_t CompareValue;
    CompareValue = period*duty;

    if(channel == 0)
        {DL_TimerA_setCaptureCompareValue(MOTOR_INST, CompareValue, GPIO_MOTOR_C0_IDX);}
    else if (channel == 1)
        {DL_TimerA_setCaptureCompareValue(MOTOR_INST, CompareValue, GPIO_MOTOR_C1_IDX);}
}

// ��������״̬��ǰ�������ˣ�
void change_motor_mode(uint8_t mode1, uint8_t mode2)
{

    if(mode1 == 1)
        {motor_l.dir = 1;DL_GPIO_setPins(PORTB_PORT, PORTB_AIN1_PIN);DL_GPIO_clearPins(PORTB_PORT, PORTB_AIN2_PIN);}
    else
        {motor_l.dir = -1;DL_GPIO_setPins(PORTB_PORT, PORTB_AIN2_PIN);DL_GPIO_clearPins(PORTB_PORT, PORTB_AIN1_PIN);}
        
    if(mode2 == 1)
        {motor_r.dir = 1;DL_GPIO_setPins(PORTB_PORT, PORTB_BIN1_PIN);DL_GPIO_clearPins(PORTB_PORT, PORTB_BIN2_PIN);}
    else
        {motor_r.dir = -1;DL_GPIO_setPins(PORTB_PORT, PORTB_BIN2_PIN);DL_GPIO_clearPins(PORTB_PORT, PORTB_BIN1_PIN);}
}

// ���������ת��
void motor_speed(float speed1, float speed2)
{
    change_motor_mode(speed1>=0.0, speed2>=0.0);
    speed1 = fabs(speed1);
    speed2 = fabs(speed2);
    speed1 /= (1000.0 / MAXSPEED);
    speed2 /= (1000.0 / MAXSPEED);
    
    
    motor_l.encoder_speed = speed1;
    motor_r.encoder_speed = speed2;
    
}

// ���������PID����
void single_motor_pid(motor_t *motor)
{
    if(motor -> encoder_speed == 0.0)  {motor -> pwmDuty = 0;return;}
    motor -> error = motor -> encoder_speed - motor -> encoder.speed;
    motor -> integral += motor -> error;
    motor -> pwmDuty += (motor -> error * PID[0] + motor -> integral * PID[1] + (motor -> error - motor -> last_error) * PID[2]) / MAXSPEED;
    float maxPwm = motor -> encoder_speed / MAXSPEED + 0.05 > 1.0 ? 1.0 : motor -> encoder_speed / MAXSPEED + 0.05;
    motor -> pwmDuty = motor -> pwmDuty > maxPwm ? maxPwm : motor -> pwmDuty;
    motor -> last_error = motor -> error;
}

// �ƶ�
void motor_stop(void)
{
    DL_GPIO_setPins(PORTB_PORT, PORTB_AIN2_PIN);
    DL_GPIO_setPins(PORTB_PORT, PORTB_AIN1_PIN);
    DL_GPIO_setPins(PORTB_PORT, PORTB_BIN1_PIN);
    DL_GPIO_setPins(PORTB_PORT, PORTB_BIN2_PIN);
}

// ʧ��
void motor_disable(void)
{
    ;
}


/*�����ı���������*/
void ENCODER1A_INST_IRQHandler(void) {
    switch (DL_Timer_getPendingInterrupt(ENCODER1A_INST)) {
        case DL_TIMERG_IIDX_CC0_DN:
            motor_l.encoder.direct = DL_GPIO_readPins(GPIO_ENCODER_ENCODER1B_PORT, GPIO_ENCODER_ENCODER1B_PIN);//��ȡIO��ƽ��ȡ�����ת����
//            motor_l.encoder.countnum = motor_l.encoder.direct? (motor_l.encoder.countnum + 1) : (motor_l.encoder.countnum - 1);//ͨ���ж���ת����������countnum���ӻ��Ǽ���
            motor_l.encoder.countnum++;
            break;
        default:
            break;
  }
}

 
/*�ҵ���ı���������*/
void ENCODER2A_INST_IRQHandler(void) {
    switch (DL_Timer_getPendingInterrupt(ENCODER2A_INST)) {
        case DL_TIMERG_IIDX_CC0_DN:
            motor_r.encoder.direct = DL_GPIO_readPins(GPIO_ENCODER_ENCODER2B_PORT, GPIO_ENCODER_ENCODER2B_PIN);//��ȡIO��ƽ��ȡ�����ת����
//            motor_r.encoder.countnum = motor_r.encoder.direct ? ( motor_r.encoder.countnum + 1) : ( motor_r.encoder.countnum- 1);//ͨ���ж���ת����������countnum���ӻ��Ǽ���
            motor_r.encoder.countnum++;
            break;
        default:
            break;
  }
}
 
/*period 100Hz*/
int global_count = 0;
void CLOCK_INST_IRQHandler(void){
    float Speed_Low_Filter(float new_Spe,encoder_t *encoder);
    switch (DL_Timer_getPendingInterrupt(CLOCK_INST)) 
    {
        case DL_TIMER_IIDX_ZERO:
            DL_Timer_clearInterruptStatus(CLOCK_INST,DL_TIMER_IIDX_ZERO);
            motor_l.encoder.speed=(float)(motor_l.encoder.countnum-motor_l.encoder.lastcount)*6000/PULSE_PER_CYCLE * LINE_SPEED_C/60;
            motor_r.encoder.speed=(float)(motor_r.encoder.countnum-motor_r.encoder.lastcount)*6000/PULSE_PER_CYCLE * LINE_SPEED_C/60;  // cm/s
            motor_l.encoder.speed=Speed_Low_Filter(motor_l.encoder.speed,&motor_l.encoder);
            motor_r.encoder.speed=Speed_Low_Filter(motor_r.encoder.speed,&motor_r.encoder);
            motor_l.encoder.lastcount=motor_l.encoder.countnum;
            motor_r.encoder.lastcount=motor_r.encoder.countnum;
        
            global_count++;
    }
}

// �����ݽ��е�ͨ�˲�
float Speed_Low_Filter(float new_Spe,encoder_t *encoder)
{
    float sum = 0.0f;
    uint32_t test_Speed = new_Spe;
    for(uint8_t i=SPEED_RECORD_NUM-1;i>0;i--)
    {
        encoder->speed_Record[i] = encoder->speed_Record[i-1];
        sum += encoder->speed_Record[i-1];
    }
    encoder->speed_Record[0] = new_Spe;
    sum += new_Spe;
    test_Speed = sum/SPEED_RECORD_NUM;
    return sum/SPEED_RECORD_NUM;
}

// �����ٶȡ�ת��Ƕȣ�������ʵ��ת��Ƕȣ���PID�����ĵ������
volatile float last_angle = 0.0;
void racecar(float speed, float angle, float k, float turn_D)
{
//    float k=speed/5.0;
    float value = angle * k - turn_D * (angle - last_angle);
        
    motor_speed(speed - value, speed + value);
    last_angle = angle;
}


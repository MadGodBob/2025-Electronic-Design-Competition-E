/**********************************************************
***	���������߼�����
***	B վ���ߣ�-���η���-
***	B վ��ҳ��https://space.bilibili.com/525044038
***	��ʾ��Ƶ��https://www.bilibili.com/video/BV1Gutcz5EaG
***	��������ȫ�̿�Դ������ӯ��
**********************************************************/
#include "headfile.h"
#include "task.h"

// Ĭ�ϲ���
#define task_amount 9       // 6����Ŀ����+�������һ�궨+��������+�������ַ���
#define max_circles 5       // ��ʻȦ����N���Ϊ5
#define circle_min_encoder_count 6525   // ��ʻһȦ���߹�����ͱ���������(��ͬӲ������ֵ��ͬ)
// �ɱ����
float PID_[3] = {40, 0.0, 2};       // С��Ѳ��pid
float track_PID_1[3] = {0.9, 0, 1.5};   // ����׷��pid 1(ˮƽ�ߵ����PID)
float track_PID_2[3] = {7, 0.0, 2};   // ����׷��pid 2(��ֱ�ߵ����PID)
int default_laser_point[2] = {388, 307};    // �����������е�����
int ellipse[2] = {0};       // ��Բ�ĳ���Ͷ��ᣨx��y��
float inner_circle_radius = 6;          // �жϼ�����Ƿ�����Բ�����ذ뾶
// Ѳ�߲����������
float gray_config_1[12] = {1.5, 1.0, 0.8, 0.5, 0.2, 0.1, -0.1, -0.2, -0.5, -0.8, -1.0, -1.5};   // ����200

// ��ʼ������
u8 task_id=-1;              // ����ID
u8 target_cirlces=1;        // Ŀ��Ȧ��N
u8 circle=0;                // ����ʻȦ��
u8 start_flag=0;            // ��ʼ��־
float turn_theta = 5;      // ֱ��ת��ʱ��ת��Ƕ�
float turn_speed = 200;     // ֱ��ת��ʱ���ٶ�
float target_speed = 200;   // �����PID��Ŀ���ٶ�
float encoder_count=0.0;    // ����������
float turn_angle=0.0;       // �����PID��Ŀ��Ƕ�
float angle_to_imu_zero=0.0;    // �����ǹ��㷽����y�Ḻ����ļнǣ���ʱ��Ϊ��
float roll, pitch, yaw;     // �����Ƕ���
int circle_count_start=0;   // ����Ȧ������ʼ����������
bool final_start_flag = false;    // ��ǰ��ʼ��־������������̨���
bool stepper_motor_2_ban = false;   // ���ò��������
bool set_laser_point_flag = false;  // ���ü����־λ
bool laser_flag = false;       // ����򿪱�־

// �����ʼ��
void task_init(void)
{
    servo_pid_init(track_PID_1, track_PID_2);   // ���������ʼ��
    set_laser_point(default_laser_point);       // ���ü��������
    change_task_init();                         // ����ı�ʱִ��һ��
    Beep_up();
}
                         
// ����������(ѭ��ִ��)
void task_main(void)
{
    set_encoder_count();    // ���±���������
    circle_detect();        // ����Ƿ�����һȦ
    change_task();          // ͨ�������ж��Ƿ�ı�����
    display_task();         // ��ʾ���������Ϣ
    set_target_point(target_point); // ��OPENMV�����ľ������ĵ�������Ϊ��Ŀ���
    activate_task();        // ���񼤻��(�������߼�)
    
//    // �����ǲ���
//    stepper_motor_control_vel(10, 10);
    
//    static float ii=0;
//    ii += 0.1;
//    stepper_motor_control(ii, 0);
//    stepper_motor_control(180, 20);
//    
//    stepper_motor_control(0, 0);    // 12800
    
//    Serial_JY901S_Zero_Yaw();
//    getAngle(&roll, &pitch, &yaw);
//    stepper_motor_control(yaw, 0);
//    stepper_motor_control(0, 50);
}

// �л�����ʱ��Ҫ��ʼ��һЩ����
void change_task_init(void)
{
    start_flag=0;
    circle=0;
    circle_count_start=encoder_count;
    final_start_flag = false;
    // �������������
    motor_l.encoder.countnum = 0;
    motor_r.encoder.countnum = 0;
    // ����ر� ��ֹ�������������
    laser_off();
    set_laser_point_flag = false;
    // ��ʼ��Ѳ�߲���
    switch(task_id)
    {
        case 0: set_line_follow_config(gray_config_1);break;
        case 3: set_line_follow_config(gray_config_1);set_turn_type(left);
        case 4: set_line_follow_config(gray_config_1);set_turn_type(left);
        case 5: set_current_point(B);init_relative_l(90.0);break;    // ��λģ�����
    }
    
}

// ͨ����ť�л�����
/**
    ��ť1�л�����ID
    ��ť2�л�Ŀ��Ȧ��N��N=2
    ��ť3ִ�������ǹ��㡢��������궨���λ��
    ��ť4��ʼִ������

    ����1-3ʱ�����°�ť4��ֱ��ִ������
    ����4-6ʱ�����°�ť4�󣬼����Ƚ��д�У��ȶ����°�ť2��ִ��С��Ѳ��
**/
void change_task(void)
{
    uint8_t keyState = readKeys();
    static float key3_count = 0;
    switch(keyState)
    {
        case 1: task_id++;task_id%=task_amount;change_task_init();break;
        case 2: target_cirlces%=max_circles;target_cirlces++;final_start_flag=true;break;
        case 3: cali_yaw();stepper_motor_set_zero(true);break;     // �궨��̨ stepper_motor_set_zero(true);
        case 4: start_flag=1;target_point[0]=0;target_point[1]=0;break;
        case 5: DL_SYSCTL_resetDevice(1);break;
    }
    if(target_cirlces == 4) {laser_on();}
    if(target_cirlces == 5) {laser_off();}
//    if(key3_count > 1) {DL_SYSCTL_resetDevice(1);}
}

// ==================================�����߼�==================================
// ��������
typedef void (*task_func_t)(void);
task_func_t task_funcs[] = {
    task1,  // task_id = 0
    task2,  // task_id = 1
    task3,  // task_id = 2
    task4,  // task_id = 3
    task5,  // task_id = 4
    task6,  // task_id = 5
    task7,  // �궨�������1
    task8,  // �궨��������
    task9   // ������������
};
void activate_task(void)
{
    if(!start_flag ||  task_id == 0xFF) {
        motor_speed(0.0, 0.0);
        return ;
    }
    task_funcs[task_id]();
}

// ===========����1=============
void task1(void)
{    
    target_speed = 220;
    turn_speed = 100;
    turn_theta = 0.8;      // ����2.45
    set_turn_left_complete_count(280);  // ת�����ʱ�߹��ı���������390
    PID_[0] = 50;
    
    turn_angle = get_gray_output();
    car_drive();
    
    // С��ֹͣ�߼�
    if(circle < target_cirlces) {return ;}
    start_flag = 0;
}

// ===========����2=============
void task2(void)
{
    laser_flag = false;
    // ���⿪�ؿ���
    #define laser_on_min_count 5    // �򿪼���ʱ���������С�Ĵ���
    static uint8_t laser_count = 0;     // �������Ŷ�
    float error_ = get_point_error();
    if(error_ < pow(inner_circle_radius, 2)) {laser_count++;}
    else {laser_count = 0;}
    if(laser_count > laser_on_min_count-1) {laser_on();}
    // ��̨���
    stepper_motor_control_pid(0.0);
}

// ===========����3=============
void task3(void)
{
    laser_flag = false;
    static bool pid_init = false;
    if(!pid_init)
    {
        // pid
        track_PID_1[0] = 0.6;
        track_PID_1[2] = 1.0;
        set_pid1(track_PID_1);
        pid_init = true;
    }
    
    // ���⿪�ؿ���
    #define laser_on_min_count 5    // �򿪼���ʱ���������С�Ĵ���
    static uint8_t laser_count = 0;     // �������Ŷ�
    float error_ = get_point_error();
    if(error_ < pow(inner_circle_radius, 2)) {laser_count++;}
    else {laser_count = 0;}
    if(laser_count > laser_on_min_count-1) {laser_on();}
    // ѭ����תѰ�ұ��
    static float bias_theta = 0.0;
    static int turn_count_ = 0;
    int step = 70/1.2;
    if(target_point[0] == 0 && target_point[1] == 0)
    {   // û��ʶ��
        stepper_motor_rpm = 3000;
        stepper_motor_a = 250;
        bias_theta = ((target_cirlces == 1) ? 1 : -1) * 60 * (int)(turn_count_/step+1);       // N=1����ת��������ת
        turn_count_++;
        if(turn_count_/step > 8) {turn_count_ = 8;}
    }
    else{
        stepper_motor_rpm = 5000;
        stepper_motor_a = 256;
    }
    stepper_motor_control_pid(bias_theta);
}

// ===========����4=============
void task4(void)
{
    float start_turn_theta = 0.9;
    static bool start_turn_flag = false;
    // ��ǰ��е�pid
    track_PID_1[0] = 0.6;
    track_PID_1[2] = 1.0;
    
    if(final_start_flag){
        // ��ʻ�е�pid
        track_PID_1[0] = 2.0;
        track_PID_1[2] = 1.8;
        // ת��ʱ������һ��pid��ͬʱC��D����Զ�㣬��Ҫ����
        if(turn_type == left){
            track_PID_1[0] = 0.9;
            switch(current_point)
            {
                case A: track_PID_1[0] += 0.2;break;
                case B: track_PID_1[0] += 0.2;break;
                case C: track_PID_1[0] += 0.2;break;
                case D: track_PID_1[0] += 0.2;break;
            }
        }
        // ���ݵ�ǰ�����߶��ʵ�����p
        switch(current_point)
        {
            case C: track_PID_1[0] = 2.2;break;
        }
        // С����ʻ
        float target_speed_ = 280;
        float target_speed_up_k = 1.8;      // ʹ���ٳ������Σ����ٿ���������м����٣���ʱ�����ν���Ϊtarget_speed_up_k*target_speed_
        turn_speed = 100;
        // ��̬�����ٶȣ����������м��ٶȿ죬������
        static float instant_relative_l = 0.0;
        if(turn_type == left) {instant_relative_l = relative_l;}        // ֻ����С�����ת���Ž��������ٶȵ���
        float bias_relative_l = relative_l - instant_relative_l;        // ת��ʱbiasΪ0
        #define square_side_ 70         // С�����ת�������һ�����Լ70cm
        target_speed = -(target_speed_ * target_speed_up_k - turn_speed) * fabs(bias_relative_l - square_side_/2.0) / (square_side_/2.0) + 
                        target_speed_ * target_speed_up_k;
        // �������ֵ����Сֵ
        target_speed = MIN(MAX(target_speed, turn_speed), target_speed_);
        
        turn_theta = !start_turn_flag ? start_turn_theta : 0.8;      // ����2.45
        set_turn_left_complete_count(250);  // ת�����ʱ�߹��ı���������
        PID_[0] = 50;
        
//        display_6_8_number(50, 3, target_speed);
        
        turn_angle = get_gray_output();
        car_drive();
        
        if(turn_type == straight) {start_turn_flag = true;}
        
        stepper_motor_rpm = 5000;
        stepper_motor_a = 256;
    }
    else{
        stepper_motor_rpm = 1000;
        stepper_motor_a = 230;
    }
    
    // ��̨
    set_pid1(track_PID_1);
    angle_to_imu_zero = get_angle();
    Serial_JY901S_Zero_Yaw();
    getAngle(&roll, &pitch, &yaw);
//    static float last_yaw = 0.0;
    
//    float bias_yaw = fabs(yaw - last_yaw);
//    last_yaw = yaw;
//    add_servo_angle(bias_yaw);
//    stepper_motor_control_vel(last_yaw*2, 0.0);
    // �������������ݷ���
    debounce_flag = true;
    stepper_motor_control_pid(yaw + angle_to_imu_zero);
    // ����
    set_laser_point_flag = true;
    if(!laser_flag){
        #define laser_on_min_count 5    // �򿪼���ʱ���������С�Ĵ���
        static uint8_t laser_count = 0;     // �������Ŷ�
        float error_ = get_point_error();
        if(error_ < pow(inner_circle_radius, 2)) {laser_count++;}
        else {laser_count = 0;}
        if(laser_count > laser_on_min_count-1) {laser_on();}
    }   
        
    // С��ֹͣ�߼�
    if(task_id == 3 || task_id == 5) {target_cirlces = 1;}
    if(task_id == 4) {target_cirlces = 2;}
    if(circle < target_cirlces) {return ;}
    laser_off();
    start_flag = 0;
}

// ===========����5=============
void task5(void)
{
    task4();
}

// ===========����6=============
void task6(void)
{
    #define PI 3.1415926535f
    #define square_side 100 * 0.95
    // ������Բ�ϵĵ�
    int ellipse_point[2] = {0};
    ellipse_point[0] = target_point[0];
    ellipse_point[1] = target_point[1];
    float theta = PI/2.0;     // ��ʼ��
    switch(current_point)       // ʹ�ñ������������ó�Ŀ��㡣��С����AC�߶��ϣ���Ŀ���ΪԲ�������ķ�֮һ��
    {
        case A: theta += (relative_l/square_side + 0)* PI/2.0;break;
        case C: theta += (relative_l/square_side + 1) * PI/2.0;break;
        case D: theta += (relative_l/square_side + 2) * PI/2.0;break;
        case B: theta += (relative_l/square_side + 3) * PI/2.0;break;
    }
    // ��ʽΪx=acos��y=bsin��
    ellipse_point[0] += ellipse[0] * cos(theta);
    ellipse_point[1] -= ellipse[1] * sin(theta);
    set_target_point(ellipse_point);
    
    task4();
}

// ===========����7 �궨�������һ=============
void task7(void)
{
    static bool cal_init = false;
    if(!cal_init) {Emm_v5_Cal(0x01, UART_2_INST);;}
    cal_init = true;
}
    
// ===========����8 ����ͨ��OPENMV���ص����ݽ��м����������=============
void task8(void){
    laser_on();
    set_laser_point_flag = true;
}

// ===========����9 �����������֣�С����ת��������=============
void task9(void){
    target_speed = 100;
    if(final_start_flag){
        laser_on();
        motor_speed(-target_speed, target_speed);  
    }
    
    // ��̨
    track_PID_1[0] = 2.0;
    track_PID_1[2] = 1.5;
    set_pid1(track_PID_1);
    Serial_JY901S_Zero_Yaw();
    getAngle(&roll, &pitch, &yaw);
    // �������������ݷ���
    debounce_flag = true;
    stepper_motor_control_pid(yaw);
    // ֹͣ�߼�
    #define ROUND(x) (int)(x + 0.5f)
    static float last_yaw = 0.0;
    // �����
    float yaw_ = yaw;
    float detla = yaw_ - last_yaw;
    int n;
    if(detla < -180)
    {
        n = ROUND(fabs(detla)/360);
        n = (n > 0) ? n : 1;
        yaw_ += 360*n;
    }
    else if(detla > 180)
    {
        n = ROUND(fabs(detla)/360);
        n = (n > 0) ? n : 1;
        yaw_ -= 360*n;
    }
    last_yaw = yaw_;
    if(yaw_ < 3*360) {return ;}         // ת��NȦ��Ϊ����
    laser_off();
    start_flag = 0;
}

// ===========������Ϣ��ʾ=============
// ͨ������OLED����ʾ���ַ�����������������߳��������ٶȣ��Ӷ���߲����������ѯ��
void display_task(void)
{
    static bool display_word_init = false;
    // ����id
    if(!display_word_init) display_6_8_string(0, 7, "task:");
    display_6_8_number(30, 7, task_id+1==256?0:task_id+1);
    // Ŀ��Ȧ��
    if(!display_word_init) display_6_8_string(50, 7, "N:");
    display_6_8_number(70, 7, target_cirlces);
//    // encoder����
//    if(!display_word_init) display_6_8_string(0, 4, "encoder:");
//    display_6_8_number(60, 4, (int)encoder_count);
//    // Ԥ���Ƕ�
//    display_6_8_string(0, 3, "angle:          ");
//    display_6_8_number(60, 3, turn_angle);
//    // ��ʻ����
//    if(!display_word_init) display_6_8_string(0, 6, "turn:");
//    display_6_8_string(60, 6, turn_type==left?"left":"line");
//    // ����ʻ��Ȧ��
//    if(!display_word_init) display_6_8_string(0, 4, "circle:");
//    display_6_8_number(60, 4, circle);
    // ����Ŀ���
    static bool target_receive = false, last_target_receive=false;
    if(!display_word_init) display_6_8_string(0, 5, "target:");
    display_6_8_number(60, 5, target_point[0]);
    display_6_8_number(90, 5, target_point[1]);
    
    static int last_default_laser_point[2] = {0};
    if(!display_word_init) display_6_8_string(0, 6, "laser:");
    if(default_laser_point[0] != last_default_laser_point[0]) {display_6_8_number(60, 6, default_laser_point[0]);
                                                        last_default_laser_point[0] = default_laser_point[0];}
    if(default_laser_point[1] != last_default_laser_point[1]) {display_6_8_number(90, 6, default_laser_point[1]);
                                                        last_default_laser_point[1] = default_laser_point[1];}
    // ����״̬
//    display_6_8_string(90, 7, start_flag?"busy":"free");
    display_word_init = true;
}
// ==================================�����߼�==================================
// author:Tianmengfanxing

// С�������߼������յ���ʼ��־start_flag������е���˶�
// ��С��״̬Ϊ��תleft�������Ϊturn_speed turn_theta
void car_drive(void)
{
    static TurnType last_turn_type = none;
    if(!start_flag)
    {
        motor_speed(0, 0);
        return ;
    }
    if(turn_type == straight){
        racecar(target_speed, turn_angle, PID_[0], PID_[2]);
    }
    else if(turn_type == left){
        racecar(turn_speed, turn_theta, PID_[0], PID_[2]);
        // ֻ�е�С�����ת�����ж�������һ����
        if(last_turn_type == straight ||last_turn_type == none) {approach_point();}
    }
    last_turn_type = turn_type;
}

// ͨ������������Ȧ���Ƿ�+1
void circle_detect(void)
{
    if(encoder_count - circle_count_start < circle_min_encoder_count){
        return ;
    }
    circle_count_start = encoder_count;
    circle++;
}

void set_encoder_count(void)
{
    encoder_count = (motor_l.encoder.countnum + motor_r.encoder.countnum)/2.0;
}

void set_default_laser_point(int *p)
{
    if(!set_laser_point_flag) return ;
    default_laser_point[0] = p[0];
    default_laser_point[1] = p[1];
    set_laser_point(default_laser_point);
}

void set_ellipse(int a, int b)
{
    ellipse[0] = a;
    ellipse[1] = b;
}
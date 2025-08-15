/**********************************************************
***	任务处理主逻辑例程
***	B 站作者：-天梦繁星-
***	B 站主页：https://space.bilibili.com/525044038
***	演示视频：https://www.bilibili.com/video/BV1Gutcz5EaG
***	所有例程全程开源，绝非盈利
**********************************************************/
#include "headfile.h"
#include "task.h"

// 默认参数
#define task_amount 9       // 6个题目任务+步进电机一标定+激光点纠正+其它部分发挥
#define max_circles 5       // 行驶圈数，N最大为5
#define circle_min_encoder_count 6525   // 行驶一圈所走过的最低编码器读数(不同硬件车该值不同)
// 可变参数
float PID_[3] = {40, 0.0, 2};       // 小车巡线pid
float track_PID_1[3] = {0.9, 0, 1.5};   // 激光追踪pid 1(水平线电机的PID)
float track_PID_2[3] = {7, 0.0, 2};   // 激光追踪pid 2(竖直线电机的PID)
int default_laser_point[2] = {388, 307};    // 激光点在相机中的坐标
int ellipse[2] = {0};       // 椭圆的长轴和短轴（x和y）
float inner_circle_radius = 6;          // 判断激光点是否在内圆的像素半径
// 巡线参数（多个）
float gray_config_1[12] = {1.5, 1.0, 0.8, 0.5, 0.2, 0.1, -0.1, -0.2, -0.5, -0.8, -1.0, -1.5};   // 低速200

// 初始化参数
u8 task_id=-1;              // 任务ID
u8 target_cirlces=1;        // 目标圈数N
u8 circle=0;                // 已行驶圈数
u8 start_flag=0;            // 开始标志
float turn_theta = 5;      // 直角转弯时的转弯角度
float turn_speed = 200;     // 直角转弯时的速度
float target_speed = 200;   // 输入进PID的目标速度
float encoder_count=0.0;    // 编码器读数
float turn_angle=0.0;       // 输入进PID的目标角度
float angle_to_imu_zero=0.0;    // 陀螺仪归零方向与y轴负方向的夹角，逆时针为正
float roll, pitch, yaw;     // 陀螺仪读数
int circle_count_start=0;   // 计算圈数的起始编码器读数
bool final_start_flag = false;    // 提前开始标志，用于先让云台打靶
bool stepper_motor_2_ban = false;   // 禁用步进电机二
bool set_laser_point_flag = false;  // 设置激光标志位
bool laser_flag = false;       // 激光打开标志

// 任务初始化
void task_init(void)
{
    servo_pid_init(track_PID_1, track_PID_2);   // 步进电机初始化
    set_laser_point(default_laser_point);       // 设置激光点坐标
    change_task_init();                         // 任务改变时执行一次
    Beep_up();
}
                         
// 任务主函数(循环执行)
void task_main(void)
{
    set_encoder_count();    // 更新编码器读数
    circle_detect();        // 检测是否走完一圈
    change_task();          // 通过按键判断是否改变任务
    display_task();         // 显示相关任务信息
    set_target_point(target_point); // 将OPENMV发来的矩阵中心点数据作为新目标点
    activate_task();        // 任务激活函数(任务主逻辑)
    
//    // 以下是测试
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

// 切换任务时需要初始化一些参数
void change_task_init(void)
{
    start_flag=0;
    circle=0;
    circle_count_start=encoder_count;
    final_start_flag = false;
    // 归零编码器读数
    motor_l.encoder.countnum = 0;
    motor_r.encoder.countnum = 0;
    // 激光关闭 禁止纠正激光坐标点
    laser_off();
    set_laser_point_flag = false;
    // 初始化巡线参数
    switch(task_id)
    {
        case 0: set_line_follow_config(gray_config_1);break;
        case 3: set_line_follow_config(gray_config_1);set_turn_type(left);
        case 4: set_line_follow_config(gray_config_1);set_turn_type(left);
        case 5: set_current_point(B);init_relative_l(90.0);break;    // 定位模块归零
    }
    
}

// 通过按钮切换任务
/**
    按钮1切换任务ID
    按钮2切换目标圈数N；N=2
    按钮3执行陀螺仪归零、步进电机标定零点位置
    按钮4开始执行任务

    任务1-3时，按下按钮4后直接执行任务
    任务4-6时，按下按钮4后，激光先进行打靶，稳定后按下按钮2，执行小车巡线
**/
void change_task(void)
{
    uint8_t keyState = readKeys();
    static float key3_count = 0;
    switch(keyState)
    {
        case 1: task_id++;task_id%=task_amount;change_task_init();break;
        case 2: target_cirlces%=max_circles;target_cirlces++;final_start_flag=true;break;
        case 3: cali_yaw();stepper_motor_set_zero(true);break;     // 标定云台 stepper_motor_set_zero(true);
        case 4: start_flag=1;target_point[0]=0;target_point[1]=0;break;
        case 5: DL_SYSCTL_resetDevice(1);break;
    }
    if(target_cirlces == 4) {laser_on();}
    if(target_cirlces == 5) {laser_off();}
//    if(key3_count > 1) {DL_SYSCTL_resetDevice(1);}
}

// ==================================任务逻辑==================================
// 激活任务
typedef void (*task_func_t)(void);
task_func_t task_funcs[] = {
    task1,  // task_id = 0
    task2,  // task_id = 1
    task3,  // task_id = 2
    task4,  // task_id = 3
    task5,  // task_id = 4
    task6,  // task_id = 5
    task7,  // 标定步进电机1
    task8,  // 标定激光坐标
    task9   // 发挥其他部分
};
void activate_task(void)
{
    if(!start_flag ||  task_id == 0xFF) {
        motor_speed(0.0, 0.0);
        return ;
    }
    task_funcs[task_id]();
}

// ===========任务1=============
void task1(void)
{    
    target_speed = 220;
    turn_speed = 100;
    turn_theta = 0.8;      // 空载2.45
    set_turn_left_complete_count(280);  // 转弯完成时走过的编码器读数390
    PID_[0] = 50;
    
    turn_angle = get_gray_output();
    car_drive();
    
    // 小车停止逻辑
    if(circle < target_cirlces) {return ;}
    start_flag = 0;
}

// ===========任务2=============
void task2(void)
{
    laser_flag = false;
    // 激光开关控制
    #define laser_on_min_count 5    // 打开激光时所需误差最小的次数
    static uint8_t laser_count = 0;     // 激光置信度
    float error_ = get_point_error();
    if(error_ < pow(inner_circle_radius, 2)) {laser_count++;}
    else {laser_count = 0;}
    if(laser_count > laser_on_min_count-1) {laser_on();}
    // 云台打靶
    stepper_motor_control_pid(0.0);
}

// ===========任务3=============
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
    
    // 激光开关控制
    #define laser_on_min_count 5    // 打开激光时所需误差最小的次数
    static uint8_t laser_count = 0;     // 激光置信度
    float error_ = get_point_error();
    if(error_ < pow(inner_circle_radius, 2)) {laser_count++;}
    else {laser_count = 0;}
    if(laser_count > laser_on_min_count-1) {laser_on();}
    // 循环右转寻找标靶
    static float bias_theta = 0.0;
    static int turn_count_ = 0;
    int step = 70/1.2;
    if(target_point[0] == 0 && target_point[1] == 0)
    {   // 没有识别到
        stepper_motor_rpm = 3000;
        stepper_motor_a = 250;
        bias_theta = ((target_cirlces == 1) ? 1 : -1) * 60 * (int)(turn_count_/step+1);       // N=1则右转，否则左转
        turn_count_++;
        if(turn_count_/step > 8) {turn_count_ = 8;}
    }
    else{
        stepper_motor_rpm = 5000;
        stepper_motor_a = 256;
    }
    stepper_motor_control_pid(bias_theta);
}

// ===========任务4=============
void task4(void)
{
    float start_turn_theta = 0.9;
    static bool start_turn_flag = false;
    // 提前瞄靶的pid
    track_PID_1[0] = 0.6;
    track_PID_1[2] = 1.0;
    
    if(final_start_flag){
        // 行驶中的pid
        track_PID_1[0] = 2.0;
        track_PID_1[2] = 1.8;
        // 转弯时单独设一个pid，同时C、D属于远点，需要调大
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
        // 根据当前所在线段适当调节p
        switch(current_point)
        {
            case C: track_PID_1[0] = 2.2;break;
        }
        // 小车行驶
        float target_speed_ = 280;
        float target_speed_up_k = 1.8;      // 使加速呈现梯形，加速快减速慢，中间匀速，此时的梯形交点为target_speed_up_k*target_speed_
        turn_speed = 100;
        // 动态调整速度，即正方形中间速度快，两边慢
        static float instant_relative_l = 0.0;
        if(turn_type == left) {instant_relative_l = relative_l;}        // 只有在小车完成转弯后才进行线性速度调整
        float bias_relative_l = relative_l - instant_relative_l;        // 转弯时bias为0
        #define square_side_ 70         // 小车完成转弯后，离下一个点大约70cm
        target_speed = -(target_speed_ * target_speed_up_k - turn_speed) * fabs(bias_relative_l - square_side_/2.0) / (square_side_/2.0) + 
                        target_speed_ * target_speed_up_k;
        // 限制最大值和最小值
        target_speed = MIN(MAX(target_speed, turn_speed), target_speed_);
        
        turn_theta = !start_turn_flag ? start_turn_theta : 0.8;      // 空载2.45
        set_turn_left_complete_count(250);  // 转弯完成时走过的编码器读数
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
    
    // 云台
    set_pid1(track_PID_1);
    angle_to_imu_zero = get_angle();
    Serial_JY901S_Zero_Yaw();
    getAngle(&roll, &pitch, &yaw);
//    static float last_yaw = 0.0;
    
//    float bias_yaw = fabs(yaw - last_yaw);
//    last_yaw = yaw;
//    add_servo_angle(bias_yaw);
//    stepper_motor_control_vel(last_yaw*2, 0.0);
    // 开启陀螺仪数据防抖
    debounce_flag = true;
    stepper_motor_control_pid(yaw + angle_to_imu_zero);
    // 激光
    set_laser_point_flag = true;
    if(!laser_flag){
        #define laser_on_min_count 5    // 打开激光时所需误差最小的次数
        static uint8_t laser_count = 0;     // 激光置信度
        float error_ = get_point_error();
        if(error_ < pow(inner_circle_radius, 2)) {laser_count++;}
        else {laser_count = 0;}
        if(laser_count > laser_on_min_count-1) {laser_on();}
    }   
        
    // 小车停止逻辑
    if(task_id == 3 || task_id == 5) {target_cirlces = 1;}
    if(task_id == 4) {target_cirlces = 2;}
    if(circle < target_cirlces) {return ;}
    laser_off();
    start_flag = 0;
}

// ===========任务5=============
void task5(void)
{
    task4();
}

// ===========任务6=============
void task6(void)
{
    #define PI 3.1415926535f
    #define square_side 100 * 0.95
    // 计算椭圆上的点
    int ellipse_point[2] = {0};
    ellipse_point[0] = target_point[0];
    ellipse_point[1] = target_point[1];
    float theta = PI/2.0;     // 起始点
    switch(current_point)       // 使用编码器读数，得出目标点。如小车在AC线段上，则目标点为圆的左上四分之一内
    {
        case A: theta += (relative_l/square_side + 0)* PI/2.0;break;
        case C: theta += (relative_l/square_side + 1) * PI/2.0;break;
        case D: theta += (relative_l/square_side + 2) * PI/2.0;break;
        case B: theta += (relative_l/square_side + 3) * PI/2.0;break;
    }
    // 公式为x=acosθy=bsinθ
    ellipse_point[0] += ellipse[0] * cos(theta);
    ellipse_point[1] -= ellipse[1] * sin(theta);
    set_target_point(ellipse_point);
    
    task4();
}

// ===========任务7 标定步进电机一=============
void task7(void)
{
    static bool cal_init = false;
    if(!cal_init) {Emm_v5_Cal(0x01, UART_2_INST);;}
    cal_init = true;
}
    
// ===========任务8 允许通过OPENMV传回的数据进行激光坐标纠正=============
void task8(void){
    laser_on();
    set_laser_point_flag = true;
}

// ===========任务9 发挥其它部分：小车自转，激光打靶=============
void task9(void){
    target_speed = 100;
    if(final_start_flag){
        laser_on();
        motor_speed(-target_speed, target_speed);  
    }
    
    // 云台
    track_PID_1[0] = 2.0;
    track_PID_1[2] = 1.5;
    set_pid1(track_PID_1);
    Serial_JY901S_Zero_Yaw();
    getAngle(&roll, &pitch, &yaw);
    // 开启陀螺仪数据防抖
    debounce_flag = true;
    stepper_motor_control_pid(yaw);
    // 停止逻辑
    #define ROUND(x) (int)(x + 0.5f)
    static float last_yaw = 0.0;
    // 解包裹
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
    if(yaw_ < 3*360) {return ;}         // 转完N圈视为结束
    laser_off();
    start_flag = 0;
}

// ===========任务信息显示=============
// 通过减少OLED上显示的字符数量，可以显著提高程序运行速度，从而提高步进电机的轮询率
void display_task(void)
{
    static bool display_word_init = false;
    // 任务id
    if(!display_word_init) display_6_8_string(0, 7, "task:");
    display_6_8_number(30, 7, task_id+1==256?0:task_id+1);
    // 目标圈数
    if(!display_word_init) display_6_8_string(50, 7, "N:");
    display_6_8_number(70, 7, target_cirlces);
//    // encoder计数
//    if(!display_word_init) display_6_8_string(0, 4, "encoder:");
//    display_6_8_number(60, 4, (int)encoder_count);
//    // 预定角度
//    display_6_8_string(0, 3, "angle:          ");
//    display_6_8_number(60, 3, turn_angle);
//    // 行驶类型
//    if(!display_word_init) display_6_8_string(0, 6, "turn:");
//    display_6_8_string(60, 6, turn_type==left?"left":"line");
//    // 已行驶的圈数
//    if(!display_word_init) display_6_8_string(0, 4, "circle:");
//    display_6_8_number(60, 4, circle);
    // 串口目标点
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
    // 任务状态
//    display_6_8_string(90, 7, start_flag?"busy":"free");
    display_word_init = true;
}
// ==================================任务逻辑==================================
// author:Tianmengfanxing

// 小车驱动逻辑。接收到开始标志start_flag后才运行电机运动
// 若小车状态为左转left，则参数为turn_speed turn_theta
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
        // 只有当小车完成转弯后才判定进入下一个点
        if(last_turn_type == straight ||last_turn_type == none) {approach_point();}
    }
    last_turn_type = turn_type;
}

// 通过编码器计算圈数是否+1
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
import pyb
from pyb import UART
import time
import sensor
# import display
from machine import LED
import display

uart = UART(3, 115200, timeout_char=1000)

sensor.reset()
sensor.set_hmirror(False)       #左右翻转
sensor.set_vflip(False)         #上下翻转

sensor.set_transpose(False)
sensor.set_pixformat(sensor.RGB565)
# sensor.set_pixformat(sensor.GRAYSCALE)

sensor.set_framesize(sensor.SVGA)

button1 = pyb.Pin("P1", pyb.Pin.IN, pyb.Pin.PULL_UP)
button2 = pyb.Pin("P9", pyb.Pin.IN, pyb.Pin.PULL_UP)

default_write = [45, 100, -20, 20, -50, 20]
default_black = [0, 49, -20, 20, -20, 20]
default_purple = [99, 100, -5, 5, -5, 5]
default_config = [45, 100, -20, 20, -50, 20,0, 49, -20, 20, -20, 20, 99, 100, -5, 5, -5, 5]
# thr_write = (55, 100, -20, 20, -20, 20)
# thr_black = (0, 20, -20, 20, -20, 20)

black_area_threshold = 9000
white_area_threshold = 8000
black_max_area = 220000         #黑色矩形框最大面积
b_x_stride = 10
b_y_stride = 7
w_x_stride = 10
w_y_stride = 7

img_w = sensor.width()
img_h = sensor.height()

#保存按键状态
last_state_1 = button1.value()
last_state_2 = button2.value()

#状态
state = 0               #状态机状态
state_thr = 0            #阈值状态 0表示白色 1黑色 2 激光
button_state = 0        #按钮状态
button1_state = 0       #按钮1状态，0表示未被按下，1表示按下
button2_state = 0       #按钮2状态，0表示未被按下，1表示按下

lcd = display.SPIDisplay()
count = 0


path = '\lab.txt'
#写数据
def write_lab(lines):
    for i in range(len(lines)):
        lines[i] = str(int(lines[i]))+'\n'

    with open(path, 'w') as file:
        file.write(''.join(lines))

def open_lab():
    with open(path, "r") as file:
        lines = file.readlines()  # 读取所有行到列表
    for i in range(len(lines)):  # 要处理所有行
        lines[i] = int(lines[i].strip())
    return lines

def bin_image(img,lines):
    threshold = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
    for i in range(len(threshold)):
        threshold[i] = int(threshold[i])
    img =img.binary([threshold])# 将图像进行二值化处理，符合阈值的像素将为白色，其余为黑色
    return img

#状态8激光追踪
def find_laser(img,lines):
    threshold = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
    for i in range(len(threshold)):
        threshold[i] = int(threshold[i])

    cirlce_size = 0
    center = [0, 0]
    max_b = 0
    for b in img.find_blobs([threshold],pixels_threshold=2, area_threshold=15, merge=True,invert = 0):
        # 在屏幕上画出色块
        # print(1)
        img.draw_rectangle(b.rect(), color = (255, 0, 0), scale = 2, thickness = 2)
        if(b.w() * b.h() > cirlce_size):
            cirlce_size = b.w() * b.h()
            max_b = b
    b = max_b
    if b == 0:
        return
    center = [b.x() + b.w()/2, b.y() + b.h()/2]
    uart.write("(" + fill_number(int(center[0])) + " " + fill_number(int(center[1])) + ")")
    # img.draw_circle(int(center[0]), int(center[1]), 2, color = (0, 0, 255), thickness = 2, fill = False)
    print("--------------->" + fill_number(int(center[0])) + "," + fill_number(int(center[1])))

def find_circle(img,thr_write,thr_black):
    black_blobs = img.find_blobs([thr_black],x_stride= b_x_stride ,y_stride = b_y_stride ,area_threshold = black_area_threshold ,merge =False) #找黑框

    if black_blobs:  #如果有目标
        for single_black_blob in black_blobs:
            if (single_black_blob.x() * single_black_blob.y() < black_max_area
                and single_black_blob.w() / single_black_blob.h() > 0.6
                and 1.8 > single_black_blob.w() / single_black_blob.h()):
                #img.draw_rectangle(single_black_blob.rect(),color=(255,0,255)) #绘制初步找黑框结果
                #找到的目标中，符合阈值的面积和总的区域之间的比值。因为黑框内部不是黑色，所以这个比值不会很大。
                if single_black_blob.pixels() / (single_black_blob.w()*single_black_blob.h()) < 0.6:
                    corner_b = single_black_blob.corners()
                    # 处在屏幕边缘的矩形舍去
                    if (corner_b[0][0] < 5 or corner_b[0][1] < 5 or corner_b[2][0] > img_w-6 or corner_b[2][1] > img_h-6
                        or corner_b[3][0] > img_w-6 or corner_b[3][1] > img_h-6):
                        continue
                    img.draw_rectangle(single_black_blob.rect(),color=(0,255,0))#绘制符合条件的区域
                    # 在屏幕上圈出矩形角点
                    # img.draw_circle(corner_b[0][0], corner_b[0][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    # img.draw_circle(corner_b[1][0], corner_b[1][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    # img.draw_circle(corner_b[2][0], corner_b[2][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    # img.draw_circle(corner_b[3][0], corner_b[3][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    #在区域内找白色
                    write_blobs = img.find_blobs([thr_write],area_threshold=2,roi =single_black_blob.rect(), x_stride= w_x_stride ,y_stride = w_y_stride ,area_threshold = white_area_threshold,merge =False)
                    if write_blobs:#如果有目标
                        largest_white = max(write_blobs, key=lambda b: b.pixels())#找到最大的块
                        white_corner = largest_white.corners()
                        if(corner_b[0][0] == white_corner[0][0] or corner_b[0][1] == white_corner[0][1] or
                           corner_b[2][0] == white_corner[2][0] or corner_b[2][1] == white_corner[2][1] or
                           single_black_blob.h() - largest_white.h()<2 or single_black_blob.w() - largest_white.w()<2):
                               continue

                        img.draw_rectangle(largest_white.rect(), color=(255, 0, 0))  # 绘制白色区域的矩形框
                        # corner = largest_white.corners()

                        # img.draw_rectangle(largest_white.rect(), color=(255, 0, 0))  # 绘制白色区域的矩形框

                        #绘制圆
                        white_width = largest_white.w()       #得到白色区域宽度
                        white_height = largest_white.h()      #得到白色区域高度
                        black_width = single_black_blob.w()   #黑色区域宽
                        black_height = single_black_blob.h()  #黑色区域高


                        # 在屏幕上圈出矩形角点
                        # img.draw_circle(corner[0][0], corner[0][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                        # img.draw_circle(corner[1][0], corner[1][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                        # img.draw_circle(corner[2][0], corner[2][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                        # img.draw_circle(corner[3][0], corner[3][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                        #p = single_black_blob.pixels() / (single_black_blob.w()*single_black_blob.h())

                        # #绘制中心点
                        # img.draw_cross(largest_white.cx()+target_offset[0],largest_white.cy()+target_offset[1],color=(0,0,255))
                        # img.draw_cross(single_black_blob.cx()+target_offset[0],single_black_blob.cy()+target_offset[1],color=(0,255,0))
                        #判断条件1：黑色区域面积和白色区域面积的比例；判断条件2：黑框和白色区域中心坐标的差值
                        if (1 < largest_white.pixels() / single_black_blob.pixels() < 5) and\
                            abs( largest_white.cx() - single_black_blob.cx() ) < 50 and \
                            abs( largest_white.cy() - single_black_blob.cy() ) < 50 :

                            #得到靶心
                            center_x = int(largest_white.cx() * 1.8 - single_black_blob.cx() * 0.8)
                            center_y = int(largest_white.cy() * 1.1 - single_black_blob.cy() * 0.1)

                            #得到圆半径
                            circle_r_x = white_width * 61 / 127 / 4 + black_width * 61 / 149 / 4 #+ white_height * 61 / 89 / 8 + black_height * 61 / 107.5 / 8
                            circle_r_y = white_height * 61 / 89 / 8 + black_height * 61 / 107.5 / 8 + white_width * 61 / 127 / 8 + black_width * 61 / 149 / 8
                            #标注圆
                            img.draw_cross(center_x,center_y + int(circle_r_y),color=(0,0,255))
                            img.draw_cross(center_x,center_y - int(circle_r_y),color=(0,255,0))
                            # 在图像上标注圆形
                            img.draw_circle(center_x, center_y, int(circle_r_x),
                                            color=(0, 255, 0), thickness=2)
                            #标注椭圆
                            img.draw_ellipse(center_x, center_y, int(circle_r_x), int(circle_r_y), 0, color = (255,255,255), thickness = 1, fill=False)


                            #标注靶心
                            # img.draw_cross(largest_white.cx()+target_offset[0],largest_white.cy()+target_offset[1],color=(0,0,255))
                            target = [center_x, center_y ] #白色区域中心坐标
                            img.draw_cross(target,color=(255,0,0),thickness=1) #绘制在画布上
                            c_x = str(center_x)
                            c_y = str(center_y)
                            uart.write("#" + fill_number(c_x) + "|" + fill_number(c_y) + "$")
                            uart.write("@" + fill_number(int(circle_r_x)) + "," + fill_number(int(circle_r_y)) + "%")#长轴短轴
                            global led_state
                            led_state = False
                            print(fill_number(c_x) + ',' + fill_number(c_y))
    return img
# def judge_thr(state_thr,lines):
#     if state_thr == 0:
#         thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
#     elif state_thr == 1:
#         thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
#     else:
#         state_thr = 0
#     return thr

# def fine_cir(img,lines

# 补齐数字至四位
def fill_number(num):
    num = str(num)
    l = 4-len(num)
    return l*"0" + num

lines = open_lab()
stride = 0.5    # 步长

clock = time.clock()
# lcd = display.SPIDisplay()
led = LED("LED_GREEN")


while True:
    led_state = True

    clock.tick()
    count += 1

    img = sensor.snapshot()

    # 检查按键1的状态
    if button1.value() != last_state_1:
        last_state_1 = button1.value()    # 更新按键1的状态

    # 检查按键2的状态
    if button2.value() != last_state_2:
        last_state_2 = button2.value()    # 更新按键2的状态

    # 如果去抖动时间已过，输出按键的状态
    if last_state_1 == 0:
        button1_state = 1
    else:
        button1_state = 0

    #按钮2
    if last_state_2 == 0:
        button2_state = 1
    else:
        button2_state = 0

    #判断按钮状态
    if button1_state == 0 and button2_state == 0 :
        button_state = 0
    elif button1_state == 1 and button2_state == 0 :
        button_state = 1
    elif button1_state == 0 and button2_state == 1 :
        button_state = 2
    elif button1_state == 1 and button2_state == 1 :
        button_state = 3


    #更改状态机状态
    if button_state == 3:
        if state <7 and state >= 0 :
            state +=1
        else :
            state = 0
        time.sleep(0.2)



    #进入状态机
    if(button_state == 3):
        write_lab(lines)
        lines_ = open_lab()
        lines = lines_
    if(len(lines) == 0):
        print("error")
        write_lab(default_config)
        lines = open_lab()
        continue
    if state == 0:
        thr_write = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
        thr_black = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]

        find_circle(img,thr_write,thr_black)
        # print(count)
        if (count % int(clock.fps()) == 0):
            # print("----------------------------->" + str(count % 30))
            find_laser(img,lines)


    elif state == 1:
        if button_state ==1 :
            if state_thr <2 :
                state_thr += 1
            else :
                state_thr = 0
        elif button_state == 2:
            if state_thr >0 :
                state_thr -= 1
            else :
                state_thr = 2
        time.sleep(0.1)
        # thr = judge_thr(state_thr,lines)
    elif state == 2:
        if state_thr == 0:
            if button_state == 1 :
                if  lines[0] > 0 :
                    lines[0] -= stride
                else :
                    lines[0] = 0
            elif button_state == 2:
                if  lines[0] < 100 :
                    lines[0] += stride
                else :
                    lines[0] = 100
            thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
            bin_image(img,thr)
        elif state_thr == 1 :
            if button_state == 1 :
                if  lines[6] > 0 :
                    lines[6] -= stride
                else :
                    lines[6] = 0
            elif button_state == 2:
                if  lines[6] < 100 :
                    lines[6] += stride
                else :
                    lines[6] = 100
            thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
            bin_image(img,thr)
        else :
            if button_state == 1 :
                if  lines[12] > 0 :
                    lines[12] -= stride
                else :
                    lines[12] = 0
            elif button_state == 2:
                if  lines[12] < 100 :
                    lines[12] += stride
                else :
                    lines[12] = 100
            thr = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
            bin_image(img,thr)
    elif state == 3:
        if state_thr == 0:
            if button_state == 1 :
                if  lines[1] > 0 :
                    lines[1] -= stride
                else :
                    lines[1] = 0
            elif button_state == 2:
                if  lines[1] < 100 :
                    lines[1] += stride
                else :
                    lines[1] = 100
            thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
            bin_image(img,thr)
        elif state_thr == 1:
            if button_state == 1 :
                if  lines[7] > 0 :
                    lines[7] -= stride
                else :
                    lines[7] = 0
            elif button_state == 2:
                if  lines[7] < 100 :
                    lines[7] += stride
                else :
                    lines[7] = 100
            thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
            bin_image(img,thr)
        else :
            if button_state == 1 :
                if  lines[13] > 0 :
                    lines[13] -= stride
                else :
                    lines[13] = 0
            elif button_state == 2:
                if  lines[13] < 100 :
                    lines[13] += stride
                else :
                    lines[13] = 100
            thr = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
            bin_image(img,thr)
    elif state == 4:
        if state_thr == 0:
            if button_state == 1 :
                if  lines[2] > -128 :
                    lines[2] -= stride
                else :
                    lines[2] = -128
            elif button_state == 2:
                if  lines[2] < 127 :
                    lines[2] += stride
                else :
                    lines[2] = 127
            thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
            bin_image(img,thr)
        elif state_thr == 1:
            if button_state == 1 :
                if  lines[8] > -128 :
                    lines[8] -= stride
                else :
                    lines[8] = -128
            elif button_state == 2:
                if  lines[8] < 127 :
                    lines[8] += stride
                else :
                    lines[8] = 127
            thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
            bin_image(img,thr)
        else :
            if button_state == 1 :
                if  lines[14] > -128 :
                    lines[14] -= stride
                else :
                    lines[14] = -128
            elif button_state == 2:
                if  lines[14] < 127 :
                    lines[14] += stride
                else :
                    lines[14] = 127
            thr = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
            bin_image(img,thr)
    elif state == 5:
        if state_thr == 0:
            if button_state == 1 :
                if  lines[3] > -128 :
                    lines[3] -= stride
                else :
                    lines[3] = -128
            elif button_state == 2:
                if  lines[3] < 127 :
                    lines[3] += stride
                else :
                    lines[3] = 127
            thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
            bin_image(img,thr)
        elif state_thr == 1:
            if button_state == 1 :
                if  lines[9] > -128 :
                    lines[9] -= stride
                else :
                    lines[9] = -128
            elif button_state == 2:
                if  lines[9] < 127 :
                    lines[9] += stride
                else :
                    lines[9] = 127
            thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
            bin_image(img,thr)
        else :
            if button_state == 1 :
                if  lines[15] > -128 :
                    lines[15] -= stride
                else :
                    lines[15] = -128
            elif button_state == 2:
                if  lines[15] < 127 :
                    lines[15] += stride
                else :
                    lines[15] = 127
            thr = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
            bin_image(img,thr)
    elif state == 6:
        if state_thr == 0:
            if button_state == 1 :
                if  lines[4] > -128 :
                    lines[4] -= stride
                else :
                    lines[4] = -128
            elif button_state == 2:
                if  lines[4] < 127 :
                    lines[4] += stride
                else :
                    lines[4] = 127
            thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
            bin_image(img,thr)
        elif state_thr == 1:
            if button_state == 1 :
                if  lines[10] > -128 :
                    lines[10] -= stride
                else :
                    lines[10] = -128
            elif button_state == 2:
                if  lines[10] < 127 :
                    lines[10] += stride
                else :
                    lines[10] = 127
            thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
            bin_image(img,thr)
        else :
            if button_state == 1 :
                if  lines[16] > -128 :
                    lines[16] -= stride
                else :
                    lines[16] = -128
            elif button_state == 2:
                if  lines[16] < 127 :
                    lines[16] += stride
                else :
                    lines[16] = 127
            thr = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
            bin_image(img,thr)
    elif state == 7:
        if state_thr == 0:
            if button_state == 1 :
                if  lines[5] > -128 :
                    lines[5] -= stride
                else :
                    lines[5] = -128
            elif button_state == 2:
                if  lines[5] < 127 :
                    lines[5] += stride
                else :
                    lines[5] = 127
            thr = [lines[0], lines[1], lines[2], lines[3],lines[4], lines[5]]
            bin_image(img,thr)
        elif state_thr == 1 :
            if button_state == 1 :
                if  lines[11] > -128 :
                    lines[11] -= stride
                else :
                    lines[11] = -128
            elif button_state == 2:
                if  lines[11] < 127 :
                    lines[11] += stride
                else :
                    lines[11] = 127
            thr = [lines[6], lines[7], lines[8], lines[9],lines[10], lines[11]]
            bin_image(img,thr)
        else :
            if button_state == 1 :
                if  lines[17] > -128 :
                    lines[17] -= stride
                else :
                    lines[17] = -128
            elif button_state == 2:
                if  lines[17] < 127 :
                    lines[17] += stride
                else :
                    lines[17] = 127
            thr = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
            bin_image(img,thr)
    # elif state == 8 :
    #     find_laser(img,lines)
    else :
        state = 0

    if(state > 0 and state < 8):
        led.off()
        # 显示参数
        for i in range(6):
            if(i%2==0):
                y = 90
            else:
                y = 0
            color = (255, 0, 0)
            if(state == i+2):
                color = (0, 255, 0)
            if state_thr == 0:
                img.draw_string(((int)(i/2))*200, y, str(int(lines[i])), color=color, scale=8)
            elif state_thr == 1 :
                img.draw_string(((int)(i/2))*200, y, str(int(lines[i+6])), color=color, scale=8)
            else :
                img.draw_string(((int)(i/2))*200, y, str(int(lines[i+12])), color=color, scale=8)
        if state_thr == 0 :
            img.draw_string(600, y, str("white"), color=(0,0,255), scale=6)
        elif state_thr == 1 :
            img.draw_string(600, y, str("black"), color=(0,0,255), scale=6)
        elif state_thr == 2 :
            img.draw_string(600, y, str("purple"), color=(0,0,255), scale=6)
        # img.draw_string(600, y, str(state_thr), color=(0,0,255), scale=6)
        else :
            state_thr = 0
    else :
        state =0

    # if(button_state == 3):
    #     write_lab(lines)


    # print(button_state)
    # print(state)
    # # print(lines)
    print("FPS %f" % clock.fps())
    # print('\n')
    # lcd.write(img)
    # lcd.write(img.scale(x_scale=128/800, y_scale=160/600))  # Take a picture and display the image.




    # print(str(button1.value())+'a'+str(button2.value())+'b')
    # print('last_state_1  = ' + str(last_state_1))
    # print('button1_state = ' + str(button1_state))
    # print('last_state_2  = ' + str(last_state_2))
    # print('button2_state = ' + str(button2_state))



    #img = sensor.snapshot().lens_corr(strength = 1.3, zoom = 1.0)
    # img = img.scale(0.8,0.8)
    #将图像显示在lcd中

    # time.sleep(0.1)

    if(led_state):
        led.on()
    else:
        led.off()




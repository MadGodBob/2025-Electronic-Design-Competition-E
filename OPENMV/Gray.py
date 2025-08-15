import sensor, image, time
from pyb import UART
from machine import LED



sensor.reset()
sensor.set_hmirror(False)
sensor.set_vflip(False)


sensor.set_transpose(False)
# sensor.set_pixformat(sensor.RGB565)
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.SVGA)
# from pyb import UART
# import lcd

uart = UART(3, 115200, timeout_char=1000)
# import display

img_w = sensor.width()
img_h = sensor.height()

#sensor.set_contrast(0) #对比度
# sensor.set_windowing([70,40,500,400])
# sensor.set_windowing([200,120,240,240]),
#sensor.skip_frames(time = 2000)
#exposure = sensor.get_exposure_us()
#sensor.set_auto_exposure(False,exposure_us=500)  #

clock = time.clock()

#黑框和白底的颜色阈值，可以宽泛一些
big_thr = 140
small_thr = 120
same_thr = big_thr
thr_write = (same_thr, 255)
thr_black = (0, same_thr)
thr_pur = (255, 255)

black_area_threshold = 8000
white_area_threshold = 7000
b_x_stride = 10
b_y_stride = 7
w_x_stride = 10
w_y_stride = 7

rotation = 0 #椭圆的旋转角度（弧度制），0表示长轴水平

target_offset = [0,0] #目标补偿量，根据视角倾斜角度而设，暂时手动填写
# lcd = display.SPIDisplay()

# 激光点只活动在屏幕中心附近
# 分别为中心那一块区域的长/2和宽/2
laser_bias_x_to_center = 70
laser_bias_y_to_center = 50


def fill_number(num):
    num = str(num)
    l = 4-len(num)
    return l*"0" + num

def find_laser(img):
    # threshold = [lines[12], lines[13], lines[14], lines[15],lines[16], lines[17]]
    # for i in range(len(threshold)):
    #     threshold[i] = int(threshold[i])

    cirlce_size = 0
    center = [0, 0]
    max_b = 0
    img = img.erode(3)
    laser_rect = [int(img_w/2) - laser_bias_x_to_center, int(img_h/2) - laser_bias_y_to_center, 2*laser_bias_x_to_center, 2*laser_bias_y_to_center]
    # print(laser_rect)
    for b in img.find_blobs([thr_pur],pixels_threshold=2, roi = laser_rect, area_threshold=15, merge=True,invert = 0):
        # 在屏幕上画出色块
        # print(1)
        if(b.w() * b.h() > cirlce_size):
            cirlce_size = b.w() * b.h()
            max_b = b
    b = max_b
    if b == 0:
        return
    img.draw_rectangle(b.rect(), color = (255, 0, 0), scale = 2, thickness = 2)
    center = [b.x() + b.w()/2, b.y() + b.h()/2]
    uart.write("(" + fill_number(int(center[0])) + " " + fill_number(int(center[1])) + ")")
    # img.draw_circle(int(center[0]), int(center[1]), 2, color = (0, 0, 255), thickness = 2, fill = False)
    print("--------------->" + fill_number(int(center[0])) + "," + fill_number(int(center[1])))

count = 0
led = LED("LED_GREEN")

while(True):
    # timer = millis()
    led_state = True
    count += 1
    clock.tick()


    img = sensor.snapshot()

    black_blobs = img.find_blobs([thr_black],x_stride= b_x_stride ,y_stride = b_y_stride ,area_threshold = black_area_threshold ,merge =False) #找黑框

    success = False
    if black_blobs:  #如果有目标
        # print(1)
        for single_black_blob in black_blobs:
            #img.draw_rectangle(single_black_blob.rect(),color=(255,0,255)) #绘制初步找黑框结果
            #找到的目标中，符合阈值的面积和总的区域之间的比值。因为黑框内部不是黑色，所以这个比值不会很大。
            if single_black_blob.pixels() / (single_black_blob.w()*single_black_blob.h()) < 0.5:
                corner_b = single_black_blob.corners()
                # 处在屏幕边缘的矩形舍去
                if (corner_b[0][0] < 3 or corner_b[0][1] < 3 or corner_b[2][0] > img_w-4 or corner_b[2][1] > img_h-4
                    or corner_b[3][0] > img_w-4 or corner_b[3][1] > img_h-4):
                    continue
                img.draw_rectangle(single_black_blob.rect(),color=(0,255,255))#绘制符合条件的区域
                # 在屏幕上圈出矩形角点
                # img.draw_circle(corner_b[0][0], corner_b[0][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                # img.draw_circle(corner_b[1][0], corner_b[1][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                # img.draw_circle(corner_b[2][0], corner_b[2][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                # img.draw_circle(corner_b[3][0], corner_b[3][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                #在区域内找白色
                write_blobs = img.find_blobs([thr_write],area_threshold=2,roi =single_black_blob.rect(), x_stride= w_x_stride ,y_stride = w_y_stride ,area_threshold = white_area_threshold,merge =False)
                print(single_black_blob.rect())
                if write_blobs:#如果有目标
                    largest_white = max(write_blobs, key=lambda b: b.pixels())#找到最大的块
                    white_corner = largest_white.corners()
                    if(corner_b[0][0] == white_corner[0][0] or corner_b[0][1] == white_corner[0][1] or
                       corner_b[2][0] == white_corner[2][0] or corner_b[2][1] == white_corner[2][1] or
                       single_black_blob.h() - largest_white.h()<2 or single_black_blob.w() - largest_white.w()<2):
                           continue
                    img.draw_rectangle(largest_white.rect(), color=(255, 0, 0))  # 绘制白色区域的矩形框

                    #绘制圆
                    white_width = largest_white.w()       #得到白色区域宽度
                    white_height = largest_white.h()      #得到白色区域高度
                    black_width = single_black_blob.w()   #黑色区域宽
                    black_height = single_black_blob.h()  #黑色区域高


                    # 在屏幕上圈出矩形角点
                    # corner = largest_white.corners()
                    # img.draw_circle(corner[0][0], corner[0][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    # img.draw_circle(corner[1][0], corner[1][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    # img.draw_circle(corner[2][0], corner[2][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    # img.draw_circle(corner[3][0], corner[3][1], 2, color = (0, 0, 255), thickness = 2, fill = False)
                    #p = single_black_blob.pixels() / (single_black_blob.w()*single_black_blob.h())

                    # #绘制中心点
                    # img.draw_cross(largest_white.cx()+target_offset[0],largest_white.cy()+target_offset[1],color=(0,0,255))
                    # img.draw_cross(single_black_blob.cx()+target_offset[0],single_black_blob.cy()+target_offset[1],color=(0,255,0))
                    #判断条件1：黑色区域面积和白色区域面积的比例；判断条件2：黑框和白色区域中心坐标的差值
                    if (0.5 < largest_white.pixels() / single_black_blob.pixels() < 7 and
                        abs( largest_white.cx() - single_black_blob.cx() ) < 80 and
                        abs( largest_white.cy() - single_black_blob.cy() ) < 80 ):
                        #得到靶心
                        center_x = int(largest_white.cx() * 1.7 - single_black_blob.cx() * 0.7) +target_offset[0]
                        center_y = int(largest_white.cy() * 1.1 - single_black_blob.cy() * 0.1)+target_offset[1]

                        #得到圆半径
                        circle_r_x = white_width * 61 / 127 / 4 + black_width * 61 / 149 / 4 #+ white_height * 61 / 89 / 8 + black_height * 61 / 107.5 / 8
                        circle_r_y = white_height * 61 / 89 / 8 + black_height * 61 / 107.5 / 8 + white_width * 61 / 127 / 8 + black_width * 61 / 149 / 8
                        #标注圆
                        # img.draw_cross(center_x,center_y + int(circle_r_y),color=(0,0,255))
                        # img.draw_cross(center_x,center_y - int(circle_r_y),color=(0,255,0))
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
                        print(fill_number(c_x) + ',' + fill_number(c_y))
                        success = True
                        led_state = False
    # 识别失败，改变阈值
    if not success:
        if same_thr != small_thr:
            same_thr = small_thr
        else:
            same_thr = big_thr
        thr_write = (same_thr, 255)
        thr_black = (0, same_thr)

    if (count % int(clock.fps()) == 0):
        # print("----------------------------->" + str(count % 30))
        find_laser(img)

    if(led_state):
        led.on()
    else:
        led.off()

    fps = clock.fps()

    print("FPS %f" % clock.fps())
    # print(count)

    # uart.write(str(int(fps))+'\n')
    # img.draw_string(10, 10, "FPS: " + str(int(fps)), color=(255, 0, 0))
    # lcd.write(img)
    # print("FPS %f" % clock.fps())


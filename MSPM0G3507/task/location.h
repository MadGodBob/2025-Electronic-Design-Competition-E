#ifndef __LOCATION_H__
#define __LOCATION_H__
 
#include "headfile.h"

enum CurrentPoint{ 
    A,
    B,
    C,
    D
};

extern float relative_l;
extern enum CurrentPoint current_point;

float get_angle(void);
void convert_xy_to_angle(void);
void convert_absolute_to_xy(void);
enum CurrentPoint switch_to_next_point(enum CurrentPoint P);
void set_current_point(enum CurrentPoint P);
void approach_point(void);
void init_relative_l(float l);
void set_relative_l(void);

#endif
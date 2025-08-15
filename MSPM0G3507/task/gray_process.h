#ifndef __TASK_PROCESS_H
#define __TASK_PROCESS_H

#include "headfile.h"

typedef enum {
    left,
    straight,
    none
} TurnType;
extern TurnType turn_type;
void set_line_follow_config(float *config);
float get_gray_output(void);
void turn_maintain(float output);
void set_turn_left_complete_count(int c);
void set_turn_type(TurnType t);
void clear_last_output(void);

#endif
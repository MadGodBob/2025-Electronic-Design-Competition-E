#ifndef __HEADFILE_H
#define __HEADFILE_H

#define ABS(X)  (((X)>0)?(X):-(X))
#define MAX(a,b)  ((a)>(b)?(a):(b))
#define MIN(a,b)  ((a)>(b)?(b):(a))

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#ifndef u64
#define u64 uint64_t
#endif

/*********************************************************/
#include "ti_msp_dl_config.h"
#include "system.h"
#include "oled.h"
#include "ssd1306.h"
#include "uart.h"
#include "motor.h"
#include "beep.h"
#include "led.h"
#include "key.h"
#include "jy901.h" 
#include "gray.h"
#include "gray_process.h"
#include "task.h"
#include "track.h"
#include "yunyai.h"
#include "location.h"
#include "stepper_motor.h"

#endif

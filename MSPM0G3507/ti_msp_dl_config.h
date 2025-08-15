/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000



/* Defines for MOTOR */
#define MOTOR_INST                                                         TIMA1
#define MOTOR_INST_IRQHandler                                   TIMA1_IRQHandler
#define MOTOR_INST_INT_IRQN                                     (TIMA1_INT_IRQn)
#define MOTOR_INST_CLK_FREQ                                             10000000
/* GPIO defines for channel 0 */
#define GPIO_MOTOR_C0_PORT                                                 GPIOB
#define GPIO_MOTOR_C0_PIN                                          DL_GPIO_PIN_4
#define GPIO_MOTOR_C0_IOMUX                                      (IOMUX_PINCM17)
#define GPIO_MOTOR_C0_IOMUX_FUNC                     IOMUX_PINCM17_PF_TIMA1_CCP0
#define GPIO_MOTOR_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_MOTOR_C1_PORT                                                 GPIOB
#define GPIO_MOTOR_C1_PIN                                          DL_GPIO_PIN_1
#define GPIO_MOTOR_C1_IOMUX                                      (IOMUX_PINCM13)
#define GPIO_MOTOR_C1_IOMUX_FUNC                     IOMUX_PINCM13_PF_TIMA1_CCP1
#define GPIO_MOTOR_C1_IDX                                    DL_TIMER_CC_1_INDEX



/* Defines for ENCODER1A */
#define ENCODER1A_INST                                                  (TIMG12)
#define ENCODER1A_INST_IRQHandler                              TIMG12_IRQHandler
#define ENCODER1A_INST_INT_IRQN                                (TIMG12_INT_IRQn)
#define ENCODER1A_INST_LOAD_VALUE                                       (39999U)
/* GPIO defines for channel 0 */
#define GPIO_ENCODER1A_C0_PORT                                             GPIOB
#define GPIO_ENCODER1A_C0_PIN                                     DL_GPIO_PIN_20
#define GPIO_ENCODER1A_C0_IOMUX                                  (IOMUX_PINCM48)
#define GPIO_ENCODER1A_C0_IOMUX_FUNC                IOMUX_PINCM48_PF_TIMG12_CCP0

/* Defines for ENCODER2A */
#define ENCODER2A_INST                                                   (TIMG7)
#define ENCODER2A_INST_IRQHandler                               TIMG7_IRQHandler
#define ENCODER2A_INST_INT_IRQN                                 (TIMG7_INT_IRQn)
#define ENCODER2A_INST_LOAD_VALUE                                       (39999U)
/* GPIO defines for channel 0 */
#define GPIO_ENCODER2A_C0_PORT                                             GPIOA
#define GPIO_ENCODER2A_C0_PIN                                     DL_GPIO_PIN_17
#define GPIO_ENCODER2A_C0_IOMUX                                  (IOMUX_PINCM39)
#define GPIO_ENCODER2A_C0_IOMUX_FUNC                 IOMUX_PINCM39_PF_TIMG7_CCP0





/* Defines for CLOCK */
#define CLOCK_INST                                                       (TIMA0)
#define CLOCK_INST_IRQHandler                                   TIMA0_IRQHandler
#define CLOCK_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define CLOCK_INST_LOAD_VALUE                                            (9999U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART3
#define UART_0_INST_FREQUENCY                                           80000000
#define UART_0_INST_IRQHandler                                  UART3_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOB
#define GPIO_UART_0_TX_PORT                                                GPIOB
#define GPIO_UART_0_RX_PIN                                         DL_GPIO_PIN_3
#define GPIO_UART_0_TX_PIN                                         DL_GPIO_PIN_2
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM16)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM15)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM16_PF_UART3_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM15_PF_UART3_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_80_MHZ_115200_BAUD                                      (43)
#define UART_0_FBRD_80_MHZ_115200_BAUD                                      (26)
/* Defines for UART_1 */
#define UART_1_INST                                                        UART0
#define UART_1_INST_FREQUENCY                                           40000000
#define UART_1_INST_IRQHandler                                  UART0_IRQHandler
#define UART_1_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOA
#define GPIO_UART_1_RX_PIN                                        DL_GPIO_PIN_31
#define GPIO_UART_1_TX_PIN                                        DL_GPIO_PIN_28
#define GPIO_UART_1_IOMUX_RX                                      (IOMUX_PINCM6)
#define GPIO_UART_1_IOMUX_TX                                      (IOMUX_PINCM3)
#define GPIO_UART_1_IOMUX_RX_FUNC                       IOMUX_PINCM6_PF_UART0_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                       IOMUX_PINCM3_PF_UART0_TX
#define UART_1_BAUD_RATE                                                (115200)
#define UART_1_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_1_FBRD_40_MHZ_115200_BAUD                                      (45)
/* Defines for UART_2 */
#define UART_2_INST                                                        UART1
#define UART_2_INST_FREQUENCY                                           40000000
#define UART_2_INST_IRQHandler                                  UART1_IRQHandler
#define UART_2_INST_INT_IRQN                                      UART1_INT_IRQn
#define GPIO_UART_2_RX_PORT                                                GPIOA
#define GPIO_UART_2_TX_PORT                                                GPIOA
#define GPIO_UART_2_RX_PIN                                         DL_GPIO_PIN_9
#define GPIO_UART_2_TX_PIN                                         DL_GPIO_PIN_8
#define GPIO_UART_2_IOMUX_RX                                     (IOMUX_PINCM20)
#define GPIO_UART_2_IOMUX_TX                                     (IOMUX_PINCM19)
#define GPIO_UART_2_IOMUX_RX_FUNC                      IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART_2_IOMUX_TX_FUNC                      IOMUX_PINCM19_PF_UART1_TX
#define UART_2_BAUD_RATE                                                (115200)
#define UART_2_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_2_FBRD_40_MHZ_115200_BAUD                                      (45)





/* Port definition for Pin Group PORTA */
#define PORTA_PORT                                                       (GPIOA)

/* Defines for BEEP: GPIOA.22 with pinCMx 47 on package pin 18 */
#define PORTA_BEEP_PIN                                          (DL_GPIO_PIN_22)
#define PORTA_BEEP_IOMUX                                         (IOMUX_PINCM47)
/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for PIN_LED: GPIOA.0 with pinCMx 1 on package pin 33 */
#define LED_PIN_LED_PIN                                          (DL_GPIO_PIN_0)
#define LED_PIN_LED_IOMUX                                         (IOMUX_PINCM1)
/* Port definition for Pin Group PORTB */
#define PORTB_PORT                                                       (GPIOB)

/* Defines for LCD_SDA: GPIOB.15 with pinCMx 32 on package pin 3 */
#define PORTB_LCD_SDA_PIN                                       (DL_GPIO_PIN_15)
#define PORTB_LCD_SDA_IOMUX                                      (IOMUX_PINCM32)
/* Defines for LCD_RST: GPIOB.14 with pinCMx 31 on package pin 2 */
#define PORTB_LCD_RST_PIN                                       (DL_GPIO_PIN_14)
#define PORTB_LCD_RST_IOMUX                                      (IOMUX_PINCM31)
/* Defines for LCD_DC: GPIOB.22 with pinCMx 50 on package pin 21 */
#define PORTB_LCD_DC_PIN                                        (DL_GPIO_PIN_22)
#define PORTB_LCD_DC_IOMUX                                       (IOMUX_PINCM50)
/* Defines for LCD_CS: GPIOB.16 with pinCMx 33 on package pin 4 */
#define PORTB_LCD_CS_PIN                                        (DL_GPIO_PIN_16)
#define PORTB_LCD_CS_IOMUX                                       (IOMUX_PINCM33)
/* Defines for AIN2: GPIOB.0 with pinCMx 12 on package pin 47 */
#define PORTB_AIN2_PIN                                           (DL_GPIO_PIN_0)
#define PORTB_AIN2_IOMUX                                         (IOMUX_PINCM12)
/* Defines for AIN1: GPIOB.13 with pinCMx 30 on package pin 1 */
#define PORTB_AIN1_PIN                                          (DL_GPIO_PIN_13)
#define PORTB_AIN1_IOMUX                                         (IOMUX_PINCM30)
/* Defines for BIN1: GPIOB.8 with pinCMx 25 on package pin 60 */
#define PORTB_BIN1_PIN                                           (DL_GPIO_PIN_8)
#define PORTB_BIN1_IOMUX                                         (IOMUX_PINCM25)
/* Defines for BIN2: GPIOB.7 with pinCMx 24 on package pin 59 */
#define PORTB_BIN2_PIN                                           (DL_GPIO_PIN_7)
#define PORTB_BIN2_IOMUX                                         (IOMUX_PINCM24)
/* Defines for LCD_SCL: GPIOB.17 with pinCMx 43 on package pin 14 */
#define PORTB_LCD_SCL_PIN                                       (DL_GPIO_PIN_17)
#define PORTB_LCD_SCL_IOMUX                                      (IOMUX_PINCM43)
/* Defines for ENCODER1B: GPIOA.24 with pinCMx 54 on package pin 25 */
#define GPIO_ENCODER_ENCODER1B_PORT                                      (GPIOA)
#define GPIO_ENCODER_ENCODER1B_PIN                              (DL_GPIO_PIN_24)
#define GPIO_ENCODER_ENCODER1B_IOMUX                             (IOMUX_PINCM54)
/* Defines for ENCODER2B: GPIOB.18 with pinCMx 44 on package pin 15 */
#define GPIO_ENCODER_ENCODER2B_PORT                                      (GPIOB)
#define GPIO_ENCODER_ENCODER2B_PIN                              (DL_GPIO_PIN_18)
#define GPIO_ENCODER_ENCODER2B_IOMUX                             (IOMUX_PINCM44)
/* Defines for KEY_0: GPIOA.15 with pinCMx 37 on package pin 8 */
#define KEY_KEY_0_PORT                                                   (GPIOA)
#define KEY_KEY_0_PIN                                           (DL_GPIO_PIN_15)
#define KEY_KEY_0_IOMUX                                          (IOMUX_PINCM37)
/* Defines for KEY_1: GPIOA.16 with pinCMx 38 on package pin 9 */
#define KEY_KEY_1_PORT                                                   (GPIOA)
#define KEY_KEY_1_PIN                                           (DL_GPIO_PIN_16)
#define KEY_KEY_1_IOMUX                                          (IOMUX_PINCM38)
/* Defines for KEY_2: GPIOA.18 with pinCMx 40 on package pin 11 */
#define KEY_KEY_2_PORT                                                   (GPIOA)
#define KEY_KEY_2_PIN                                           (DL_GPIO_PIN_18)
#define KEY_KEY_2_IOMUX                                          (IOMUX_PINCM40)
/* Defines for KEY_3: GPIOB.19 with pinCMx 45 on package pin 16 */
#define KEY_KEY_3_PORT                                                   (GPIOB)
#define KEY_KEY_3_PIN                                           (DL_GPIO_PIN_19)
#define KEY_KEY_3_IOMUX                                          (IOMUX_PINCM45)
/* Defines for GRAY_1: GPIOB.27 with pinCMx 58 on package pin 29 */
#define GRAY_GRAY_1_PORT                                                 (GPIOB)
#define GRAY_GRAY_1_PIN                                         (DL_GPIO_PIN_27)
#define GRAY_GRAY_1_IOMUX                                        (IOMUX_PINCM58)
/* Defines for GRAY_2: GPIOB.26 with pinCMx 57 on package pin 28 */
#define GRAY_GRAY_2_PORT                                                 (GPIOB)
#define GRAY_GRAY_2_PIN                                         (DL_GPIO_PIN_26)
#define GRAY_GRAY_2_IOMUX                                        (IOMUX_PINCM57)
/* Defines for GRAY_3: GPIOB.23 with pinCMx 51 on package pin 22 */
#define GRAY_GRAY_3_PORT                                                 (GPIOB)
#define GRAY_GRAY_3_PIN                                         (DL_GPIO_PIN_23)
#define GRAY_GRAY_3_IOMUX                                        (IOMUX_PINCM51)
/* Defines for GRAY_4: GPIOB.21 with pinCMx 49 on package pin 20 */
#define GRAY_GRAY_4_PORT                                                 (GPIOB)
#define GRAY_GRAY_4_PIN                                         (DL_GPIO_PIN_21)
#define GRAY_GRAY_4_IOMUX                                        (IOMUX_PINCM49)
/* Defines for GRAY_5: GPIOB.10 with pinCMx 27 on package pin 62 */
#define GRAY_GRAY_5_PORT                                                 (GPIOB)
#define GRAY_GRAY_5_PIN                                         (DL_GPIO_PIN_10)
#define GRAY_GRAY_5_IOMUX                                        (IOMUX_PINCM27)
/* Defines for GRAY_6: GPIOA.25 with pinCMx 55 on package pin 26 */
#define GRAY_GRAY_6_PORT                                                 (GPIOA)
#define GRAY_GRAY_6_PIN                                         (DL_GPIO_PIN_25)
#define GRAY_GRAY_6_IOMUX                                        (IOMUX_PINCM55)
/* Defines for GRAY_7: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GRAY_GRAY_7_PORT                                                 (GPIOB)
#define GRAY_GRAY_7_PIN                                         (DL_GPIO_PIN_24)
#define GRAY_GRAY_7_IOMUX                                        (IOMUX_PINCM52)
/* Defines for GRAY_8: GPIOA.27 with pinCMx 60 on package pin 31 */
#define GRAY_GRAY_8_PORT                                                 (GPIOA)
#define GRAY_GRAY_8_PIN                                         (DL_GPIO_PIN_27)
#define GRAY_GRAY_8_IOMUX                                        (IOMUX_PINCM60)
/* Defines for GRAY_9: GPIOB.11 with pinCMx 28 on package pin 63 */
#define GRAY_GRAY_9_PORT                                                 (GPIOB)
#define GRAY_GRAY_9_PIN                                         (DL_GPIO_PIN_11)
#define GRAY_GRAY_9_IOMUX                                        (IOMUX_PINCM28)
/* Defines for GRAY_10: GPIOB.5 with pinCMx 18 on package pin 53 */
#define GRAY_GRAY_10_PORT                                                (GPIOB)
#define GRAY_GRAY_10_PIN                                         (DL_GPIO_PIN_5)
#define GRAY_GRAY_10_IOMUX                                       (IOMUX_PINCM18)
/* Defines for GRAY_11: GPIOA.7 with pinCMx 14 on package pin 49 */
#define GRAY_GRAY_11_PORT                                                (GPIOA)
#define GRAY_GRAY_11_PIN                                         (DL_GPIO_PIN_7)
#define GRAY_GRAY_11_IOMUX                                       (IOMUX_PINCM14)
/* Defines for GRAY_12: GPIOB.25 with pinCMx 56 on package pin 27 */
#define GRAY_GRAY_12_PORT                                                (GPIOB)
#define GRAY_GRAY_12_PIN                                        (DL_GPIO_PIN_25)
#define GRAY_GRAY_12_IOMUX                                       (IOMUX_PINCM56)
/* Defines for LASER: GPIOA.13 with pinCMx 35 on package pin 6 */
#define PORTA_LASER_PIN                                         (DL_GPIO_PIN_13)
#define PORTA_LASER_IOMUX                                        (IOMUX_PINCM35)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_SYSCTL_CLK_init(void);
void SYSCFG_DL_MOTOR_init(void);
void SYSCFG_DL_ENCODER1A_init(void);
void SYSCFG_DL_ENCODER2A_init(void);
void SYSCFG_DL_CLOCK_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_UART_1_init(void);
void SYSCFG_DL_UART_2_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */

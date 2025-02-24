/******************************************************************************
 * Copyright (C) 2024 xxxxxx, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_driver.h
 *
 * @par dependencies
 * - stdio.h
 * - stdint.h
 *
 * @author ZhuangZhong | R&D Dept. | EternalChip, Inc.(Gmbh)
 *
 * @brief BSP layer's Advanced LED driver for STM32F4xx
 *
 * Processing flow:
 *
 * call directly.
 *
 * @version V1.0 2025-02-24
 *
 * @note 1 tab == 4 spaces!
 *
 *****************************************************************************/

#ifndef __BSP_LED_DRIVER_H__
#define __BSP_LED_DRIVER_H__

//******************************** Includes *********************************//

#include <stdint.h>
#include <stdio.h>
//******************************** Includes *********************************//


//******************************** Defines **********************************//

#define INITED             (0x01)   /* LED driver initialized                */
#define UNINITED           (0x00)   /* LED driver not initialized            */

#define OS_SUPPORTING             /* Define to enable OS supporting features */

typedef enum
{
    PROPORTION_ON_OFF_1_1 =    0,   /* 1:1 proportion of on/off time.        */
    PROPORTION_ON_OFF_1_2 =    1,   /* 1:2 proportion of on/off time.        */
    PROPORTION_ON_OFF_1_3 =    2,   /* 1:3 proportion of on/off time.        */
    PROPORTION_ON_OFF_NUM       ,   /* Number of proportion types.           */
    PROPORTION_ON_OFF_x_x = 0xFF,   /* Reserved.                             */
} proportion_t;

typedef enum
{
    LED_OK                =    0,     /* Operation completed successfully.   */
    LED_ERROR             =    1,     /* Run-time error without case matched */
    LED_ERRORTIMEOUT      =    2,     /* Operation failed with timeout       */
    LED_ERRORRESOURCE     =    3,     /* Resource not available.             */
    LED_ERRORPARAMETER    =    4,     /* Parameter error.                    */
    LED_ERRORNOMEMORY     =    5,     /* Out of memory.                      */
    LED_ERRORISR          =    6,     /* Not allowed in ISR context          */
    LED_STATUS_NUM              ,     /* Number of led status                */
    LED_RESERVED          = 0xFF,     /* Reserved                            */
} led_status_t;
//******************************** Defines **********************************//


//******************************** Declaring ********************************//

typedef struct
{
    led_status_t (*pf_led_on)  (void); /* Function to turn the LED on        */
    led_status_t (*pf_led_off) (void); /* Function to turn the LED off       */
} led_operations_t;

typedef struct
{
    /* Function to get the time base in milliseconds */
    led_status_t (*pf_get_time_base_ms) (uint32_t *const); 
} time_base_ms_t;

typedef struct
{
    /* Function to delay in milliseconds */
    led_status_t (*pf_os_delay_ms)       (const uint32_t); 
} os_delay_ms_t;

typedef led_status_t (*pf_led_control_t) (
                                       const uint32_t ,   //     Cycle time[ms]
                                       const uint32_t ,   // Blink times[times]
                                       const proportion_t //  proportion_on_off
                                         );
typedef struct
{
    /******************Target of Status*****************/
    uint32_t                                   is_inited;
    /**********Target of enternal requirements**********/
    /* The whole time of blink                         */
    uint32_t                               cycle_time_ms;
    /* The times of blink                              */
    uint32_t                                 blink_times;
    /* The proportion ralationship of light on and off */
    proportion_t                       proportion_on_off;

    /***************Target of internal IOs**************/
    /* The APIs from Core layer                        */
    led_operations_t                    *p_led_opes_inst;
    time_base_ms_t                  *p_time_base_ms_inst;
#ifdef OS_SUPPORTING
    /* The APIs from OS layer                          */
    os_delay_ms_t                    *p_os_delay_ms_inst;
#endif // End of OS_SUPPORTING

    /**************Target of enternal APIs**************/
    pf_led_control_t                    pf_led_controler;

} bsp_led_driver_t;

led_status_t led_driver_inst(
                            const bsp_led_driver_t   *const      self,
                            const led_operations_t   *const   led_ops,
#ifdef OS_SUPPORTING
                            const os_delay_ms_t      *const  os_delay,
#endif // End of OS_SUPPORTING
                            const time_base_ms_t     *const time_base
                            );
//******************************** Declaring ********************************//

#endif // End of __BSP_LED_DRIVER_H__

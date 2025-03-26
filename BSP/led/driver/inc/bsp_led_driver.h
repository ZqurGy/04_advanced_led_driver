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
 * @author ZhuangZhong | R&D Dept. | xxxxxx, Inc.(Gmbh)
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

#define OS_SUPPORTING               /* switch of enable OS supporting        */
#define DEBUG_ON                    /* swtich of enable debug                */

#ifdef DEBUG_ON
#define DEBUG_OUT(X)    printf(X)   /* Debug output macro                    */
#else
#define DEBUG_OUT(X)
#endif

typedef enum
{
    LED_NOT_INITED     =    0,      /* LED not initialized.                  */
    LED_INITED         =    1,      /* LED initialized.                      */
} led_driver_init_t;

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
    LED_OK                =    0,   /* Operation completed successfully.     */
    LED_ERROR             =    1,   /* Run-time error without case matched   */
    LED_ERRORTIMEOUT      =    2,   /* Operation failed with timeout         */
    LED_ERRORRESOURCE     =    3,   /* Resource not available.               */
    LED_ERRORPARAMETER    =    4,   /* Parameter error.                      */
    LED_ERRORNOMEMORY     =    5,   /* Out of memory.                        */
    LED_ERRORISR          =    6,   /* Not allowed in ISR context            */
    LED_STATUS_NUM              ,   /* Number of led status                  */
    LED_RESERVED          = 0xFF,   /* Reserved                              */
} led_status_t;
//******************************** Defines **********************************//


//******************************** Declaring ********************************//

typedef struct bsp_led_driver_s bsp_led_driver_t;

typedef struct
{
    /* Function to delay in microsecond  */
    led_status_t (*pf_os_delay_us)       (const uint32_t); 
    /* Function to delay in milliseconds */
    led_status_t (*pf_os_delay_ms)       (const uint32_t);
    /* Function to delay in minute       */
    led_status_t (*pf_os_delay_mm)       (const uint32_t); 
} os_delay_t;

typedef struct
{
    led_status_t (*pf_led_on)  (void); /* Function to turn the LED on        */
    led_status_t (*pf_led_off) (void); /* Function to turn the LED off       */
} led_operations_t;

typedef struct
{
    /* Function to get the time base in microsecond  */
    led_status_t (*pf_get_time_base_us) (uint32_t *const); 
    /* Function to get the time base in milliseconds */
    led_status_t (*pf_get_time_base_ms) (uint32_t *const); 
    /* Function to get the time base in minute       */
    led_status_t (*pf_get_time_base_mm) (uint32_t *const); 
} time_base_t;

typedef led_status_t (*pf_led_control_t) (
                              bsp_led_driver_t *const, //  Pointer to itself
                        const uint32_t               , //     Cycle time[ms]
                        const uint32_t               , // Blink times[times]
                        const proportion_t             //  proportion_on_off
                                         );
typedef struct bsp_led_driver_s
{
    /******************Target of Status*****************/
    // TBD: need lock
    led_driver_init_t                          is_inited;
    /**********Target of enternal requirements**********/
    /* The whole time of blink                         */
    uint32_t                               cycle_time_ms;
    /* The times of blink                              */
    uint32_t                                 blink_times;
    /* The proportion ralationship of light on and off */
    proportion_t                       proportion_on_off;

    /***************Target of internal IOs**************/
#ifdef OS_SUPPORTING
    /* The APIs from OS layer                          */
    const os_delay_t                         *p_os_delay;
#endif // End of OS_SUPPORTING
    /* The APIs from Core layer                        */
    const led_operations_t                   *p_led_opes;
    const time_base_t                       *p_time_base;

    /**************Target of enternal APIs**************/
    pf_led_control_t                    pf_led_controler;

} bsp_led_driver_t;

/**
 * @brief the constructor of bsp_led_driver_t.
 * 
 * Steps:
 * 1. Link the target of internal IOs.
 *  1-1. Add the Core layer's APIs into bsp_led_driver instance.
 *  1-2. Add the OS layer's APIs into bsp_led_driver instance.
 * 2. Initialize the target of enternal requirement.
 *  
 * @param[in] self        : Pointer to the target of handler.
 * @param[in] os_delay    : Pointer to the os_delay_interface.
 * @param[in] led_ops     : Pointer to the led_operations_interface.
 * @param[in] time_base   : Pointer to the time_base_interface.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
led_status_t led_driver_inst(
                                  bsp_led_driver_t   *const      self,
#ifdef OS_SUPPORTING
                            const os_delay_t         *const  os_delay,
#endif // End of OS_SUPPORTING
                            const led_operations_t   *const   led_ops,
                            const time_base_t        *const time_base
                            );
//******************************** Declaring ********************************//

#endif // End of __BSP_LED_DRIVER_H__

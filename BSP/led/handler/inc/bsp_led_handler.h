/******************************************************************************
 * Copyright (C) 2024 xxxxxx, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_handler.h
 *
 * @par dependencies
 * - stdio.h
 * - stdint.h
 *
 * @author ZhuangZhong | R&D Dept. | xxxxxx, Inc.(Gmbh)
 *
 * @brief BSP layer's Advanced LED handler for STM32F4xx
 *
 * Processing flow:
 *
 * call directly.
 *
 * @version V1.0 2025-03-05
 *
 * @note 1 tab == 4 spaces!
 *
 *****************************************************************************/

#ifndef __BSP_LED_HANDLER_H__
#define __BSP_LED_HANDLER_H__ 

//******************************** Includes *********************************//

#include "bsp_led_driver.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cmsis_os2.h>
//******************************** Includes *********************************//


//******************************** Defines **********************************//

#define OS_SUPPORTING               /* switch of enable OS supporting        */
#define FREERTOS_SUPPORTING         /* switch of enable FreeRTOS supporting  */
#define DEBUG_ON                    /* swtich of enable debug                */

#ifdef  DEBUG_ON
#define DEBUG_OUT(format, ...)  \
    printf(format, ##__VA_ARGS__)   /* Debug output macro                    */
#else
#define DEBUG_OUT(X)
#endif

/* Initialization pattern                */
#define INIT_PATTERN  (bsp_led_driver_t*)(0xA6A6A6A6)
                                    

typedef enum
{
    HANDLER_NOT_INITED     =    0,  /* Handler not initialized.              */
    HANDLER_INITED         =    1,  /* Handler initialized.                  */
} led_handler_init_t;

typedef enum
{
    LED_HANDLER_NO_1    =       0, /* Handler of the No1 led.                */
    LED_HANDLER_NO_2             , /* Handler of the No2 led.                */
    LED_HANDLER_NO_3             , /* Handler of the No3 led.                */
    LED_HANDLER_NO_4             , /* Handler of the No4 led.                */
    LED_HANDLER_NO_5             , /* Handler of the No5 led.                */
    LED_HANDLER_NO_6             , /* Handler of the No6 led.                */
    LED_HANDLER_NO_7             , /* Handler of the No7 led.                */
    LED_HANDLER_NO_8             , /* Handler of the No8 led.                */
    LED_HANDLER_NO_9             , /* Handler of the No9 led.                */
    MAX_INSTANCE_NUBER           , /* The max led intances number 
                                                      could be registered.   */
    LED_NOT_INITIALIZED = 0xFFFF
} led_index_t;

typedef enum
{
    HANDLER_OK             =    0,  /* Operation completed successfully.     */
    HANDLER_ERROR          =    1,  /* Run-time error without case matched   */
    HANDLER_ERRORTIMEOUT   =    2,  /* Operation failed with timeout         */
    HANDLER_ERRORRESOURCE  =    3,  /* Resource not available.               */
    HANDLER_ERRORPARAMETER =    4,  /* Parameter error.                      */
    HANDLER_ERRORNOMEMORY  =    5,  /* Out of memory.                        */
    HANDLER_ERRORISR       =    6,  /* Not allowed in ISR context            */
    HANDLER_STATUS_NUM           ,  /* Number of handler status              */
    HANDLER_RESERVED       = 0xFF,  /* Reserved                              */
} led_handler_status_t;
//******************************** Defines **********************************//


//******************************** Declaring ********************************//

typedef struct bsp_led_handler_s bsp_led_handler_t;

typedef struct
{
    led_index_t                 index;
    uint32_t            cycle_time_ms;
    uint32_t              blink_times;
    proportion_t    proportion_on_off;
} led_event_t;

#ifdef OS_SUPPORTING
typedef struct
{
    /* Function to delay in microsecond  */
    led_handler_status_t (*pf_os_delay_us)       (const uint32_t us); 
    /* Function to delay in milliseconds */
    led_handler_status_t (*pf_os_delay_ms)       (const uint32_t ms); 
    /* Function to delay in minute       */
    led_handler_status_t (*pf_os_delay_mm)       (const uint32_t mm); 
} handler_os_delay_t;

typedef struct
{
    /* enter critica.                    */
    led_handler_status_t (*pf_os_critical_enter)       (void); 
    /* exit critica.                     */
    led_handler_status_t (*pf_os_critical_exit )       (void); 

} handler_os_critical_t;

typedef struct
{
    /* OS queue create */
    led_handler_status_t (*pf_os_queue_create) (
                                            uint32_t const        item_num,
                                            uint32_t const       item_size,
                                            void   **const p_queue_handler
                                               );

    /* OS queue put    */
    led_handler_status_t (*pf_os_queue_put   ) (
                                            void    *const p_queue_handler,
                                            void    *const          p_item,
                                            uint32_t const         timeout
                                               );
    
    /* OS queue get    */
    led_handler_status_t (*pf_os_queue_get   ) (
                                            void    *const p_queue_handler,
                                            void    *const           p_meg,
                                            uint32_t const         timeout
                                               );

    /* OS queue delete */
    led_handler_status_t (*pf_os_queue_delete) (
                                            void    *const p_queue_handler
                                               );
} handler_os_queue_t;

typedef union
{
    struct
    {
        const char *const name;
        const uint16_t stack_depth;
        const osPriority_t priority;
    } freeRTOS_attribute;

} task_atrribute_t;

typedef void (*TaskFunction_t)( void * );

typedef struct
{
    /*
    I think the atrribute that the thread created should be packed to a structure.
    e.g. p_task_name, stack_depth, and priority.
    so that it could be used in different OS.
    */
    /* OS thread create */
    led_handler_status_t (*pf_os_thread_create) (
                                    TaskFunction_t   const p_task_code     ,
                            //   const char            *const p_task_name     ,
                            //   const uint16_t               stack_depth     ,
                                    void            *const p_task_arg      ,
                            //   const uint32_t               priority        ,
                              const void            *const p_task_attribute,
                                    void           **const p_thread_handler
                                                );
    /* OS thread delete */
    led_handler_status_t (*pf_os_thread_delete) (
                                            void    *const p_thread_handler
                                                );
} handler_os_thread_t;
#endif // End of OS_SUPPORTING

typedef struct
{
    /* Function to get the time base in microsecond  */
    led_handler_status_t (*pf_get_time_base_us) (uint32_t *const us);
    /* Function to get the time base in milliseconds */
    led_handler_status_t (*pf_get_time_base_ms) (uint32_t *const ms);
    /* Function to get the time base in minute       */
    led_handler_status_t (*pf_get_time_base_mm) (uint32_t *const mm);
} handler_time_base_t;

typedef struct
{
    bsp_led_driver_t    *p_led_instance_group[MAX_INSTANCE_NUBER];
    uint32_t                                   led_instance_count;
} instance_regiseted_t;

typedef led_handler_status_t (*pf_handler_led_control_t) (
                             bsp_led_handler_t *const             self,
                       const led_index_t                     led_index,
                       const uint32_t                    cycle_time_ms,
                       const uint32_t                      blink_times,
                       const proportion_t            proportion_on_off
                                                         );

typedef led_handler_status_t (*pf_led_register_t) (
                            bsp_led_handler_t *const      self,
                            bsp_led_driver_t  *const       led,
                            led_index_t       *const led_index
                                                  );

typedef struct bsp_led_handler_s
{
    /*****************Internal runtime data of Internal status***************/
    /* Target of Status                                */
    led_handler_init_t                         is_inited;
    /* Regiseted instances                             */
    instance_regiseted_t                       instances;
    /* OS queue's Handler                              */
    void                             *p_os_queue_handler;
    /* OS thread's Handler                             */
    void                            *p_os_thread_handler;

    /*****************Internal interfaces of the handler*********************/
#ifdef OS_SUPPORTING
    /* The APIs from OS layer                          */
    const handler_os_delay_t                 *p_os_delay;
    const handler_os_queue_t        *p_os_queue_instance;
    const handler_os_thread_t      *p_os_thread_instance;
    const handler_os_critical_t           *p_os_critical;
#endif // End of OS_SUPPORTING
    /* The APIs from Core layer                        */
    const handler_time_base_t               *p_time_base;

    /*****************External interfaces of the handler*********************/
    /* The API for AP                                  */
    pf_handler_led_control_t    pf_handler_led_controler;
    /* The API for internal led driver                 */
    pf_led_register_t                    pf_led_register;

} bsp_led_handler_t;

/**
 * @brief the constructor of bsp_led_handler_t.
 * 
 * Steps:
 * 1. mount the target of internal IOs.
 * 2. Init the led group of target.
 * 3. mount the enternal APIs.
 *  
 * @param[in] self        : Pointer to the target of the led handler.
 * @param[in] os_delay    : Pointer to the os_delay interface.
 * @param[in] os_queue    : Pointer to the os_queue interface.
 * @param[in] time_base   : Pointer to the time_base interface.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
led_handler_status_t led_handler_inst (
                                  bsp_led_handler_t     *const        self,
#ifdef OS_SUPPORTING
                            const handler_os_delay_t    *const    os_delay,
                            const handler_os_queue_t    *const    os_queue,
                            const handler_os_critical_t *const os_critical,
                            const handler_os_thread_t   *const   os_thread,
#endif // End of OS_SUPPORTING
                            const handler_time_base_t   *const   time_base
                                      );


//******************************** Declaring ********************************//

#endif // End of __BSP_LED_HANDLER_H__

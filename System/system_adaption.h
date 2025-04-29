/******************************************************************************
 * Copyright (C) 2024 xxxxxx, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file system_adaption.h
 *
 * @par dependencies
 * - bsp_led_driver.h
 * - bsp_led_handler.h
 *
 * @author ZhuangZhong | R&D Dept. | xxxxxx, Inc.(Gmbh)
 *
 * @brief intergrate all the resources of system layer.
 *
 * Processing flow:
 *
 * call directly.
 *
 * @version V1.0 2025-04-26
 *
 * @note 1 tab == 4 spaces!
 *
 *****************************************************************************/

#ifndef __SYSTEM_ADAPTION_H__
#define __SYSTEM_ADAPTION_H__

//******************************** Includes *********************************//

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

#include "bsp_led_driver.h"
#include "bsp_led_handler.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//
#define OS_SUPPORTING               /* switch of enable OS supporting        */

#define INIT_PATTERN_SYSTEM_ADAPTER  (uint32_t)0xAEEEAAEA

typedef enum
{
    SYSTEM_ADAPTER_OK             =    0, /* Operation completed successfully.  */
    SYSTEM_ADAPTER_ERROR          =    1, /* Run-time error without case matched*/
    SYSTEM_ADAPTER_ERRORTIMEOUT   =    2, /* Operation failed with timeout      */
    SYSTEM_ADAPTER_ERRORRESOURCE  =    3, /* Resource not available.            */
    SYSTEM_ADAPTER_ERRORPARAMETER =    4, /* Parameter error.                   */
    SYSTEM_ADAPTER_ERRORNOMEMORY  =    5, /* Out of memory.                     */
    SYSTEM_ADAPTER_ERRORISR       =    6, /* Not allowed in ISR context         */
    SYSTEM_ADAPTER_STATUS_NUM           , /* Number of led status               */
    SYSTEM_ADAPTER_RESERVED       = 0xFF, /* Reserved                           */
} system_adapter_status_t;

__attribute__((used, section(".bsp_target"))) 
static bsp_led_handler_t handler = {INIT_PATTERN_SYSTEM_ADAPTER};
__attribute__((used, section(".bsp_target"))) 
static bsp_led_driver_t      led = {INIT_PATTERN_SYSTEM_ADAPTER};
//******************************** Defines **********************************//
/**************unit test for led driver -- begin************/
/**
 * @brief  Simulated OS delay function for unit testing
 * @param[in]  ms : Delay duration in milliseconds
 * @retval LED_OK Always returns success status
 */
led_status_t os_delay_ms_zz (const uint32_t ms)
{
    DEBUG_OUT("Info: Enter os_delay_ms_zz!\r\n");
    printf("delay %d ms \r\n", ms);
    return LED_OK;
}

os_delay_t os_delay_ms = 
{
    .pf_os_delay_ms = os_delay_ms_zz,
};

/**
 * @brief  Simulated LED on function for unit testing
 * @retval LED_OK Always returns success status
 */
led_status_t led_on_zz (void) 
{
    DEBUG_OUT("Info: Enter led_on_zz!\r\n");
    return LED_OK;
}
/**
 * @brief  Simulated LED off function for unit testing
 * @retval LED_OK Always returns success status
 */
led_status_t led_off_zz (void) 
{
    DEBUG_OUT("Info: Enter led_off_zz!\r\n");
    return LED_OK;
}
led_operations_t led_ops = 
{
    .pf_led_on  = led_on_zz,
    .pf_led_off = led_off_zz,
};

/**
 * @brief  Simulated time base getter function for unit testing
 * @param[out] time_base Pointer to store the time base value
 * @retval LED_OK Always returns success status
 */
led_status_t get_time_base_ms_zz (uint32_t *const time_base)
{
    DEBUG_OUT("Info: Enter get_time_base_ms_zz!\r\n");

    *time_base = 1000;
    return LED_OK;
}
time_base_t time_base_ms = 
{
    .pf_get_time_base_ms = get_time_base_ms_zz,
};
/**************unit test for led driver -- end**************/

/**************unit test for led handler -- begin***********/
/**
 * @brief  Simulated OS delay function for handler unit testing
 * @param[in] delay_ms : Delay duration in milliseconds
 * @retval HANDLER_OK Always returns success status
 */
led_handler_status_t os_delay_ms_handler (const uint32_t delay_ms)
{
    DEBUG_OUT("Info: Enter os_delay_ms_handler!\r\n");
    return HANDLER_OK;
}

handler_os_delay_t os_delay_handler = 
{
    .pf_os_delay_ms = os_delay_ms_handler,
};

/* OS queue create */
led_handler_status_t os_queue_create_handler (
                                            uint32_t const        item_num,
                                            uint32_t const       item_size,
                                            void   **const p_queue_handler
                                             )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter os_queue_create_handler!\r\n");

    QueueHandle_t queue_handler = xQueueCreate(item_num, item_size);
    if ( NULL == queue_handler )
    {
        DEBUG_OUT("Error: Create queue failed!\r\n");
        ret = HANDLER_ERRORRESOURCE;
    }
    else 
    {
        *p_queue_handler = queue_handler;
    }
    return ret;
}

/* OS queue put    */
led_handler_status_t os_queue_put_handler  (
                                        void    *const p_queue_handler,
                                        void    *const          p_item,
                                        uint32_t const         timeout
                                           )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_queue_put!\r\n");

    if ( NULL == p_queue_handler  ||
         NULL == p_item           ||
         timeout > portMAX_DELAY
       )
    {
        DEBUG_OUT("Error: Invalid queue handler!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    if ( pdTRUE != xQueueSend( (QueueHandle_t)p_queue_handler,
                               (void *       )         p_item,
                               (TickType_t   )        timeout
                             )
       )
    {
        DEBUG_OUT("Error: Send queue failed!\r\n");
        ret = HANDLER_ERRORRESOURCE;
    }

    return ret;
}
/* OS queue get    */
led_handler_status_t os_queue_get_handler  (
                                        void    *const p_queue_handler,
                                        void    *const           p_meg,
                                        uint32_t const         timeout
                                           )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_queue_get!\r\n");

    if ( NULL == p_queue_handler  ||
         NULL == p_meg            ||
         timeout > portMAX_DELAY
       )
    {
        DEBUG_OUT("Error: Invalid queue handler!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    if ( pdTRUE != xQueueReceive( (QueueHandle_t)p_queue_handler,
                                  (void *       )         p_meg,
                                  (TickType_t   )        timeout
                                )
       )
    {
        DEBUG_OUT("Error: Receive queue failed!\r\n");
        ret = HANDLER_ERRORRESOURCE;
    }

    return ret;
}

/* OS queue delete */
led_handler_status_t os_queue_delete_handler (
                                        void    *const p_queue_handler
                                             )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_queue_delete!\r\n");

    if ( NULL == p_queue_handler )
    {
        DEBUG_OUT("Error: Invalid queue handler!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    vQueueDelete( (QueueHandle_t)p_queue_handler );

    return ret;
}
handler_os_queue_t os_queue_handler = 
{
    .pf_os_queue_create = os_queue_create_handler,
    .pf_os_queue_put    =    os_queue_put_handler,
    .pf_os_queue_get    =    os_queue_get_handler,
    .pf_os_queue_delete = os_queue_delete_handler,
};

/* enter critica.                    */
led_handler_status_t os_critical_enter       (void)
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_critical_enter!\r\n");

    vPortEnterCritical();

    return ret;
}
/* exit critica.                     */
led_handler_status_t os_critical_exit        (void)
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_critical_enter!\r\n");

    vPortExitCritical();

    return ret;
}
handler_os_critical_t os_critical_handler = 
{
    .pf_os_critical_enter = os_critical_enter,
    .pf_os_critical_exit  =  os_critical_exit,
};


/* OS thread create */
led_handler_status_t pf_os_thread_create (
                                TaskFunction_t   const p_task_code     ,
                                void            *const p_task_arg      ,
                          const void            *const p_task_attribute,
                                void           **const p_thread_handler
                                            )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_thread_create!\r\n");
    task_atrribute_t task_atrribute = 
    {
#ifdef FREERTOS_SUPPORTING
        .freeRTOS_attribute = 
        {
            .name        = "defaultTask_handler"          ,
            .stack_depth = 512 * 4                        ,
            .priority    = (osPriority_t) osPriorityNormal, 
/*
Question:
    1. why tasks are pass when priority is different with defaultTask?
    2. why tasks are locked when priority is not different with defaultTask?
Guess:
    1. Someone of tasks should be hungry.
*/
        }
#else
        0
#endif // end of FREERTOS_SUPPORTING
    };
    if ( NULL == p_task_code )
    {
        DEBUG_OUT("Error: Invalid task code!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    if ( NULL != p_task_attribute )
    {
        memcpy(&task_atrribute, p_task_attribute, sizeof(task_atrribute_t));
    }

#ifdef FREERTOS_SUPPORTING
    if ( pdPASS != xTaskCreate(
            (TaskFunction_t        )p_task_code                                  ,
            (const char           *)task_atrribute.freeRTOS_attribute.name       ,
            (configSTACK_DEPTH_TYPE)task_atrribute.freeRTOS_attribute.stack_depth,
            (void                 *)p_task_arg                                   ,
            (UBaseType_t           )task_atrribute.freeRTOS_attribute.priority   ,
            (TaskHandle_t         *)p_thread_handler                             )
       )
    {
        DEBUG_OUT("Error: Create thread failed!\r\n");
        ret = HANDLER_ERRORRESOURCE;
        return ret;
    }
#endif // end of FREERTOS_SUPPORTING

    DEBUG_OUT("Info: Create thread success!\r\n");
    return ret;
}
/* OS thread delete */
led_handler_status_t pf_os_thread_delete (
                                        void    *const p_thread_handler
                                         )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter pf_os_thread_delete!\r\n");
    if ( NULL == p_thread_handler )
    {
        DEBUG_OUT("Error: Invalid thread handler!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    vTaskDelete( (TaskHandle_t)p_thread_handler );
    
    DEBUG_OUT("Info: Delete thread success!\r\n");
    return ret;
}
handler_os_thread_t os_thread_handler = 
{
    .pf_os_thread_create = pf_os_thread_create,
    .pf_os_thread_delete = pf_os_thread_delete,
};

led_handler_status_t get_time_base_ms (uint32_t *const time_stamp)
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter get_time_base_ms!\r\n");

    if ( NULL == time_stamp )
    {
        DEBUG_OUT("Error: Parameter error!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    *time_stamp = HAL_GetTick();

    return ret;
}
handler_time_base_t time_base_handler = 
{
    .pf_get_time_base_ms = get_time_base_ms,
};
/**************unit test for led handler -- end*************/


//******************************** Declaring ********************************//

system_adapter_status_t system_adapter_init_resource(void);
//******************************** Declaring ********************************//
#endif // End of __SYSTEM_ADAPTION_H__
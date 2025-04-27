/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_led_driver.h"
#include "bsp_led_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t         defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name       = "defaultTask",
    .stack_size = 512 * 4, /* 128 words */
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/**********************unit test for led driver -- begin********************/
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

/**
 * @brief  Unit test for bsp_led_driver_t.
 * @param  None
 * @retval None
 */
void Test_led_driver (void)
{
    led_status_t ret = LED_OK;
    bsp_led_driver_t led_test1;
    bsp_led_driver_t led_test2;
    ret = led_driver_inst(&led_test1, 
                          &os_delay_ms, 
                          &led_ops, 
                          &time_base_ms);
    if ( LED_OK != ret )
    {
        DEBUG_OUT("Error: Construct led_test1 failed!\r\n");
    }
    else
    {
        DEBUG_OUT("Info: Construct led_test1 success!\r\n");
    }

    ret = led_driver_inst(&led_test2, 
                          &os_delay_ms, 
                          &led_ops, 
                          &time_base_ms);
    if ( LED_OK != ret )
    {
        DEBUG_OUT("Error: Construct led_test2 failed!\r\n");
    }
    else
    {
        DEBUG_OUT("Info: Construct led_test2 success!\r\n");
    }

    ret = led_test1.pf_led_controler(&led_test1, 5, 5, PROPORTION_ON_OFF_1_1);
    if ( LED_OK != ret )
    {
        DEBUG_OUT("Error: led_test1 execute led_control failed!\r\n");
    }
    else
    {
        DEBUG_OUT("Info: led_test1 Execute led_control success!\r\n");
    }

    ret = led_test2.pf_led_controler(&led_test2, 2, 5, PROPORTION_ON_OFF_1_1);
    if ( LED_OK != ret )
    {
        DEBUG_OUT("Error: led_test2 Execute led_control failed!\r\n");
    }
    else
    {
        DEBUG_OUT("Info: led_test2 Execute led_control success!\r\n");
    }
}
/**********************unit test for led driver -- end**********************/

/**********************unit test for led handler -- begin*******************/

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

/**
 * @brief  Unit test for bsp_led_handler_t.
 * @param  None
 * @retval None
 */
void Test_led_handler (void)
{
//******************************** Handler ********************************//
    DEBUG_OUT("Begin: --------- Test led handler inst -------------\r\n");
    led_handler_status_t ret = HANDLER_OK;
    bsp_led_handler_t handler1;
    ret = led_handler_inst(&handler1           , 
                           &os_delay_handler   , 
                           &os_queue_handler   , 
                           &os_critical_handler,
                           &os_thread_handler  ,
                           &time_base_handler );
    
    if ( HANDLER_OK != ret )
    {
        DEBUG_OUT("Error: Construct handler failed!\r\n");
        return;
    }
    DEBUG_OUT("End  : --------- Test led handler inst -------------\r\n\r\n");
//******************************** Driver *********************************//
    DEBUG_OUT("Begin: ---------- Test led driver inst -------------\r\n");
    bsp_led_driver_t led_test1;
    led_driver_inst(&led_test1, 
                    &os_delay_ms, 
                    &led_ops, 
                    &time_base_ms);

    bsp_led_driver_t led_test2;
    led_driver_inst(&led_test2, 
                    &os_delay_ms, 
                    &led_ops, 
                    &time_base_ms);

    bsp_led_driver_t led_test3;
    led_driver_inst(&led_test3, 
                    &os_delay_ms, 
                    &led_ops, 
                    &time_base_ms);

    bsp_led_driver_t led_test4;
    led_driver_inst(&led_test4, 
                    &os_delay_ms, 
                    &led_ops, 
                    &time_base_ms);

    DEBUG_OUT("End  : ---------- Test led driver inst -------------\r\n\r\n");
//**************************** Intergrated Test ***************************//
    DEBUG_OUT("Begin: --------- Test handler register -------------\r\n");
    // mov LED_NOT_INITIALIZED dword ptr [handler_index] -- 线程1
    led_index_t handler_index[4] = { LED_NOT_INITIALIZED };
    ret = handler1.pf_led_register(&handler1, &led_test1, &handler_index[0]);
    ret = handler1.pf_led_register(&handler1, &led_test2, &handler_index[1]);
    ret = handler1.pf_led_register(&handler1, &led_test3, &handler_index[2]);
    ret = handler1.pf_led_register(&handler1, &led_test4, &handler_index[3]);

    for (uint8_t i = 0; i < 4; i++)
    {
        printf("handler_index[%d] = [%d]\r\n", i, handler_index[i]);
    }

    if ( HANDLER_OK != ret )
    {
        DEBUG_OUT("Error: Test handler failed!\r\n");
        return;
    }

    DEBUG_OUT("End  : --------- Test handler register -------------\r\n\r\n");
//********************* The external API for AP ***************************//
    DEBUG_OUT("Begin: --------- Test handler controler ------------\r\n");
    // so handler_index[0] is invalid.
    handler1.pf_handler_led_controler(&handler1,
                                      handler_index[0], // 线程2, 优先于线程1执行
                                      5,
                                      5,
                                      PROPORTION_ON_OFF_1_1);
    for (;;)
    {
        handler1.pf_handler_led_controler(&handler1,
                                          handler_index[1],
                                          2,
                                          1,
                                          PROPORTION_ON_OFF_1_1);
        handler1.pf_handler_led_controler(&handler1,
                                          handler_index[2],
                                          1,
                                          1,
                                          PROPORTION_ON_OFF_1_2);
        handler1.pf_handler_led_controler(&handler1,
                                          handler_index[3],
                                          3,
                                          2,
                                          PROPORTION_ON_OFF_1_3);
        osDelay(3000);
    }

    DEBUG_OUT("End  : --------- Test handler controler ------------\r\n\r\n");
    DEBUG_OUT("Info: Test handler success!\r\n");

    while(1);
}

/**********************unit test for led handler -- begin*******************/

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, 
                                    NULL, 
                                    &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    /* unit test for led driver         */
    DEBUG_OUT("Info: StartDefaultTask!\r\n");
    // Test_led_driver();
    Test_led_handler();
    static uint32_t start_count = 0;

    for (;;)
    {
        start_count ++;
        osDelay(100);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/* USER CODE END Application */

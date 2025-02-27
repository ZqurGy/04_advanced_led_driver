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
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_led_driver.h"
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
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/*********************Link to bsp_driver_t's os_delay_ms_t*********************/
led_status_t os_delay_ms_zz (const uint32_t); /* my own's function delay ms */

os_delay_ms_t os_delay_ms = 
{
    .pf_os_delay_ms = os_delay_ms_zz,
};

/******************Link to bsp_led_driver_t's led_operations_t*****************/
led_status_t led_on_zz  (void); /* my own's function led on  */
led_status_t led_off_zz (void); /* my own's function led off */

led_operations_t led_ops = 
{
    .pf_led_on  = led_on_zz,
    .pf_led_off = led_off_zz,
};

/******************Link to bsp_led_driver_t's time_base_ms_t******************/
led_status_t get_time_base_ms_zz (uint32_t *const);

time_base_ms_t time_base_ms = 
{
    .pf_get_time_base_ms = get_time_base_ms_zz,
};

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
    printf("hello world \r\n");
    bsp_led_driver_t led_test1;
    led_driver_inst(&led_test1, &os_delay_ms, &led_ops, &time_base_ms);
    // const bsp_led_driver_t led_test2;
    // led_driver_inst(&led_test1, (const os_delay_ms_t*)&led_test2, (const led_operations_t*)&led_test2, (const time_base_ms_t*)&led_test2);
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

led_status_t os_delay_ms_zz (const uint32_t ms)
{
    DEBUG_OUT("Info: Enter os_delay_ms_zz!\n");
    printf("delay %d ms \r\n", ms);
    return LED_OK;
}

led_status_t led_on_zz (void) 
{
    DEBUG_OUT("Info: Enter led_on_zz!\n");
    return LED_OK;
}

led_status_t led_off_zz (void) 
{
    DEBUG_OUT("Info: Enter led_off_zz!\n");
    return LED_OK;
}

led_status_t get_time_base_ms_zz (uint32_t *const time_base)
{
    DEBUG_OUT("Info: Enter get_time_base_ms_zz!\n");

    *time_base = 1000;
    return LED_OK;
}
/* USER CODE END Application */

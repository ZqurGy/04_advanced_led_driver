/******************************************************************************
 * Copyright (C) 2024 xxxxxx, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_driver.h
 *
 * @par dependencies
 * - bsp_led_driver.h
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


//******************************** Includes *********************************//
#include "bsp_led_driver.h"
//******************************** Includes *********************************//


//******************************** Defines **********************************//

/**
 * @brief help function led_driver_inst to initialize.
 * 
 * Steps:
 *  
 * @param[in] self        : Pointer to the target of handler.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
static led_status_t led_driver_init (bsp_led_driver_t *const self)
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("Info: Enter led_driver_init!\n");

    if(NULL == self)
    {
        DEBUG_OUT("Error: Parameter error!\n");
        ret = LED_ERRORPARAMETER;
        goto EXIT;
    }

    self->p_led_opes_inst->pf_led_off();
    self->p_led_opes_inst->pf_led_on();
    self->p_os_delay_ms_inst->pf_os_delay_ms(0x5a5a5a5a);
    uint32_t time_base = 0;
    self->p_time_base_ms_inst->pf_get_time_base_ms(&time_base);
    printf("time_base = %d \r\n", time_base);

EXIT:
    return ret;
}

/**
 * @brief Instantiate the target of bsp_led_driver.
 * 
 * Steps:
 * 1. Link the target of internal IOs
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
led_status_t led_driver_inst (
                                  bsp_led_driver_t   *const      self,
#ifdef OS_SUPPORTING
                            const os_delay_ms_t      *const  os_delay,
#endif // End of OS_SUPPORTING
                            const led_operations_t   *const   led_ops,
                            const time_base_ms_t     *const time_base
                             )
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("Info: Enter led_driver_inst!\n");
    
/*************************1.Check the input parameter*************************/
    if (
        (NULL == self)      || 
#ifdef OS_SUPPORTING
        (NULL == os_delay)  ||
#endif // End of OS_SUPPORTING
        (NULL == led_ops)   || 
        (NULL == time_base) 
       )
    {
        DEBUG_OUT("Error: Parameter error!\n");
        ret = LED_ERRORPARAMETER;
        goto EXIT;
    } 

/****************************2.Check the Resources****************************/
    if (INITED == self->is_inited)
    {
        DEBUG_OUT("Error: The driver has been initialized!\n");
        ret = LED_ERRORRESOURCE;
        goto EXIT;
    }

/**********************3.Link the target of internal IOs**********************/
    self->p_os_delay_ms_inst  =  os_delay;
    self->p_led_opes_inst     =   led_ops;
    self->p_time_base_ms_inst = time_base;

/***************4.Initialize the target of enternal requirement***************/
    self->cycle_time_ms     =            0x5a5a5a5a;
    self->blink_times       =            0x5a5a5a5a;
    self->proportion_on_off = PROPORTION_ON_OFF_x_x;

    ret = led_driver_init(self);

    if(LED_OK != ret)
    {
        DEBUG_OUT("Error: led_driver_init failed!\n");

        self->p_os_delay_ms_inst  = NULL;
        self->p_led_opes_inst     = NULL;
        self->p_time_base_ms_inst = NULL;

        goto EXIT;
    }

    self->is_inited = INITED;
    DEBUG_OUT("Info: led_driver_inst success!\n");
/********************************goto : return********************************/
EXIT:
    return ret;
}

//******************************** Defines **********************************//

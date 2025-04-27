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
#ifndef OS_SUPPORTING
/**
 * @brief 
 * 
 * Steps:
 * 1. analyze the target's parameters.
 * 2. run the operation of led.
 *  
 * @param[in] self             : Pointer to the target of handler.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
static led_status_t led_blink(bsp_led_driver_t *const self) 
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("Info: Enter led_blink!\r\n");

    // 1. check if the target has been initialized.
    if (
        NULL     == self                  ||
        LED_NOT_INITED == self->is_inited
       )
    {
        DEBUG_OUT("Error: The driver has not been initialized!\r\n");
        ret = LED_ERRORRESOURCE;
        return ret;
    }

    {
        // 2. analyze the target's parameters.
        // 2-1. save target's parameters to local variables.
        uint32_t     cycle_time_local        =     self->cycle_time_ms;
        uint32_t     blink_times_total       =       self->blink_times;
        proportion_t proportion_on_off_local = self->proportion_on_off;
        
        // 2-2. calculate the proportion time of led on and off.
        uint32_t     led_toggle_time         =              0x5a5a5a5a;
        switch (proportion_on_off_local)
        {
            case PROPORTION_ON_OFF_1_1:
                led_toggle_time = cycle_time_local / 2;
                break;
            case PROPORTION_ON_OFF_1_2:
                led_toggle_time = cycle_time_local / 3;
                break;
            case PROPORTION_ON_OFF_1_3:
                led_toggle_time = cycle_time_local / 4;
                break;
            default:
                DEBUG_OUT("\
                    calculate the proportion time of led on and off failed!\r\n");
                ret = LED_ERRORPARAMETER;
                return ret;
        }

        // 3. run the operation of led.
        for (
             uint32_t blink_number = 0; 
             blink_number < blink_times_total; 
             ++ blink_number
            ) 
        {
            for (
                uint32_t start_cycyle_time = 0;
                start_cycyle_time < cycle_time_local;
                ++ start_cycyle_time
                )
            {
                if (start_cycyle_time >= led_toggle_time)
                {
                    self->p_led_opes->pf_led_off();
                }
                else
                {
                    self->p_led_opes->pf_led_on();
                }
            }
        }
    }

    return ret;
}
#endif // End of OS_SUPPORTING

#ifndef OS_SUPPORTING
/**
 * @brief Link led_control to the enternal APIs of target.
 * 
 * Steps:
 * 1. set external requirement values of target and do some actions.
 * 2. add the data in target.
 * 3. run the operation of target.
 *  
 * @param[in] self             : Pointer to the target of handler.
 * @param[in] cycle_time_ms    : The whole time of blink.
 * @param[in] blink_times      : The times of blink.
 * @param[in] proportion_on_off: The proportion ralationship of led on and off.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
static led_status_t led_control (
                              bsp_led_driver_t *const self             , 
                        const uint32_t                cycle_time_ms    , 
                        const uint32_t                blink_times      , 
                        const proportion_t            proportion_on_off 
                                )
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("Info: Enter led_control!\r\n");
    /******************0.check target status******************/
    // 0-1. check if the target has been initialized.
    if ( 
        NULL     == self                 ||
        LED_NOT_INITED == self->is_inited  
       )
    {
        // 0-2. if the target has not been initialized, return error.
        DEBUG_OUT("Error: The driver has not been initialized!\r\n");
        ret = LED_ERRORRESOURCE;
        return ret;
    }

    /***************1.Check the input parameter***************/
    // 1-1. check if they are valid values.
    if (
        (cycle_time_ms     <=  0                   ) ||
        (cycle_time_ms     >= 10000                ) ||
        (blink_times       <=  0                   ) ||
        (blink_times       >= 1000                 ) ||
        (proportion_on_off <  PROPORTION_ON_OFF_1_1) ||
        (proportion_on_off >= PROPORTION_ON_OFF_NUM)
       )
    {
        // 1-2. if the input parameter is invalid, return error.
        DEBUG_OUT("Error: Parameter error!\r\n");
        ret = LED_ERRORPARAMETER;
    }

    /****************2.add the data in target****************/
    self->cycle_time_ms     =     cycle_time_ms;
    self->blink_times       =       blink_times;
    self->proportion_on_off = proportion_on_off; 

    /*************3. run the operation of target**************/
    ret = led_blink(self);

    return ret;
}
#endif // End of OS_SUPPORTING

/**
 * @brief help constructor led_driver_inst to initialize led's Power-on status.
 * 
 * Steps:
 *  
 * @param[in] self   : Pointer to the target of handler.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
static led_status_t led_driver_init (bsp_led_driver_t *const self)
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("Info: Enter led_driver_init!\r\n");

    if (NULL == self)
    {
        DEBUG_OUT("Error: Parameter error!\r\n");
        ret = LED_ERRORPARAMETER;
        return ret;
    }

    self->p_led_opes->pf_led_off();
    // self->p_led_opes_inst->pf_led_on();
    self->p_os_delay->pf_os_delay_ms(0x5a5a5a5a);
    uint32_t time_base = 0;
    self->p_time_base->pf_get_time_base_ms(&time_base);
    printf("time_base = %d \r\n", time_base);

    return ret;
}

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
led_status_t led_driver_inst (
                                  bsp_led_driver_t   *const      self,
#ifdef OS_SUPPORTING
                            const os_delay_t      *const  os_delay,
#endif // End of OS_SUPPORTING
                            const led_operations_t   *const   led_ops,
                            const time_base_t     *const time_base
                             )
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("Info: Enter led_driver_inst!\r\n");
    
    /****************1.Check the input parameter**************/
    if (
        (NULL == self)      || 
#ifdef OS_SUPPORTING
        (NULL == os_delay)  ||
#endif // End of OS_SUPPORTING
        (NULL == led_ops)   || 
        (NULL == time_base) 
       )
    {
        DEBUG_OUT("Error: Parameter error!\r\n");
        ret = LED_ERRORPARAMETER;
        return ret;
    } 

    /******************2.Check the Resources******************/
    if (LED_INITED == self->is_inited)
    {
        DEBUG_OUT("Error: The driver has been initialized!\r\n");
        ret = LED_ERRORRESOURCE;
        return ret;
    }

    /************3.Link the target of internal IOs************/
#ifdef OS_SUPPORTING
    self->p_os_delay  =  os_delay;
#endif // End of OS_SUPPORTING
    self->p_led_opes     =   led_ops;
    self->p_time_base = time_base;

    /*****4.Initialize the target of enternal requirement*****/
    self->cycle_time_ms     =            0x5a5a5a5a;
    self->blink_times       =            0x5a5a5a5a;
    self->proportion_on_off = PROPORTION_ON_OFF_x_x;

    /**************5.Link the enternal APIs*******************/
#ifndef OS_SUPPORTING

    self->pf_led_controler = led_control;
#endif // End of OS_SUPPORTING
    ret = led_driver_init(self);

    if (LED_OK != ret)
    {
        DEBUG_OUT("Error: led_driver_init failed!\r\n");

        self->p_os_delay  = NULL;
        self->p_led_opes     = NULL;
        self->p_time_base = NULL;

        return ret;
    }

    self->is_inited = LED_INITED;
    DEBUG_OUT("Info: led_driver_inst success!\r\n");

    return ret;
}

//******************************** Defines **********************************//

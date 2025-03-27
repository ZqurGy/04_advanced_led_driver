/******************************************************************************
 * Copyright (C) 2024 xxxxxx, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file bsp_led_handler.c
 *
 * @par dependencies
 * - bsp_led_handler.h
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


//******************************** Includes *********************************//
#include "bsp_led_handler.h"
//******************************** Includes *********************************//


//******************************** Defines **********************************//
/**
 * @brief helper function to initialize led_instance array of handler.
 * 
 * @param[in] p_led_instance_group : led_instance array of handler.
 * @param[in] led_size             : the size of array.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
static led_handler_status_t __array_init(
                            bsp_led_driver_t *p_led_instance_group[],
                            uint32_t                        led_size
                                        )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter __array_init!\r\n");
    for (int led_number = 0; led_number < led_size; ++ led_number)
    {
        p_led_instance_group[led_number] = (bsp_led_driver_t *) INIT_PATTERN;
    }
    // TBD: check if memory is vaild.

    return ret;
}

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
static led_handler_status_t handler_led_control (
                                bsp_led_handler_t *const self             ,
                          const led_index_t              index            ,
                          const uint32_t                 cycle_time_ms    , 
                          const uint32_t                 blink_times      , 
                          const proportion_t             proportion_on_off 
                                                )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter handler_led_control!\r\n");
    /******************0.check target status******************/
    // 0-1. check if the point is valid.
    if ( NULL == self                           ||
         HANDLER_NOT_INITED == self->is_inited
       )
    {
        DEBUG_OUT("Error: The handler has not been initialized!\r\n");
        ret = HANDLER_ERRORRESOURCE;
        return ret;
    }

    /***************1.Check the input parameter***************/
    // 1-1. check if the led handler[index] is valid.
    if ( index < LED_HANDLER_NO_1                    ||
         index >= MAX_INSTANCE_NUBER                 ||
         index >= self->instances.led_instance_count
       )
    {
        DEBUG_OUT("Error: The led index is invalid!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    // 1-2. check if they are valid values.
    if (
        (cycle_time_ms     <=  0                   ) ||
        (cycle_time_ms     >= 10000                ) ||
        (blink_times       <=  0                   ) ||
        (blink_times       >= 1000                 ) ||
        (proportion_on_off <  PROPORTION_ON_OFF_1_1) ||
        (proportion_on_off >= PROPORTION_ON_OFF_NUM)
       )
    {
        DEBUG_OUT("Error: Parameter error!\r\n");
        ret = HANDLER_ERRORPARAMETER;
    }

    /***************2.Send event to LED queue*****************/
    // 2-1. create the event.
    DEBUG_OUT("Info: Create a event!\r\n");
    led_event_t led_event = 
    {
        .cycle_time_ms     = cycle_time_ms    ,
        .blink_times       = blink_times      ,
        .proportion_on_off = proportion_on_off,
    };
    // 2-2. send the event to the led queue.





    return ret;
}

/**
 * @brief register the target of led_driver_instance to the handler.
 * 
 * Steps:
 * 1. check the status of target.
 * 2. mount the led to handler.
 *  
 * @param[in]  self        : Pointer to the target of handler.
 * @param[in]  led         : Pointer to led which need to be mounted.
 * @param[out] index       : Pointer to handler of led.
 * 
 * @return led_handler_status_t : Status of the function.
 * 
 * */
static led_handler_status_t led_register (
                                bsp_led_handler_t *const  self,  
                                bsp_led_driver_t  *const   led, 
                                led_index_t       *const index 
                                         )
{
    led_handler_status_t ret = HANDLER_OK;
    /************1.check the status of target************/

    if ( NULL == self                           ||
         HANDLER_NOT_INITED == self->is_inited
       )
    {
        ret = HANDLER_ERRORRESOURCE;
        DEBUG_OUT("Error: The handler has not been initialized!\r\n");
        return ret;
    }

    if ( NULL == led                            ||
         LED_NOT_INITED == led->is_inited
       )
    {
        ret = HANDLER_ERRORRESOURCE;
        DEBUG_OUT("Error: The led has not been initialized!\r\n");
        return ret;
    }

    if ( NULL == index )
    {
        ret = HANDLER_ERRORPARAMETER;
        DEBUG_OUT("Error: Parameter error! index is null~~\r\n");
        return ret;
    }
    
    if (self->instances.led_instance_count >= MAX_INSTANCE_NUBER)
    {
        ret = HANDLER_ERRORNOMEMORY;
        DEBUG_OUT("Error: led_group is full! not mount led!\r\n");
        return ret;
    }

    /************2.mount led to handler******************/
#ifdef OS_SUPPORTING
    self->p_os_critical->pf_os_critical_enter();
#endif // End of OS_SUPPORTING
    DEBUG_OUT("Info: Enter os_critical\r\n");
    if (self->instances.led_instance_count < MAX_INSTANCE_NUBER)
    {
        self->instances.p_led_instance_group[                        \
                            self->instances.led_instance_count] = led;
        *index = self->instances.led_instance_count;
        self->instances.led_instance_count++;
    }
#ifdef OS_SUPPORTING
    self->p_os_critical->pf_os_critical_exit();
#endif // End of OS_SUPPORTING

    return ret;
}

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
#endif // End of OS_SUPPORTING
                            const handler_time_base_t   *const   time_base
                            )
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter led_handler_inst!\r\n");
    
    /****************1.Check the input parameter**************/
    if (
        (NULL == self)      || 
#ifdef OS_SUPPORTING
        (NULL == os_delay)  ||
        (NULL == os_queue)  ||
#endif // End of OS_SUPPORTING
        (NULL == time_base) 
       )
    {
        DEBUG_OUT("Error: Parameter error!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    } 

    /******************2.Check the Resources******************/
    if (HANDLER_NOT_INITED == self->is_inited)
    {
        DEBUG_OUT("Error: The driver has been initialized!\r\n");
        ret = HANDLER_ERRORRESOURCE;
        return ret;
    }

    /************3.mount the target of internal IOs***********/
#ifdef OS_SUPPORTING
    self->p_os_delay          =    os_delay;
    self->p_os_queue_instance =    os_queue;
    self->p_os_critical       = os_critical;
#endif // End of OS_SUPPORTING
    self->p_time_base         =   time_base;

    /************4.Init the led group of target***************/
    self->instances.led_instance_count =   LED_HANDLER_NO_1;
    ret = __array_init(self->instances.p_led_instance_group, 
                       MAX_INSTANCE_NUBER                 );
    if(HANDLER_OK != ret)
    {
        DEBUG_OUT("Error: __array_init failed!");
        DEBUG_OUT("Error HANDLER_ERRORNOMEMORY\r\n");
        return ret;
    }

    /**************5.mount the enternal APIs*******************/
    self->pf_handler_led_controler = handler_led_control;
    self->pf_led_register          =        led_register;

    if (HANDLER_OK != ret)
    {
        DEBUG_OUT("Info: led_handler_inst failed!\r\n");

#ifdef OS_SUPPORTING
        self->p_os_delay          = NULL;
        self->p_os_queue_instance = NULL;
        self->p_os_critical       = NULL;
#endif // End of OS_SUPPORTING
        self->p_time_base         = NULL;

        return ret;
    }

    self->is_inited = HANDLER_INITED;
    DEBUG_OUT("Info: led_handler_inst success!\r\n");

    return ret;
}

//******************************** Defines **********************************//

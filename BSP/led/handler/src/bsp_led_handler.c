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
#include "cmsis_os2.h"
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
        p_led_instance_group[led_number] = INIT_PATTERN;
    }
    // TBD: check if memory is vaild.

    return ret;
}

static led_handler_status_t led_blink(bsp_led_driver_t *const self) 
{
    led_handler_status_t ret = HANDLER_OK;
    DEBUG_OUT("Info: Enter led_blink!\r\n");

    // 1. check if the target has been initialized.
    if (
        NULL     == self                  ||
        LED_NOT_INITED == self->is_inited
       )
    {
        DEBUG_OUT("Error: The driver has not been initialized!\r\n");
        ret = HANDLER_ERRORRESOURCE;
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
                ret = HANDLER_ERRORPARAMETER;
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

static led_handler_status_t __event_process(
                            bsp_led_handler_t *const self,
                            led_event_t       *const p_msg
                                           )
{
    led_handler_status_t ret = HANDLER_OK;
    bsp_led_driver_t *p_led_instance = NULL;
    DEBUG_OUT("Info: Enter __event_process!\r\n");

    if ( NULL == self || NULL == p_msg )
    {
        DEBUG_OUT("Error: The handler or message is invalid!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    if ((p_msg->index < LED_HANDLER_NO_1                    ||
         p_msg->index >= self->instances.led_instance_count ||
         p_msg->index >= MAX_INSTANCE_NUBER
        )                                                   &&
         p_msg->index != LED_NOT_INITIALIZED 
       )
    {
        DEBUG_OUT("Error: The led index is invalid!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }
    
    if ( INIT_PATTERN == self->instances.p_led_instance_group[p_msg->index] )
    {
        DEBUG_OUT("Error: The led instance has not been initialized!\r\n");
        ret = HANDLER_ERRORRESOURCE;
        return ret;
    }
    DEBUG_OUT("Info: Cycle time = %d, Blink times = %d, Proportion = %d\r\n",
              p_msg->cycle_time_ms,
              p_msg->blink_times,
              p_msg->proportion_on_off);
    p_led_instance = self->instances.p_led_instance_group[p_msg->index];
    p_led_instance->cycle_time_ms     =     p_msg->cycle_time_ms;
    p_led_instance->blink_times       =       p_msg->blink_times;
    p_led_instance->proportion_on_off = p_msg->proportion_on_off;

    if ( HANDLER_OK != led_blink(
                    self->instances.p_led_instance_group[p_msg->index]
                                )
       )
    {
        DEBUG_OUT("Error: The led blink failed!\r\n");
        ret = HANDLER_ERRORRESOURCE;
        return ret;
    }
    DEBUG_OUT("Info: The led blink success!\r\n");
    
    return ret;
    
}
static void handler_start_thread ( void * p_task_arg)
{
    osDelay(2000);
    DEBUG_OUT("Info: Enter handler_start_thread!\r\n");
    /******************0.check target status******************/
    led_handler_status_t ret = HANDLER_OK;
    bsp_led_handler_t * p_led_handler = NULL;
    led_event_t         message       = {0U} ;
    static uint32_t thread_count      = 0   ;
    /***************1.Check the input parameter***************/
    if ( NULL != p_task_arg )
    {
        p_led_handler = (bsp_led_handler_t *) p_task_arg;
    }

    /***************2.Start first thread *********************/
    for (;;)
    {
        thread_count++;
        ret = p_led_handler->p_os_queue_instance->pf_os_queue_get(  
                                        p_led_handler->p_os_queue_handler,
                                        (void *)&message                 ,
                                        0
                                                                 );
        if ( HANDLER_OK == ret )
        {
            __event_process(p_led_handler, &message);
            DEBUG_OUT("Info: Get the message from the led queue!\r\n");
        }

        osDelay(1000);
    }

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
         index >= self->instances.led_instance_count ||
         index >= MAX_INSTANCE_NUBER
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
        DEBUG_OUT("Error: handler_led_control Parameter error!\r\n");
        ret = HANDLER_ERRORPARAMETER;
        return ret;
    }

    /***************2.Send event to LED queue*****************/
    // 2-1. create the event.
    DEBUG_OUT("Info: Create a event!\r\n");
    led_event_t led_event = 
    {
        .index             = index            ,
        .cycle_time_ms     = cycle_time_ms    ,
        .blink_times       = blink_times      ,
        .proportion_on_off = proportion_on_off,
    };
    // 2-2. send the event to the led queue.
    DEBUG_OUT("Info: Send the event to the led queue!\r\n");
    ret = self->p_os_queue_instance->pf_os_queue_put(self->p_os_queue_handler,
                                                     (void *)&led_event      ,
                                                     0                      );
    if (HANDLER_OK != ret)
    {
        DEBUG_OUT("Error: Send the event to the led queue failed!\r\n");
        return ret;
    }

    DEBUG_OUT("Info: led_control success!\r\n");
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
        DEBUG_OUT("Error: Parameter error! index is null!\r\n");
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
                            const handler_os_thread_t   *const   os_thread,
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
        (NULL == os_thread) ||
#endif // End of OS_SUPPORTING
        (NULL == time_base) 
       )
    {
        DEBUG_OUT("Error: led_handler_inst Parameter error!\r\n");
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

    /*****3.mount external API to internal IOs of objcet******/
#ifdef OS_SUPPORTING
    self->p_os_delay          =    os_delay;
    self->p_os_queue_instance =    os_queue;
    self->p_os_critical       = os_critical;
    self->p_os_thread_instance =  os_thread;
#endif // End of OS_SUPPORTING
    self->p_time_base         =   time_base;

    /************4.Init the led group of target***************/
#ifdef OS_SUPPORTING
    // 4.1 init os thread that will be used.
    /*
    TBD:
        I think there is one problem here, should create a thread pool, 
        instead of create a thread.
        And p_os_thread_handler only point to one thread.
    */
    ret = self->p_os_thread_instance->pf_os_thread_create(
                                           handler_start_thread       ,
                                           self                       ,
                                           NULL                       ,
                                         &(self->p_os_thread_handler));
    if (HANDLER_OK != ret)
    {
        DEBUG_OUT("Error: Create thread failed!\r\n");
        self->p_os_thread_instance->pf_os_thread_delete(self->p_os_thread_handler);
        return ret;
    }
    // 4.1 init os queue that will be used.
    ret = self->p_os_queue_instance->pf_os_queue_create(
                                         10                          ,
                                         sizeof(led_event_t)         ,
                                       &(self->p_os_queue_handler));
    if (HANDLER_OK != ret)
    {
        DEBUG_OUT("Error: Create queue failed!\r\n");
        self->p_os_queue_instance->pf_os_queue_delete(
                                            self->p_os_queue_handler);
        self->p_os_thread_instance->pf_os_thread_delete(
                                            self->p_os_thread_handler);
        return ret;
    }
#endif // End of OS_SUPPORTING
    // 4.2 init the led instance group.
    self->instances.led_instance_count =   LED_HANDLER_NO_1;
    ret = __array_init(self->instances.p_led_instance_group, 
                       MAX_INSTANCE_NUBER                 );

    /**************5.mount the enternal APIs*******************/
    self->pf_handler_led_controler = handler_led_control;
    self->pf_led_register          =        led_register;

    if (HANDLER_OK != ret)
    {
        DEBUG_OUT("Error: Init led instance group failed!\r\n");

#ifdef OS_SUPPORTING
        self->p_os_delay          = NULL;
        self->p_os_queue_instance = NULL;
        self->p_os_critical       = NULL;
        self->p_os_thread_instance->pf_os_thread_delete(self->p_os_thread_handler);
        self->p_os_queue_instance->pf_os_queue_delete(self->p_os_queue_handler);
#endif // End of OS_SUPPORTING
        self->p_time_base         = NULL;

        return ret;
    }

    self->is_inited = HANDLER_INITED;
    DEBUG_OUT("Info: led_handler_inst success!\r\n");

    return ret;
}

//******************************** Defines **********************************//

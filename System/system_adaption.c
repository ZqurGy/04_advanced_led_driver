/******************************************************************************
 * Copyright (C) 2024 xxxxxx, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file system_adaption.c
 *
 * @par dependencies
 * - system_adaption.h
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

//******************************** Includes *********************************//

#include "system_adaption.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

system_adapter_status_t system_adapter_init_resource(void)
{
    system_adapter_status_t ret = SYSTEM_ADAPTER_OK;
    DEBUG_OUT("Info: Enter system_adapter_init_resource!\r\n");

    ret = led_handler_inst(&handler            , 
                           &os_delay_handler   , 
                           &os_queue_handler   , 
                           &os_critical_handler,
                           &os_thread_handler  ,
                           &time_base_handler );
    if ( LED_OK != ret )
    {
        DEBUG_OUT("Error: Construct led_test1 failed!\r\n");
    }
    else
    {
        DEBUG_OUT("Info: Construct led_test1 success!\r\n");
    }
    ret = led_driver_inst(&led          , 
                          &os_delay_ms  , 
                          &led_ops      , 
                          &time_base_ms);
    if ( LED_OK != ret )
    {
        DEBUG_OUT("Error: Construct led_test2 failed!\r\n");
    }
    else
    {
        DEBUG_OUT("Info: Construct led_test2 success!\r\n");
    }
}

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
//******************************** Defines **********************************//
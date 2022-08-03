/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-27     XHH       the first version
 */

#include <rtdevice.h>
#include <board.h>

#define MOTOR GET_PIN(F,12)

extern  rt_event_t motor_event;


static void motor_task_entry(void *par)
{
    rt_uint32_t e;

    rt_pin_mode(MOTOR, PIN_MODE_OUTPUT);

    while(1)
    {
        if(rt_event_recv(motor_event, (1<<3), RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, RT_WAITING_NO, &e)==RT_EOK)
        {
            rt_pin_write(MOTOR, PIN_LOW);//控制继电器;
//            rt_kprintf("motor_on\r\n");
        }
        if(rt_event_recv(motor_event, (1<<2), RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, RT_WAITING_NO, &e)==RT_EOK)
        {
            rt_pin_write(MOTOR, PIN_HIGH);//控制继电器;
//            rt_kprintf("motor_off\r\n");
        }
        rt_thread_mdelay(200);
    }

}

void motor_task_create(rt_uint8_t priority,rt_uint32_t tick)
{
    rt_thread_t motor_task=RT_NULL;
    motor_task=rt_thread_create("motortask",
                                motor_task_entry,
                                RT_NULL,
                                512,
                                priority, tick);
    if(motor_task!=RT_NULL)
    {
        rt_thread_startup(motor_task);
    }
    else {
        rt_kprintf("motor_task_err\r\n");
    }

}

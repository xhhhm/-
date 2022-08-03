/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-25     XHH       the first version
 */


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
//
//#include "main.h"

#include "board.h"
#include "sensor.h"
#include "sensor_hc_sr04.h"


/* Modify this pin according to the actual wiring situation */
#define SR04_TRIG_PIN GET_PIN(G, 4)
#define SR04_ECHO_PIN GET_PIN(G, 3)

#define MOTOR GET_PIN(F,12)

//int sr04_read_distance_sample(void);
//int rt_hw_sr04_port(void);

rt_mq_t sr04_data_mq=RT_NULL;

static void sr04_read_distance_entry(void *parameter)
{
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data sensor_data;
    rt_size_t res;

    rt_pin_mode(MOTOR,PIN_MODE_OUTPUT);
    rt_pin_write(MOTOR, PIN_HIGH);

    struct rt_sensor_config cfg;
    rt_base_t pins[2] = {SR04_TRIG_PIN, SR04_ECHO_PIN};

    cfg.intf.dev_name = "timer1";
    cfg.intf.user_data = (void *)pins;
    rt_hw_sr04_init("sr04", &cfg);

    dev = rt_device_find(parameter);
    if (dev == RT_NULL) {
        rt_kprintf("Can't find device:%s\n", parameter);
        return;
    }

    if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        rt_kprintf("open device failed!\n");
        return;
    }
    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)100);

    while (1)
    {
        res = rt_device_read(dev, 0, &sensor_data, 1);
        if (res != 1)
        {
            rt_kprintf("read data failed!size is %d\n", res);
            rt_device_close(dev);
            return;
        }
        else
        {
//          rt_kprintf("distance:%3d.%dcm, timestamp:%5d\n", rt_mq_send  / 10, sensor_data.data.proximity % 10, sensor_data.timestamp);
//            rt_uint16_t send_data=()sensor_data.data.proximity
            rt_mq_send(sr04_data_mq,&sensor_data.data.proximity ,4);
        }
        rt_thread_mdelay(25);//这里用25ms检测一次，一次发送数据检测20次，减去最高两次和最低两次，后取平均
    }
}


int sr04_task_create(rt_uint8_t priority,rt_uint32_t tick)
{
    rt_thread_t sr04_thread;



    sr04_data_mq=rt_mq_create("sr_da_mq", 4, 10, RT_IPC_FLAG_PRIO);




    sr04_thread = rt_thread_create("sr04",
                                   sr04_read_distance_entry,
                                   "pr_sr04",
                                   1024,
                                   priority,
                                   tick);
    if (sr04_thread != RT_NULL)
    {
        rt_thread_startup(sr04_thread);
    }
    else {
        rt_kprintf("sr04_task_create_err\r\n");
    }

    return RT_EOK;
}



//INIT_APP_EXPORT(sr04_task_create);

//int sr04_read_distance_sample(void)
//{
//
//}
//INIT_APP_EXPORT(sr04_read_distance_sample);



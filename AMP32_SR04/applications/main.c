/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-20     Abbcc        first version
 */

    /*数据处理任务优先级应该低于数据采集任务优先级*/
    /*人机交互（非实时任务）设备例如键盘、串口控制台、显示屏任务优先级中低优先级*/
    /*和中断之间共享资源时（例如全局变量），必须禁用中断，哪怕只有一行代码*/
//    RT_IPC_FLAG_FIFO//非实时性
//    RT_IPC_FLAG_PRIO//有优先级（实时）
    /*
     * 线程-线程之间共享资源
     * 1、禁用调度器
     *
     * 2、互斥量
     * 相关api都不可以再中断中调用
     *
     *有界优先级（上一个任务执行完还需要的时间，）
     *
     *无界优先级（时间维度，和复杂维度，都无法预计）
     * */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
//
#include "main.h"

#define IWDG_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

static void idle_hook(void);
static int iwdg_sample(void);

/* defined the LED2 pin: PE6 */
#define LED2_PIN    GET_PIN(E, 6)

#define LED3_PIN    GET_PIN(E, 5)

int main(void)
{
    /* set LED2 pin mode to output */
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    rt_hw_i2c_init();
    sr04_task_create(18,20);//SR04超声波 水位传感器（控制水位高低）
    aht10_task_create(19,20);//ATH10模拟环境温度（水温过高自动加水降温）
//    //gpio控制继电器（电机）
    data_process_task_create(22,20);//数据处理线程
    creat_lcd_task_create(25,2);//LCD显示线程
    connect_aliyun_create(26,20);//连接阿里云线程
    motor_task_create(29,20);//电机控制线程


    iwdg_sample();//看门狗（死机重启）

    while (1)
    {
//        rt_pin_write(LED2_PIN, PIN_HIGH);
//        rt_thread_mdelay(500);
//        rt_pin_write(LED2_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}


static rt_device_t wdg_dev;         /* 看门狗设备句柄 */

static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    if(rt_pin_read(LED2_PIN)==PIN_LOW)
    {
        rt_pin_write(LED2_PIN, PIN_HIGH);
    }
    else {
        rt_pin_write(LED2_PIN, PIN_LOW);
    }
//    rt_kprintf("feed the dog!\n ");
}

static int iwdg_sample(void)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t timeout = 26;    /* 溢出时间 */
    char device_name[RT_NAME_MAX];

    /* 判断命令行参数是否给定了设备名称 */

    rt_strncpy(device_name, IWDG_DEVICE_NAME, RT_NAME_MAX);

    /* 根据设备名称查找看门狗设备，获取设备句柄 */

    wdg_dev = rt_device_find(device_name);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 设置看门狗溢出时间 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", device_name);
        return RT_ERROR;
    }

    rt_device_control(wdg_dev,RT_DEVICE_CTRL_WDT_START,RT_NULL);

    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);
    return ret;
}

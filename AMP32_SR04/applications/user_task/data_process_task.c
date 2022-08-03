/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-28     XHH       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define FIND_MAX(A,B) ((A>B)?A:B)
#define FIND_MIN(A,B) ((A<B)?A:B)

struct data
{
    rt_uint16_t max;
    rt_uint16_t min;
    rt_uint16_t mean;
};

extern rt_mq_t sr04_data_mq;
rt_event_t motor_event=RT_NULL;
rt_mutex_t motor_mutex=RT_NULL;

rt_mailbox_t distance_lcd_mb=RT_NULL;
rt_mailbox_t distance_mqtt_mb=RT_NULL;

static struct data data_process(rt_uint16_t *in_data)
{
    struct data ret_data={0,0,0};
    rt_uint32_t all_data=0;
    rt_uint16_t *p=in_data;

    ret_data.max=*p;
    ret_data.min=ret_data.max;
    for(int i=0;i<10;i++)
    {
//        rt_kprintf("p[%d]=%d\r\n",i,*p);//打开可以看见单个数据
        all_data+=*p;
        ret_data.max=FIND_MAX(ret_data.max,*p);
        ret_data.min=FIND_MIN(ret_data.min,*p);
        p=p+2;//注意地址偏移，四字节（指针是两字节的数据类型）
    }

    ret_data.mean=(all_data-ret_data.max-ret_data.min)/8;

//    ret_data.max=ret_data.min;
//    ret_data.min=mean_data;
//    rt_kprintf("max_data=%d\r\n",max_data);
//    rt_kprintf("all_data=%d\r\n",all_data);

    return ret_data;
}


static void data_task_entry(void)
{
    int  count=0;
    int send_yw=0;
    rt_bool_t motor_sign_on=1,motor_sign_off=1;

    uint32_t aht10_data[22]={0};//目前传输4字节，完全是浪费空间（后期改进）

    rt_uint16_t data_test[21]={1,2,4,2,5,5,4,2,2,4,3,2,3,9,3,4,5,5,3,5};//测试用数组
    struct data test_data={0,0};//测试用数据

    motor_event=rt_event_create("mo_entry", RT_IPC_FLAG_PRIO);
    distance_lcd_mb=rt_mb_create("dtlcdmb", 1, RT_IPC_FLAG_PRIO);
    distance_mqtt_mb=rt_mb_create("dtmqtmb", 1, RT_IPC_FLAG_PRIO);



    while(1)
    {
        send_yw++;
        rt_mq_recv(sr04_data_mq, &aht10_data[count], 4, 200);
//        rt_kprintf("MQ_DATA = %d mm\r\n",aht10_data[count]);
        count++;
        if(count>=10)
        {
            test_data=data_process(aht10_data);

            rt_mutex_take(motor_mutex, 400);
            if(test_data.mean>700)
            {
//                motor_sign_off=1;
//                if(motor_sign_on==1)
//                {
                    rt_event_send(motor_event, (1<<3));//开启电机
////                    rt_kprintf("motor_on\r\n");
//                    motor_sign_on=0;
//                }
            }

            if(test_data.mean<600)
            {
//                motor_sign_on=1;
//                if(motor_sign_off==1)
//                {
                    rt_event_send(motor_event, (1<<2));//关闭电机
//                    rt_kprintf("motor_off\r\n");
//                    motor_sign_off=0;
//                }
            }


            rt_mutex_release(motor_mutex);


//            rt_kprintf("test_data.max = %d mm\r\n",test_data.max);
//            rt_kprintf("test_data.mean = %d mm\r\n",test_data.mean);
            rt_mb_send(distance_lcd_mb, test_data.mean);
            if(send_yw%200==0)rt_mb_send(distance_mqtt_mb, test_data.mean);
//            rt_kprintf("test_data.min = %d mm\r\n",test_data.min);

            rt_thread_mdelay(100);
            count=0;
        }
    }
}

void data_process_task_create(rt_uint8_t priority,rt_uint32_t tick)
{
    rt_thread_t data_task=RT_NULL;

    motor_mutex = rt_mutex_create("mot_mut", RT_IPC_FLAG_PRIO);

    data_task=rt_thread_create("datatask",data_task_entry, RT_NULL, 2048, priority, tick);
    if(data_task!=RT_NULL)
    {
        rt_thread_startup(data_task);
    }
    else {
        rt_kprintf("data_task_err\r\n");
    }
}




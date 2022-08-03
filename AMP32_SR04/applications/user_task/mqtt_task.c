/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-24     XHH       the first version
 */


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "mqttclient.h"
#include "cJSON.h"
#include "stdio.h"


extern  rt_event_t motor_event;

#define float_len 10

extern rt_mailbox_t distance_mqtt_mb;
extern rt_mailbox_t aht10_mqtt_mb;

extern rt_mutex_t motor_mutex;

static void supports_full_hd(const char * const monitor)
{
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            rt_kprintf("Error before: %s\n", error_ptr);
        }
    }

    cJSON *params = cJSON_GetObjectItemCaseSensitive(monitor_json, "params");
    cJSON *set_par = cJSON_GetObjectItemCaseSensitive(params, "powerstate");
//    rt_kprintf("string = %s \r\n", set_par->string);//�ҵ���Ӧ�ַ���
//    rt_kprintf("string = %s \r\n", set_par->valuestring);//�ҵ���Ӧ�ַ���
//    char tx_buf[float_len];
//    sprintf(tx_buf,"%f",set_par->valuedouble);
//    rt_kprintf("float = %s \r\n", tx_buf);
//    rt_kprintf("int = %d \r\n", set_par->valueint);//�ҵ���Ӧ������

    rt_mutex_take(motor_mutex, 200);
    if(set_par->valueint==1)
    {
        rt_event_send(motor_event, (1<<3));
    }
    else
    {
        rt_event_send(motor_event, (1<<2));
    }
    rt_mutex_release(motor_mutex);

    cJSON *brightness = cJSON_GetObjectItemCaseSensitive(params, "brightness");
//    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, brightness->valueint*5000);

    cJSON_Delete(monitor_json);
}

static void topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
//    KAWAII_MQTT_LOG_I("-----------------------------------------------------------------------------------");
//    KAWAII_MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    supports_full_hd((char*)msg->message->payload);
//    KAWAII_MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

static void *mqtt_publish_thread(void *arg)
{

    static int a=1;
    int distance=0;
    int mqtt_temp=0;

    mqtt_client_t *client = (mqtt_client_t *)arg;
    char buf[256] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    mqtt_list_subscribe_topic(client);
    msg.payload = (void *) buf;
    msg.qos = 0;
    rt_mb_recv(distance_mqtt_mb, &distance, 10000);
    rt_mb_recv(aht10_mqtt_mb, &mqtt_temp, 10000);

    sprintf(buf,"{\"method\":\"/sys/a1eQc7Oixwo/PV7wTcHVInREV1Cw9Oll/thing/event/property/post\",\"id\":\"224317615\",\"params\":{\"temperature\":%d,\"liqlevel\":%d},\"version\":\"1.0.0\"}",mqtt_temp,distance);
    mqtt_publish(client,"/sys/a1eQc7Oixwo/PV7wTcHVInREV1Cw9Oll/thing/event/property/post", &msg);
//    rt_kputs(buf);
    return 0;
}


static void mqtt_connect_entry(void * par)
{
    mqtt_client_t *client = NULL;

    mqtt_log_init();
    client = mqtt_lease();
    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "a1eQc7Oixwo.iot-as-mqtt.cn-shanghai.aliyuncs.com");
    mqtt_set_client_id(client, "a1eQc7Oixwo.PV7wTcHVInREV1Cw9Oll|securemode=2,signmethod=hmacsha256,timestamp=1658716425054|");
    mqtt_set_user_name(client, "PV7wTcHVInREV1Cw9Oll&a1eQc7Oixwo");
    mqtt_set_password(client, "d6ea122b19ed525ac9bbe0bc8b391b3174714b3e39c88436bc793fd1f94d325b");
    mqtt_set_clean_session(client, 1);

    if(mqtt_connect(client)!=0)
    {
        rt_kprintf("mqtt_connect_err=%d\r\n",mqtt_connect(client));
    }

//    rt_thread_delay(10);

    mqtt_subscribe(client, "/sys/a1eQc7Oixwo/PV7wTcHVInREV1Cw9Oll/thing/service/property/set", QOS0, topic1_handler);
    while(1)
    {
       rt_thread_mdelay(10000);
       /*后期应加上事件驱动发布数据*/
       mqtt_publish_thread(client);//发布消息
    }
}


int connect_aliyun_create(rt_uint8_t priority,rt_uint32_t tick)
{
    rt_base_t retn=0;

    rt_thread_t  mqtt_connect=RT_NULL;

    mqtt_connect=rt_thread_create("mqt_ali",
                                    mqtt_connect_entry,
                                    RT_NULL,
                                    2048+512,
                                    priority, tick);

    if(mqtt_connect == RT_NULL)
    {
        rt_kprintf("connect_aliyun_task_err\r\n");
    }
    else {
        retn=rt_thread_startup(mqtt_connect);
    }
    return retn;
}


//MSH_CMD_EXPORT(connect_aliyun,connect_aliyun1);








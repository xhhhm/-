/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-25     XHH       the first version
 */
#ifndef APPLICATIONS_MAIN_H_
#define APPLICATIONS_MAIN_H_

#include "lcd/lcd_init.h"
#include "lcd/lcd.h"
#include "lcd/pic.h"



void creat_lcd_task_create(rt_uint8_t priority,rt_uint32_t tick);
int connect_aliyun_create(rt_uint8_t priority,rt_uint32_t tick);
void aht10_task_create(rt_uint8_t priority,rt_uint32_t tick);
int sr04_task_create(rt_uint8_t priority,rt_uint32_t tick);
void data_process_task_create(rt_uint8_t priority,rt_uint32_t tick);
void motor_task_create(rt_uint8_t priority,rt_uint32_t tick);


rt_uint16_t * data_process(rt_uint16_t *in_data);

#endif /* APPLICATIONS_MAIN_H_ */

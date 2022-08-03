/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-24     XHH       the first version
 */
#include "lcd/lcd.h"
#include "lcd/lcd_init.h"

extern rt_mailbox_t distance_lcd_mb;
extern rt_mailbox_t aht10_lcd_mb;

#define READ_MOTOR GET_PIN(F,12)

static void lcd_task_entry(void *par)
{
    LCD_Init();//LCD初始化
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    LCD_ShowString(24,0,"xhh water pump",RED,WHITE,16,0);
    LCD_ShowString(2,16,"dist:      mm",BLUE,WHITE,16,0);
    LCD_ShowString(2,32,"temp:      c",BLUE,WHITE,16,0);
    LCD_ShowString(2,48,"state: OFF",BLUE,WHITE,16,0);
//    int count=0;
    int distance=0;
    int lcd_temp=0;
    /*这里可以选择用数据队列将数据传输到此，用来显示（拷贝数据）*/

    while(1)
    {
        rt_mb_recv(distance_lcd_mb, &distance, 500);
        LCD_ShowIntNum(40,16,distance,5,ORANGE,WHITE,16);
        rt_mb_recv(aht10_lcd_mb, &lcd_temp, 500);
        LCD_ShowIntNum(40,32,lcd_temp,5,ORANGE,WHITE,16);

        if(rt_pin_read(READ_MOTOR)==PIN_HIGH)
        {
            LCD_ShowString(56,48,"OFF",BLUE,WHITE,16,0);
        }
        else {
            LCD_ShowString(56,48,"ON ",BLUE,WHITE,16,0);
        }
//        rt_thread_mdelay(500);
//        count++;

    }
}


void creat_lcd_task_create(rt_uint8_t priority,rt_uint32_t tick)
{
    rt_thread_t lcd_task = RT_NULL;
    lcd_task=rt_thread_create("lcd_task", lcd_task_entry, RT_NULL, 1024, priority, tick);

    if(lcd_task==RT_NULL)
    {
        rt_kprintf("lcd_task_err]r]n");
    }
    else {
        rt_thread_startup(lcd_task);
    }
}



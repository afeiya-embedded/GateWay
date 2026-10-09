/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-9-30      xf       the first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdbg.h>
#include <string.h>


#define LEDSYS_PIN      GET_PIN(A, 3)

#define THREAD_PRIORITY         25    //线程优先级
#define THREAD_TIMESLICE        5         //线程时间片

/* 静态线程参数定义 */
ALIGN(RT_ALIGN_SIZE)
static char ledsys_stack[256];         //线程栈内存空间 , 最小稳定的内存 256字节
static struct rt_thread ledsys;       //线程句柄


void ledsys_thread_entry(void *parameter)
{
    while(1)
    {
        rt_pin_write(LEDSYS_PIN, !rt_pin_read(LEDSYS_PIN));// 反转电平
        rt_thread_mdelay(1000);
    }
}


int ledsys_thread_init(void)
{
    rt_pin_mode(LEDSYS_PIN, PIN_MODE_OUTPUT);

    /* 创建线程：LEDSYS */
    rt_thread_init(&ledsys,                    //线程句柄
                   "LEDSYS",                   //线程名称
                   ledsys_thread_entry,        //线程入口函数
                   RT_NULL,                    //线程入口函数参数
                   ledsys_stack,               //线程栈起始地址
                   sizeof(ledsys_stack),       //线程栈大小
                   THREAD_PRIORITY - 1,      //优先级
                   THREAD_TIMESLICE);        //时间片
    rt_thread_startup(&ledsys);
    return RT_EOK ;
}

/* 开机自动启动 */
INIT_APP_EXPORT(ledsys_thread_init);




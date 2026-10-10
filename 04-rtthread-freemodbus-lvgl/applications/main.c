/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-9-28     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>  // 头文件一定要在 #define DBG_LVL DBG_LOG 之后 ， 否则不输出


int main(void)
{
    while (1)
    {
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}

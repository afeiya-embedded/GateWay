/*
 * Copyright (c) 2006-2026, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-10-07     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define APP_VERSION "1.1.0"

int main(void)
{
    int count = 1;
    LOG_D("APP Version: %s build %s\r\n", APP_VERSION, __DATE__);

    extern int http_tb_ota_init(void);
    http_tb_ota_init();

    while (count++)
    {
        //LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

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

extern int http_tb_ota_init(void);
extern void adc_device_init(void);
extern int mqtt_start(void);
extern int mqtt_telemetry(void);


#define APP_VERSION "1.1.0"

int main(void)
{
    int count = 1;
    adc_device_init();
    http_tb_ota_init();
    mqtt_start();


    LOG_D("APP Version: %s build %s\r\n", APP_VERSION, __DATE__);
    while (count++)
    {
        rt_thread_mdelay(5000);
        mqtt_telemetry();
    }

    return RT_EOK;
}


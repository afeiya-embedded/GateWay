/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-9-30     xf       the first version
 */


#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>


#include <lvgl.h>
#include "lv_port_indev.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include "events_init.h"

#define DBG_TAG "LVGL.app"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

//   ADC1
#define ADC_DEV_NAME        "adc1"                   /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     16                       /* ADC 通道 */

extern void lv_label_set_text(lv_obj_t * obj, const char * text);


lv_ui guider_ui; // 全局的UI结构体

rt_uint32_t cpu_value = 0 ;
rt_adc_device_t adc_dev; // adc 设备

static void adc_device_init(void)
{
    rt_err_t ret = RT_EOK;

    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
    }
    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
    if(ret == RT_EOK)
    {
        rt_kprintf("adc sample run success!  find %s device!\n", ADC_DEV_NAME);
    }
}

rt_uint32_t adc_read(void)
{
    // STM32F4温度计算公式
    // V_sense = (ADC_VALUE * VREF) / 4095
    // Temperature = ((V25 - V_sense) / Avg_Slope) + 25

    const float VREF = 3.3f;           // 参考电压
    const float V25 = 0.76f;           // 25°C时的电压
    const float Avg_Slope = 0.0025f;   // 平均斜率 (2.5mV/°C)
    rt_uint32_t adc_value = 0 ;
    float V_sense = 0 ;
    float temperature = 0 ;

    /* 读取采样值 */
    for (int i = 0; i < 10; ++i) {
        adc_value += rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
    }
    adc_value = adc_value / 10 ;

    V_sense = (adc_value * VREF) / 4095.0f;
    temperature = ((V_sense - V25) / Avg_Slope) + 25.0f;

    return  (rt_uint32_t)temperature ;

}


static void update_cpu_label_cb(lv_timer_t * timer)
{
    char data[10]={0};

    // 获取温度数据
    rt_sprintf(data, "%d", adc_read() );
    lv_label_set_text(guider_ui.screen_imgbtn_cpu_label, data);

    //rt_kprintf("update_cpu_label_cb\n");
}


void lv_user_gui_init(void)
{
    setup_ui(&guider_ui);
    events_init(&guider_ui);

    adc_device_init(); // 初始化adc
    lv_timer_create(update_cpu_label_cb, 2000, NULL);     // 在ADC线程中创建定时器

}

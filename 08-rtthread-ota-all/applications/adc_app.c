/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-10-08     xf       the first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#define DBG_TAG "adc.app"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

//   ADC1
#define ADC_DEV_NAME        "adc1"                   /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     16                       /* ADC 通道 */

rt_adc_device_t adc_dev; // adc 设备

void adc_device_init(void)
{
    rt_err_t ret = RT_EOK;

    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        LOG_E("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
    }
    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
    if(ret == RT_EOK)
    {
        LOG_I("adc sample run success!  find %s device!\n", ADC_DEV_NAME);
    }
}

float adc_read(void)
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

    return  temperature ;
}

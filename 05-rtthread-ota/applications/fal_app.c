/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-10-07     xf       the first version
 */


#include <rtthread.h>
#include <board.h>
#include <drv_spi.h>
#include <rtdef.h>
#include <fal.h> // 必须包含FAL头文件
#include <spi_flash.h>
#include "spi_flash_sfud.h"


rt_spi_flash_device_t w25q128; // 定义 w25q128 结构体

int spiflash_device_init(void)
{
    //注册SPI设备
    rt_hw_spi_device_attach("spi1","SPI1",GPIOG, GPIO_PIN_15);
    //挂载flash
    if((w25q128 = rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME,"SPI1"))== NULL)
    {
        rt_kprintf("Error! W25Q128FV NOT FOUND!");
    }
    return RT_EOK;
}

static int fal_app(void)
{
    spiflash_device_init();
    fal_init();
    return RT_EOK ;
}

/* 开机自动启动 */
INIT_DEVICE_EXPORT(fal_app);

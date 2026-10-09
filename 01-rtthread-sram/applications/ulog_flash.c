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
#include <drv_spi.h>
#include <rtdef.h>
#include <fal.h> // 必须包含FAL头文件
#include <spi_flash.h>
#include <dfs_file.h>
#include "easyflash.h"
#include "spi_flash_sfud.h"

rt_spi_flash_device_t  w25q128; // 定义 w25q128 结构体

#define   EASYFLASH         "eflash"
#define   FILE_SYSTEM      "filesys"

int spiflash_device_init(void)
{
    //注册SPI设备
    rt_hw_spi_device_attach("spi1","SPI1",GPIOG, GPIO_PIN_15);
    //挂载flash
    if((w25q128 = rt_sfud_flash_probe("norflash0","SPI1"))== NULL)
    {
        rt_kprintf("Error! W25Q128FV NOT FOUND!");
    }
    return RT_EOK;
}

int blk_device_init(void)
{
    struct rt_device *blk_dev = fal_blk_device_create(EASYFLASH);
    if (blk_dev == RT_NULL)
    {
        LOG_E("Failed to create block device on '%s' partition.",EASYFLASH);
        return -RT_ERROR;
    }
    LOG_I("Create block device on '%s' partition successful!",EASYFLASH);

    blk_dev = fal_blk_device_create(FILE_SYSTEM);
    if (blk_dev == RT_NULL)
    {
        LOG_E("Failed to create block device on '%s' partition.",FILE_SYSTEM);
        return -RT_ERROR;
    }
    LOG_I("Create block device on '%s' partition successful!",FILE_SYSTEM);
    return RT_EOK;
}


int mount_partition(void)
{
    struct rt_device *blk_dev = RT_NULL;

    blk_device_init();


    LOG_I("mount '%s' partition...",FILE_SYSTEM);

    blk_dev = rt_device_find(FILE_SYSTEM); // 查找名为  FILE_SYSTEM 的块设备
    if (blk_dev == RT_NULL)
    {
        LOG_E("Error: Block device 'filesys' not found. Mount aborted.");
        return -RT_ERROR;
    }

    /* 2. 创建挂载点目录 (如果不存在) */
    mkdir("/", 0)  ;

    /* 3. 尝试挂载文件系统 */
    if (dfs_mount(FILE_SYSTEM, "/", "elm", 0, 0) == RT_EOK)
    {
        LOG_I("'%s' partition mounted successfully.",FILE_SYSTEM);
        return RT_EOK;
    }

    /* 4. 挂载失败处理：尝试格式化后重新挂载 */
    LOG_D("Mount failed. Attempting to format the partition...");
    if (dfs_mkfs("elm", FILE_SYSTEM) == 0)
    {
        LOG_D("Format successful. Remounting...");
        if (dfs_mount(FILE_SYSTEM, "/", "elm", 0, 0) == RT_EOK)
        {
            LOG_D("'%s' partition mounted successfully after format.",FILE_SYSTEM);
            return RT_EOK;
        }
    }

    LOG_E("Error: Final mount attempt failed.");
    return -RT_ERROR;
}


static int ulog_easyflash(void)
{
    spiflash_device_init();
    rt_thread_mdelay(25);
    fal_init();
    easyflash_init();
    mount_partition();
    mkdir("/upan", 0 ); // 创建U盘的挂载点
    return RT_EOK ;
}

/* 开机自动启动 */
INIT_APP_EXPORT(ulog_easyflash);

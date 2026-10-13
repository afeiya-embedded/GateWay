/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtconfig.h>
#include <board.h>

#define NOR_FLASH_DEV_NAME             "norflash0"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32_onchip_flash_128k;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32_onchip_flash_128k,                                           \
    &nor_flash0,                                                     \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                  \
{                                                                                       \
    {FAL_PART_MAGIC_WORD, "app", "onchip_flash", 0, 896*1024, 0},                       \
    {FAL_PART_MAGIC_WORD, "download", NOR_FLASH_DEV_NAME, 0, 1024*1024, 0},             \
    {FAL_PART_MAGIC_WORD, "factory", NOR_FLASH_DEV_NAME, 1*1024*1024, 1024*1024, 0},    \
    {FAL_PART_MAGIC_WORD, "env", NOR_FLASH_DEV_NAME, 2*1024*1024, 1024*1024, 0},        \
    {FAL_PART_MAGIC_WORD, "filesys", NOR_FLASH_DEV_NAME, 3*1024*1024, 4*1024*1024, 0},  \
    {FAL_PART_MAGIC_WORD, "eflash", NOR_FLASH_DEV_NAME, 7*1024*1024, 9*1024*1024, 0},   \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */

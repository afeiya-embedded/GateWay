/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */




void custom_init(lv_ui *ui)
{
    /* Add your codes here */
        // 选项1：居中显示（最常用）
    lv_obj_align(ui->screen_imgbtn_tp_label, LV_ALIGN_BOTTOM_RIGHT, -21, -9);
    lv_obj_align(ui->screen_imgbtn_hum_label, LV_ALIGN_BOTTOM_RIGHT, -21, -9);
    lv_obj_align(ui->screen_imgbtn_vol_label, LV_ALIGN_BOTTOM_RIGHT, -21, -9);
}


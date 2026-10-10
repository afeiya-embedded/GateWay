/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-9-30     xf       the first version
 */

#include <lvgl.h>
#include "lv_port_indev.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include "events_init.h"

lv_ui guider_ui; // 全局的UI结构体

void lv_user_gui_init(void)
{
    setup_ui(&guider_ui);
    events_init(&guider_ui);
}


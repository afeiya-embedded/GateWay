/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#include <rtthread.h>
#include <board.h>
#include <user_mb_app.h>

#define DBG_TAG "event"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>  // 头文件一定要在 #define DBG_LVL DBG_LOG 之后 ， 否则不输出


#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


#define SLAVE_ADDR      MB_SAMPLE_TEST_SLAVE_ADDR

extern rt_mailbox_t modbus_mb ;            // 消息通知， 消息邮箱
extern __IO rt_uint8_t modbus_cmd_status;  // 命令执行的状态
extern USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];


static void screen_imgbtn_led1_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        // 获取当前状态（是否已经选中）
        bool current_state = lv_obj_has_state(obj, LV_STATE_CHECKED);
        // 期望的新状态是当前状态的取反
        bool new_state = !current_state;

        // 成功，则设置按钮为新状态
        if (new_state)
        {
            LOG_I("led1 on");
            if (rt_mb_send(modbus_mb, LED1ON) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }
        else
        {
            LOG_I("led1 off");
            if (rt_mb_send(modbus_mb, LED1OFF) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }

    }
}

static void screen_imgbtn_led2_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        // 获取当前状态（是否已经选中）
        bool current_state = lv_obj_has_state(obj, LV_STATE_CHECKED);
        // 期望的新状态是当前状态的取反
        bool new_state = !current_state;

        // 成功，则设置按钮为新状态
        if (new_state)
        {
            LOG_D("led2 on");
            if (rt_mb_send(modbus_mb, LED2ON) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }
        else
        {
            LOG_D("led2 off");
            if (rt_mb_send(modbus_mb, LED2OFF) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }

    }
}

static void screen_imgbtn_beep_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        // 获取当前状态（是否已经选中）
        bool current_state = lv_obj_has_state(obj, LV_STATE_CHECKED);
        // 期望的新状态是当前状态的取反
        bool new_state = !current_state;

        // 成功，则设置按钮为新状态
        if (new_state)
        {
            LOG_D("beep on");
            if (rt_mb_send(modbus_mb, BEEPON) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }
        else
        {
            LOG_D("beep off");
            if (rt_mb_send(modbus_mb, BEEPOFF) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }

    }
}

static void screen_imgbtn_relay_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        // 获取当前状态（是否已经选中）
        bool current_state = lv_obj_has_state(obj, LV_STATE_CHECKED);
        // 期望的新状态是当前状态的取反
        bool new_state = !current_state;

        // 成功，则设置按钮为新状态
        if (new_state)
        {
            LOG_D("relay on");
            if (rt_mb_send(modbus_mb, RELAYON) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }
        else
        {
            LOG_D("relay off");
            if (rt_mb_send(modbus_mb, RELAYOFF) != RT_EOK)
            {
                LOG_E("modbus mailbox is full");
            }
        }

    }
}

// 100ms 执行一次
static void update_device_cb(lv_timer_t * timer)
{
    static rt_uint8_t count1s = 0 ;
    char buf[10]={0};
    count1s  ++ ;
    if(count1s >=10) // 1秒时间到
    {
        count1s =  0  ;
        if (rt_mb_send(modbus_mb, SENSOR) != RT_EOK)
        {
            LOG_E("modbus mailbox is full");
        }

    }

    if(modbus_cmd_status == LED1ON)
    {
        // 设置LED1 按钮被按下
        lv_obj_add_state(guider_ui.screen_imgbtn_led1, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == LED1OFF)
    {
        // 设置LED1 按钮被抬起
        lv_obj_clear_state(guider_ui.screen_imgbtn_led1, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == LED2ON)
    {
        // 设置LED2 按钮被按下
        lv_obj_add_state(guider_ui.screen_imgbtn_led2, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == LED2OFF)
    {
        // 设置LED2 按钮被抬起
        lv_obj_clear_state(guider_ui.screen_imgbtn_led2, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == BEEPON)
    {
        // 设置BEEP 按钮被按下
        lv_obj_add_state(guider_ui.screen_imgbtn_beep, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == BEEPOFF)
    {
        // 设置BEEP 按钮被抬起
        lv_obj_clear_state(guider_ui.screen_imgbtn_beep, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == RELAYON)
    {
        // 设置RELAY 按钮被按下
        lv_obj_add_state(guider_ui.screen_imgbtn_relay, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == RELAYOFF)
    {
        // 设置RELAY 按钮被抬起
        lv_obj_clear_state(guider_ui.screen_imgbtn_relay, LV_STATE_CHECKED);
    }
    else if(modbus_cmd_status == SENSOR)
    {
        // 把都回来的传感器数据显示到控件上
        rt_sprintf(buf, "%d",usMRegHoldBuf[SLAVE_ADDR-1][1]/100) ;
        lv_label_set_text(guider_ui.screen_imgbtn_tp_label, buf);
        rt_sprintf(buf, "%d",usMRegHoldBuf[SLAVE_ADDR-1][2]/100) ;
        lv_label_set_text(guider_ui.screen_imgbtn_hum_label, buf);
        rt_sprintf(buf, "%.1f",usMRegHoldBuf[SLAVE_ADDR-1][3]/100.0) ;
        lv_label_set_text(guider_ui.screen_imgbtn_vol_label, buf);
        rt_sprintf(buf, "%.1f",usMRegHoldBuf[SLAVE_ADDR-1][4]/1000.0) ;
        lv_label_set_text(guider_ui.screen_imgbtn_cu_label, buf);
        rt_sprintf(buf, "%.1f",usMRegHoldBuf[SLAVE_ADDR-1][5]/1000.0) ;
        lv_label_set_text(guider_ui.screen_imgbtn_pw_label, buf);
    }

    modbus_cmd_status = 0 ; //  清空值
}

void events_init(lv_ui *ui)
{

    // 移除按钮的CHECKABLE标志，改为手动控制状态
    lv_obj_clear_flag(ui->screen_imgbtn_led1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_clear_flag(ui->screen_imgbtn_led2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_clear_flag(ui->screen_imgbtn_beep, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_clear_flag(ui->screen_imgbtn_relay, LV_OBJ_FLAG_CHECKABLE);

    // 添加事件监听
    lv_obj_add_event_cb(ui->screen_imgbtn_led1, screen_imgbtn_led1_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_imgbtn_led2, screen_imgbtn_led2_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_imgbtn_beep, screen_imgbtn_beep_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui->screen_imgbtn_relay, screen_imgbtn_relay_event_handler, LV_EVENT_CLICKED, NULL);

    // 创建定时器
    lv_timer_create(update_device_cb, 100, NULL);

    // 调整控件位置
    lv_obj_align(ui->screen_imgbtn_tp_label, LV_ALIGN_BOTTOM_RIGHT,  -24, -7);
    lv_obj_align(ui->screen_imgbtn_hum_label, LV_ALIGN_BOTTOM_RIGHT, -19, -7);
    lv_obj_align(ui->screen_imgbtn_vol_label, LV_ALIGN_BOTTOM_RIGHT, -17, -7);
    lv_obj_align(ui->screen_imgbtn_cu_label, LV_ALIGN_BOTTOM_RIGHT,  -17, -7);
    lv_obj_align(ui->screen_imgbtn_pw_label, LV_ALIGN_BOTTOM_RIGHT,  -18, -7);

}

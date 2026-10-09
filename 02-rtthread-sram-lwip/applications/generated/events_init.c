/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"


#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include <rtthread.h>
#include <board.h>

#define LED1_PIN        GET_PIN(A, 6)
#define LED2_PIN        GET_PIN(A, 4)
#define BEEP_PIN        GET_PIN(A, 0)


static void screen_btn_led1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        // 判断按钮的 checked 状态
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            //lv_obj_set_style_bg_color(guider_ui.screen_btn_led1, lv_color_hex(0x14db22), LV_PART_MAIN);
            // 按钮被选中（按下状态）
            rt_pin_write(LED1_PIN, PIN_LOW); // LED1 ON
        }
        else
        {
            //lv_obj_set_style_bg_color(guider_ui.screen_btn_led1, lv_color_hex(0x2195f6), LV_PART_MAIN);
            // 按钮未选中（释放状态）
            rt_pin_write(LED1_PIN, PIN_HIGH); // LED1 OFF
        }
    }
}

static void screen_btn_led2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        // 判断按钮的 checked 状态
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            //lv_obj_set_style_bg_color(guider_ui.screen_btn_led1, lv_color_hex(0x14db22), LV_PART_MAIN);
            // 按钮被选中（按下状态）
            rt_pin_write(LED2_PIN, PIN_LOW); // LED2 ON
        }
        else
        {
            //lv_obj_set_style_bg_color(guider_ui.screen_btn_led1, lv_color_hex(0x2195f6), LV_PART_MAIN);
            // 按钮未选中（释放状态）
            rt_pin_write(LED2_PIN, PIN_HIGH); // LED2 OFF
        }
    }
}


static void screen_imgbtn_beep_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED)
    {
        // 判断按钮的 checked 状态
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            // 按钮被选中（按下状态）
            rt_pin_write(BEEP_PIN, PIN_HIGH); // BEEP ON
        }
        else
        {
            // 按钮未选中（释放状态）
            rt_pin_write(BEEP_PIN, PIN_LOW ); // BEEP OFF
        }
    }
}

// 简化的直接控制函数
void handle_key_action(uint32_t key, bool pressed)
{
    lv_obj_t *target_btn = NULL;

    // 确定目标按钮
    if (key == LV_KEY_ENTER) {
        target_btn = guider_ui.screen_btn_ok;
    } else if (key == LV_KEY_ESC) {
        target_btn = guider_ui.screen_btn_esc;
    } else {
        return;
    }

    if (pressed)
    {
        // 按键按下 - 只设置按下状态和发送PRESSED事件
        lv_obj_add_state(target_btn, LV_STATE_PRESSED);
        lv_event_send(target_btn, LV_EVENT_PRESSED, NULL);

        // 设置按下时的颜色
        //lv_obj_set_style_bg_color(target_btn, lv_color_hex(0x14db22), LV_PART_MAIN);

    }
    else
    {
        // 按键释放 - 先发送RELEASED事件，然后清除按下状态
        lv_event_send(target_btn, LV_EVENT_RELEASED, NULL);
        lv_obj_clear_state(target_btn, LV_STATE_PRESSED);

        // 发送CLICKED事件（完整的按下→释放过程完成）
        lv_event_send(target_btn, LV_EVENT_CLICKED, NULL);

        // 设置释放时的颜色
        //lv_obj_set_style_bg_color(target_btn, lv_color_hex(0x2195f6), LV_PART_MAIN);
    }
}

void events_init_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_btn_led1, screen_btn_led1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_btn_led2, screen_btn_led2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_imgbtn_beep, screen_imgbtn_beep_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(BEEP_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED1_PIN, PIN_HIGH); // LED1 OFF
    rt_pin_write(LED2_PIN, PIN_HIGH); // LED2 OFF
    rt_pin_write(BEEP_PIN, PIN_LOW );  // BEEP OFF

}

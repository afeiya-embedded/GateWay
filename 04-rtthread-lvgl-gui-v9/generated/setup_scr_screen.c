/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x1a202c), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_imgbtn_led1
    ui->screen_imgbtn_led1 = lv_imagebutton_create(ui->screen);
    lv_obj_set_pos(ui->screen_imgbtn_led1, 16, 22);
    lv_obj_set_size(ui->screen_imgbtn_led1, 85, 50);
    lv_obj_add_flag(ui->screen_imgbtn_led1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(ui->screen_imgbtn_led1, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_imgbtn_led1, LV_IMAGEBUTTON_STATE_RELEASED, &_led1off_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_led1, LV_IMAGEBUTTON_STATE_PRESSED, &_led1on_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_led1, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_led1on_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_led1, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_led1off_ARGB8565_85x50, NULL, NULL);
    ui->screen_imgbtn_led1_label = lv_label_create(ui->screen_imgbtn_led1);
    lv_label_set_text(ui->screen_imgbtn_led1_label, "");
    lv_label_set_long_mode(ui->screen_imgbtn_led1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_imgbtn_led1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_imgbtn_led1, 0, LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_led1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_imgbtn_led1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_imgbtn_led1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_led1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_imgbtn_led1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_led1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_led1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_led1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_led1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_led1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_led1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_led1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_led1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_led1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_led1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_led1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_led1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_led1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_led1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_led1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_imgbtn_led1, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_led1, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_led1, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_imgbtn_led2
    ui->screen_imgbtn_led2 = lv_imagebutton_create(ui->screen);
    lv_obj_set_pos(ui->screen_imgbtn_led2, 117, 22);
    lv_obj_set_size(ui->screen_imgbtn_led2, 85, 50);
    lv_obj_add_flag(ui->screen_imgbtn_led2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(ui->screen_imgbtn_led2, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_imgbtn_led2, LV_IMAGEBUTTON_STATE_RELEASED, &_led2off_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_led2, LV_IMAGEBUTTON_STATE_PRESSED, &_led2on_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_led2, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_led2on_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_led2, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_led2off_ARGB8565_85x50, NULL, NULL);
    ui->screen_imgbtn_led2_label = lv_label_create(ui->screen_imgbtn_led2);
    lv_label_set_text(ui->screen_imgbtn_led2_label, "");
    lv_label_set_long_mode(ui->screen_imgbtn_led2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_imgbtn_led2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_imgbtn_led2, 0, LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_led2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_imgbtn_led2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_imgbtn_led2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_led2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_imgbtn_led2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_led2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_led2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_led2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_led2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_led2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_led2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_led2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_led2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_led2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_led2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_led2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_led2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_led2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_led2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_led2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_imgbtn_led2, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_led2, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_led2, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_imgbtn_beep
    ui->screen_imgbtn_beep = lv_imagebutton_create(ui->screen);
    lv_obj_set_pos(ui->screen_imgbtn_beep, 219, 22);
    lv_obj_set_size(ui->screen_imgbtn_beep, 85, 50);
    lv_obj_add_flag(ui->screen_imgbtn_beep, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(ui->screen_imgbtn_beep, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_imgbtn_beep, LV_IMAGEBUTTON_STATE_RELEASED, &_beepoff_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_beep, LV_IMAGEBUTTON_STATE_PRESSED, &_beepon_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_beep, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_beepon_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_beep, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_beepoff_ARGB8565_85x50, NULL, NULL);
    ui->screen_imgbtn_beep_label = lv_label_create(ui->screen_imgbtn_beep);
    lv_label_set_text(ui->screen_imgbtn_beep_label, "");
    lv_label_set_long_mode(ui->screen_imgbtn_beep_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_imgbtn_beep_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_imgbtn_beep, 0, LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_beep, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_imgbtn_beep, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_imgbtn_beep, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_beep, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_imgbtn_beep, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_beep, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_beep, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_beep, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_beep, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_beep, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_beep, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_beep, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_beep, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_beep, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_beep, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_beep, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_beep, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_beep, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_beep, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_beep, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_imgbtn_beep, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_beep, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_beep, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_imgbtn_relay
    ui->screen_imgbtn_relay = lv_imagebutton_create(ui->screen);
    lv_obj_set_pos(ui->screen_imgbtn_relay, 16, 94);
    lv_obj_set_size(ui->screen_imgbtn_relay, 85, 50);
    lv_obj_add_flag(ui->screen_imgbtn_relay, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(ui->screen_imgbtn_relay, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_imgbtn_relay, LV_IMAGEBUTTON_STATE_RELEASED, &_relayoff_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_relay, LV_IMAGEBUTTON_STATE_PRESSED, &_relayon_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_relay, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_relayon_ARGB8565_85x50, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_imgbtn_relay, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_relayoff_ARGB8565_85x50, NULL, NULL);
    ui->screen_imgbtn_relay_label = lv_label_create(ui->screen_imgbtn_relay);
    lv_label_set_text(ui->screen_imgbtn_relay_label, "");
    lv_label_set_long_mode(ui->screen_imgbtn_relay_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_imgbtn_relay_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_imgbtn_relay, 0, LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_relay, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_imgbtn_relay, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_imgbtn_relay, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_relay, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_imgbtn_relay, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_relay, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_imgbtn_relay, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_relay, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_relay, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_relay, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_relay, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_relay, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_relay, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_imgbtn_relay, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_relay, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_relay, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_imgbtn_relay, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_imgbtn_relay, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_imgbtn_relay, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_imgbtn_relay, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_imgbtn_relay, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_imgbtn_relay, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_imgbtn_relay, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_img_temp
    ui->screen_img_temp = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_img_temp, 117, 94);
    lv_obj_set_size(ui->screen_img_temp, 85, 50);
    lv_obj_add_flag(ui->screen_img_temp, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_temp, &_temp_ARGB8565_85x50);
    lv_image_set_pivot(ui->screen_img_temp, 50,50);
    lv_image_set_rotation(ui->screen_img_temp, 0);

    //Write style for screen_img_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_humi
    ui->screen_img_humi = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_img_humi, 219, 94);
    lv_obj_set_size(ui->screen_img_humi, 85, 50);
    lv_obj_add_flag(ui->screen_img_humi, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_humi, &_humi_ARGB8565_85x50);
    lv_image_set_pivot(ui->screen_img_humi, 50,50);
    lv_image_set_rotation(ui->screen_img_humi, 0);

    //Write style for screen_img_humi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_humi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_vol
    ui->screen_img_vol = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_img_vol, 16, 166);
    lv_obj_set_size(ui->screen_img_vol, 85, 50);
    lv_obj_add_flag(ui->screen_img_vol, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_vol, &_vol_ARGB8565_85x50);
    lv_image_set_pivot(ui->screen_img_vol, 50,50);
    lv_image_set_rotation(ui->screen_img_vol, 0);

    //Write style for screen_img_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_temp
    ui->screen_label_temp = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_label_temp, 150, 120);
    lv_obj_set_size(ui->screen_label_temp, 31, 15);
    lv_label_set_text(ui->screen_label_temp, "--");
    lv_label_set_long_mode(ui->screen_label_temp, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_temp, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_temp, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_humi
    ui->screen_label_humi = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_label_humi, 263, 120);
    lv_obj_set_size(ui->screen_label_humi, 21, 15);
    lv_label_set_text(ui->screen_label_humi, "--");
    lv_label_set_long_mode(ui->screen_label_humi, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_humi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_humi, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_humi, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_humi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_humi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_humi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_vol
    ui->screen_label_vol = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_label_vol, 52, 191);
    lv_obj_set_size(ui->screen_label_vol, 31, 15);
    lv_label_set_text(ui->screen_label_vol, "--");
    lv_label_set_long_mode(ui->screen_label_vol, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_vol, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_vol, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_vol, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_vol, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_vol, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_vol, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_cur
    ui->screen_img_cur = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_img_cur, 117, 166);
    lv_obj_set_size(ui->screen_img_cur, 85, 50);
    lv_obj_add_flag(ui->screen_img_cur, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_cur, &_cur_ARGB8565_85x50);
    lv_image_set_pivot(ui->screen_img_cur, 50,50);
    lv_image_set_rotation(ui->screen_img_cur, 0);

    //Write style for screen_img_cur, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_cur, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_pwr
    ui->screen_img_pwr = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_img_pwr, 219, 166);
    lv_obj_set_size(ui->screen_img_pwr, 85, 50);
    lv_obj_add_flag(ui->screen_img_pwr, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_pwr, &_pwr_ARGB8565_85x50);
    lv_image_set_pivot(ui->screen_img_pwr, 50,50);
    lv_image_set_rotation(ui->screen_img_pwr, 0);

    //Write style for screen_img_pwr, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_pwr, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_cur
    ui->screen_label_cur = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_label_cur, 158, 191);
    lv_obj_set_size(ui->screen_label_cur, 31, 15);
    lv_label_set_text(ui->screen_label_cur, "--");
    lv_label_set_long_mode(ui->screen_label_cur, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_cur, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_cur, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_cur, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_cur, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_cur, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_cur, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_label_pwr
    ui->screen_label_pwr = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_label_pwr, 259, 191);
    lv_obj_set_size(ui->screen_label_pwr, 31, 15);
    lv_label_set_text(ui->screen_label_pwr, "--");
    lv_label_set_long_mode(ui->screen_label_pwr, LV_LABEL_LONG_WRAP);

    //Write style for screen_label_pwr, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_pwr, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_pwr, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_pwr, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_pwr, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_pwr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen);

}

/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
#include <rtthread.h>
#include <board.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static bool touchpad_is_pressed(void);
//static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

//static void mouse_init(void);
//static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static bool mouse_is_pressed(void);
//static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);
//
//static void encoder_init(void);
//static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static void encoder_handler(void);
//
//static void button_init(void);
//static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static int8_t button_get_pressed_id(void);
//static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;

//static int32_t encoder_diff;
//static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

#define KEY_OK_PIN      GET_PIN(G, 8)
#define KEY_ESC_PIN     GET_PIN(A, 8)


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t tp_indev_drv;
    static lv_indev_drv_t key_indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&tp_indev_drv);
    tp_indev_drv.type = LV_INDEV_TYPE_POINTER;
    tp_indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&tp_indev_drv);

    /*------------------
     * Mouse
     * -----------------*/

//    /*Initialize your mouse if you have*/
//    mouse_init();
//
//    /*Register a mouse input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_POINTER;
//    indev_drv.read_cb = mouse_read;
//    indev_mouse = lv_indev_drv_register(&indev_drv);
//
//    /*Set cursor. For simplicity set a HOME symbol now.*/
//    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
//    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
//    lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&key_indev_drv);
    key_indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    key_indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&key_indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/

    /*------------------
     * Encoder
     * -----------------*/

//    /*Initialize your encoder if you have*/
//    encoder_init();
//
//    /*Register a encoder input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_ENCODER;
//    indev_drv.read_cb = encoder_read;
//    indev_encoder = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/

    /*------------------
     * Button
     * -----------------*/

//    /*Initialize your button if you have*/
//    button_init();
//
//    /*Register a button input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_BUTTON;
//    indev_drv.read_cb = button_read;
//    indev_button = lv_indev_drv_register(&indev_drv);
//
//    /*Assign buttons to points on the screen*/
//    static const lv_point_t btn_points[2] = {
//        {10, 10},   /*Button 0 -> x:10; y:10*/
//        {40, 100},  /*Button 1 -> x:40; y:100*/
//    };
//    lv_indev_set_button_points(indev_button, btn_points);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
    extern void XPT2046_Init(void) ;
    XPT2046_Init();
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    extern int XPT2046_Read(uint8_t type);
    extern uint16_t Xdown;
    extern uint16_t Ydown;      //触摸屏被按下就返回的的坐标值

    if ( XPT2046_Read(0) == 1) // 触摸屏被按下
    {
        last_x = Xdown;
        last_y = Ydown;
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_PR;
    }
    else  // 触摸屏被抬起
    {
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;
    }

//    static lv_coord_t last_x = 0;
//    static lv_coord_t last_y = 0;
//
//    /*Save the pressed coordinates and the state*/
//    if(touchpad_is_pressed()) {
//        touchpad_get_xy(&last_x, &last_y);
//        data->state = LV_INDEV_STATE_PR;
//    }
//    else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//
//    /*Set the last pressed coordinates*/
//    data->point.x = last_x;
//    data->point.y = last_y;
}

/*Return true is the touchpad is pressed*/
//static bool touchpad_is_pressed(void)
//{
//    /*Your code comes here*/
//
//    return false;
//}

/*Get the x and y coordinates if the touchpad is pressed*/
//static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
//{
//    /*Your code comes here*/
//
//    (*x) = 0;
//    (*y) = 0;
//}

/*------------------
 * Mouse
 * -----------------*/

/*Initialize your mouse*/
//static void mouse_init(void)
//{
//    /*Your code comes here*/
//}

/*Will be called by the library to read the mouse*/
//static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//    /*Get the current x and y coordinates*/
//    mouse_get_xy(&data->point.x, &data->point.y);
//
//    /*Get whether the mouse button is pressed or released*/
//    if(mouse_is_pressed()) {
//        data->state = LV_INDEV_STATE_PR;
//    }
//    else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//}

/*Return true is the mouse button is pressed*/
//static bool mouse_is_pressed(void)
//{
//    /*Your code comes here*/
//
//    return false;
//}

/*Get the x and y coordinates if the mouse is pressed*/
//static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
//{
//    /*Your code comes here*/
//
//    (*x) = 0;
//    (*y) = 0;
//}

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
    /* 把KEY引脚设置为输入 */
    rt_pin_mode(KEY_OK_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY_ESC_PIN, PIN_MODE_INPUT_PULLUP);
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    static bool was_pressed = false;  // 跟踪之前的状态
    extern void handle_key_action(uint32_t key, bool pressed);

    /*Get whether a key is pressed*/
    uint32_t act_key = keypad_get_key();
    if (act_key != 0)
    {
        // 按键按下状态
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters*/
        switch (act_key)
        {
        case 1:
            act_key = LV_KEY_ESC;
            break;
        case 2:
            act_key = LV_KEY_ENTER;
            break;
        }

        // 如果是新按下的按键（之前没有按下）
        if (!was_pressed)
        {
            last_key = act_key;
            handle_key_action(last_key, true);  // 发送按下事件
        }

        was_pressed = true;  // 更新状态
    }
    else
    {
        // 按键释放状态
        data->state = LV_INDEV_STATE_REL;

        // 如果之前是按下的，现在释放了
        if (was_pressed)
        {
            handle_key_action(last_key, false);  // 发送释放事件，使用last_key
            was_pressed = false;  // 重置状态
            last_key = 0;         // 可选：重置last_key
        }
    }

    data->key = last_key;

}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/
    if (rt_pin_read(KEY_ESC_PIN) == PIN_LOW)
        return 1;
    else if (rt_pin_read(KEY_OK_PIN) == PIN_LOW)
        return 2;

    return 0;
}

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
//static void encoder_init(void)
//{
//    /*Your code comes here*/
//}

/*Will be called by the library to read the encoder*/
//static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//
//    data->enc_diff = encoder_diff;
//    data->state = encoder_state;
//}

/*Call this function in an interrupt to process encoder events (turn, press)*/
//static void encoder_handler(void)
//{
//    /*Your code comes here*/
//
//    encoder_diff += 0;
//    encoder_state = LV_INDEV_STATE_REL;
//}

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
//static void button_init(void)
//{
//    /*Your code comes here*/
//}

/*Will be called by the library to read the button*/
//static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//
//    static uint8_t last_btn = 0;
//
//    /*Get the pressed button's ID*/
//    int8_t btn_act = button_get_pressed_id();
//
//    if(btn_act >= 0) {
//        data->state = LV_INDEV_STATE_PR;
//        last_btn = btn_act;
//    }
//    else {
//        data->state = LV_INDEV_STATE_REL;
//    }
//
//    /*Save the last pressed button's ID*/
//    data->btn_id = last_btn;
//}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
//static int8_t button_get_pressed_id(void)
//{
//    uint8_t i;
//
//    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
//    for(i = 0; i < 2; i++) {
//        /*Return the pressed button's ID*/
//        if(button_is_pressed(i)) {
//            return i;
//        }
//    }
//
//    /*No button pressed*/
//    return -1;
//}

/*Test if `id` button is pressed or not*/
//static bool button_is_pressed(uint8_t id)
//{
//
//    /*Your code comes here*/
//
//    return false;
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

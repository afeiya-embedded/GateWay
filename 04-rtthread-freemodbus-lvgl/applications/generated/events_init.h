/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


#ifndef EVENTS_INIT_H_
#define EVENTS_INIT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"


#define  LED1ON     1
#define  LED1OFF    2
#define  LED2ON     3
#define  LED2OFF    4
#define  BEEPON     5
#define  BEEPOFF    6
#define  RELAYON    7
#define  RELAYOFF   8
#define  SENSOR     9


void events_init(lv_ui *ui);

#endif

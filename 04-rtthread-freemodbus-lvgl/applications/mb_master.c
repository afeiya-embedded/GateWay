/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-21     flybreak     first version
 */

#include <rtthread.h>
#include "mb.h"
#include "mb_m.h"
#include "events_init.h"


#include <board.h>
#define DBG_TAG "modbus"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>  // 头文件一定要在 #define DBG_LVL DBG_LOG 之后 ， 否则不输出



#ifdef PKG_MODBUS_MASTER_SAMPLE
#define SLAVE_ADDR      MB_SAMPLE_TEST_SLAVE_ADDR
#define PORT_NUM        MB_MASTER_USING_PORT_NUM
#define PORT_BAUDRATE   MB_MASTER_USING_PORT_BAUDRATE
#else
#define SLAVE_ADDR      0x01
#define PORT_NUM        3
#define PORT_BAUDRATE   115200
#endif
#define PORT_PARITY     MB_PAR_NONE

#define MB_POLL_THREAD_PRIORITY  10
#define MB_SEND_THREAD_PRIORITY  RT_THREAD_PRIORITY_MAX - 1

#define MB_SEND_REG_START  0
#define MB_SEND_REG_NUM    5

#define MB_POLL_CYCLE_MS   50


rt_mailbox_t modbus_mb;        // 消息通知， 消息邮箱
rt_ubase_t modbus_cmd;         // 消息的内容
USHORT modbus_sendcmd;               // modbus从机控制指令
__IO rt_uint8_t modbus_cmd_status;  // 命令执行的状态


static void send_thread_entry(void *parameter)
{
    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;

    // 解决第一次通信失败的处理， 先发送一包数据
    eMBMasterReqReadHoldingRegister(SLAVE_ADDR,MB_SEND_REG_START,1,1000); // 读设备

    while (1)
    {

        if( rt_mb_recv(modbus_mb, &modbus_cmd,RT_WAITING_FOREVER)  == RT_EOK)
        {
            LOG_D("modbus_cmd=%d",modbus_cmd) ;
            if(modbus_cmd == LED1ON)
            {
                modbus_sendcmd = modbus_sendcmd | (0x1 << 0) ;
                error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                       MB_SEND_REG_START,   /* register start address */
                                       modbus_sendcmd,      /* data to be written */
                                       1000);               /* timeout */

                LOG_D("error_code=%d",error_code);
                /* Record the number of errors */
                if (error_code == MB_MRE_NO_ERR)
                {
                    modbus_cmd_status = LED1ON;
                }
                else
                {
                    LOG_I("modbus slave %d not found",SLAVE_ADDR);
                }

            }
            else if(modbus_cmd == LED1OFF)
            {
                modbus_sendcmd = modbus_sendcmd &  (~(0x1 << 0)) ;
                error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                       MB_SEND_REG_START,   /* register start address */
                                       modbus_sendcmd,      /* data to be written */
                                       1000);               /* timeout */

                LOG_I("error_code=%d",error_code);
                /* Record the number of errors */
                if (error_code == MB_MRE_NO_ERR)
                {
                    modbus_cmd_status = LED1OFF;
                }
                else
                {
                    LOG_I("modbus slave %d not found",SLAVE_ADDR);
                }

            }
            else if(modbus_cmd == LED2ON)
            {
                modbus_sendcmd = modbus_sendcmd | (0x1 << 1) ;
                error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                       MB_SEND_REG_START,   /* register start address */
                                       modbus_sendcmd,      /* data to be written */
                                       1000);               /* timeout */

                LOG_D("error_code=%d",error_code);
                /* Record the number of errors */
                if (error_code == MB_MRE_NO_ERR)
                {
                    modbus_cmd_status = LED2ON;
                }
                else
                {
                    LOG_I("modbus slave %d not found",SLAVE_ADDR);
                }

            }
            else if(modbus_cmd == LED2OFF)
             {
                 modbus_sendcmd = modbus_sendcmd &  (~(0x1 << 1)) ;
                 error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                        MB_SEND_REG_START,   /* register start address */
                                        modbus_sendcmd,      /* data to be written */
                                        1000);               /* timeout */

                 LOG_I("error_code=%d",error_code);
                 /* Record the number of errors */
                 if (error_code == MB_MRE_NO_ERR)
                 {
                     modbus_cmd_status = LED2OFF;
                 }
                 else
                 {
                     LOG_I("modbus slave %d not found",SLAVE_ADDR);
                 }

             }
            else if(modbus_cmd == BEEPON)
            {
                modbus_sendcmd = modbus_sendcmd | (0x1 << 2) ;
                error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                       MB_SEND_REG_START,   /* register start address */
                                       modbus_sendcmd,      /* data to be written */
                                       1000);               /* timeout */

                LOG_D("error_code=%d",error_code);
                /* Record the number of errors */
                if (error_code == MB_MRE_NO_ERR)
                {
                    modbus_cmd_status = BEEPON;
                }
                else
                {
                    LOG_I("modbus slave %d not found",SLAVE_ADDR);
                }

            }
            else if(modbus_cmd == BEEPOFF)
             {
                 modbus_sendcmd = modbus_sendcmd &  (~(0x1 << 2)) ;
                 error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                        MB_SEND_REG_START,   /* register start address */
                                        modbus_sendcmd,      /* data to be written */
                                        1000);               /* timeout */

                 LOG_I("error_code=%d",error_code);
                 /* Record the number of errors */
                 if (error_code == MB_MRE_NO_ERR)
                 {
                     modbus_cmd_status = BEEPOFF;
                 }
                 else
                 {
                     LOG_I("modbus slave %d not found",SLAVE_ADDR);
                 }

             }
            else if(modbus_cmd == RELAYON)
            {
                modbus_sendcmd = modbus_sendcmd | (0x1 << 3) ;
                error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                       MB_SEND_REG_START,   /* register start address */
                                       modbus_sendcmd,      /* data to be written */
                                       1000);               /* timeout */

                LOG_D("error_code=%d",error_code);
                /* Record the number of errors */
                if (error_code == MB_MRE_NO_ERR)
                {
                    modbus_cmd_status = RELAYON;
                }
                else
                {
                    LOG_I("modbus slave %d not found",SLAVE_ADDR);
                }

            }
            else if(modbus_cmd == RELAYOFF)
             {
                 modbus_sendcmd = modbus_sendcmd &  (~(0x1 << 3)) ;
                 error_code = eMBMasterReqWriteHoldingRegister(SLAVE_ADDR,  /* salve address */
                                        MB_SEND_REG_START,   /* register start address */
                                        modbus_sendcmd,      /* data to be written */
                                        1000);               /* timeout */

                 LOG_I("error_code=%d",error_code);
                 /* Record the number of errors */
                 if (error_code == MB_MRE_NO_ERR)
                 {
                     modbus_cmd_status = RELAYOFF;
                 }
                 else
                 {
                     LOG_I("modbus slave %d not found",SLAVE_ADDR);
                 }

             }
            else if(modbus_cmd == SENSOR)
             {
                 error_code = eMBMasterReqReadHoldingRegister(SLAVE_ADDR,  /* salve address */
                                        MB_SEND_REG_START+1,MB_SEND_REG_NUM,1000);

                 LOG_I("error_code=%d",error_code);
                 /* Record the number of errors */
                 if (error_code == MB_MRE_NO_ERR)
                 {
                     modbus_cmd_status = SENSOR;
                 }
                 else
                 {
                     LOG_I("modbus slave %d not found",SLAVE_ADDR);
                 }

             }

        }



    }
}

static void mb_master_poll(void *parameter)
{
    eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    eMBMasterEnable();

    // 动态创建邮箱
    modbus_mb = rt_mb_create("modbus", 8, RT_IPC_FLAG_FIFO);
    if(modbus_mb == RT_NULL)
    {
        LOG_E("rt_mb_create failed! ...") ;
    }

    while (1)
    {
        eMBMasterPoll();
        rt_thread_mdelay(MB_POLL_CYCLE_MS);
    }
}

static int mb_master_sample(void)
{
    static rt_uint8_t is_init = 0;
    rt_thread_t tid1 = RT_NULL, tid2 = RT_NULL;

    if (is_init > 0)
    {
        rt_kprintf("sample is running\n");
        return -RT_ERROR;
    }
    tid1 = rt_thread_create("md_m_poll", mb_master_poll, RT_NULL, 1024, MB_POLL_THREAD_PRIORITY-4, 10);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
    }
    else
    {
        goto __exit;
    }

    tid2 = rt_thread_create("md_m_send", send_thread_entry, RT_NULL, 1024, MB_SEND_THREAD_PRIORITY - 2, 10);
    if (tid2 != RT_NULL)
    {
        rt_thread_startup(tid2);
    }
    else
    {
        goto __exit;
    }

    is_init = 1;
    return RT_EOK;

__exit:
    if (tid1)
        rt_thread_delete(tid1);
    if (tid2)
        rt_thread_delete(tid2);

    return -RT_ERROR;
}
INIT_ENV_EXPORT(mb_master_sample);

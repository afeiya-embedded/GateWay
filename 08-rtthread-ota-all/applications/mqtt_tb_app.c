#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>
#include <paho_mqtt.h>
#include <cJSON.h>
#include <easyflash.h>
#include <rthw.h>
#include <board.h>

#define DBG_TAG             "mqtt.app"
//#define DBG_LVL             DBG_LOG
#define DBG_LVL             DBG_INFO
#include <rtdbg.h>

#define LED1_PIN        GET_PIN(A, 6)
#define LED2_PIN        GET_PIN(A, 4)
#define BEEP_PIN        GET_PIN(A, 0)


/**
 * ThingsBoard MQTT 服务器配置
 */
#define THINGSBOARD_SERVER    "tcp://xthings.cloud:1883"
#define DEVICE_TOKEN          "123456789"

/* ThingsBoard MQTT 主题定义 */
// 下行主题 - 接收服务器RPC调用
#define TB_RPC_REQUEST_TOPIC     "v1/devices/me/rpc/request/+"
// 下行主题 - 接收属性更新
#define TB_ATTRIBUTES_TOPIC      "v1/devices/me/attributes"

// 上行主题 - 遥测数据上传
#define TB_TELEMETRY_TOPIC       "v1/devices/me/telemetry"
// 上行主题 - RPC响应
#define TB_RPC_RESPONSE_TOPIC    "v1/devices/me/rpc/response/"

/* 全局变量 */
static MQTTClient tb_client;
static int is_started = 0;
static int initial_attributes_published = 0;  /* 初始属性是否已发布 */
static char device_token[100] = DEVICE_TOKEN;
static char device_url[64] = THINGSBOARD_SERVER;

/* 设备状态结构体 */
typedef struct {
    char led1_status[10];
    char led2_status[10];
    char beep_status[10];
    float cpu;

} device_status_t;

static device_status_t device_status = {0};

/* 函数声明 */
static int tb_publish_telemetry(MQTTClient *c, device_status_t *status);
static int tb_publish_attributes(MQTTClient *c, device_status_t *status);

/**
 * ThingsBoard RPC请求回调函数
 */
static void tb_rpc_callback(MQTTClient *c, MessageData *msg_data)
{
    /* 安全地复制 payload 并添加字符串结束符 */
    char payload_buf[512] = {0};
    int payload_len = msg_data->message->payloadlen;
    if (payload_len >= sizeof(payload_buf)) {
        payload_len = sizeof(payload_buf) - 1;
    }
    rt_memcpy(payload_buf, msg_data->message->payload, payload_len);
    payload_buf[payload_len] = '\0';
    char *payload = payload_buf;

    /* 提取主题中的ID */
    char topic_str[128] = {0};
    int topic_len = msg_data->topicName->lenstring.len;
    if (topic_len >= sizeof(topic_str)) {
        topic_len = sizeof(topic_str) - 1;
    }
    rt_memcpy(topic_str, msg_data->topicName->lenstring.data, topic_len);
    topic_str[topic_len] = '\0';

    LOG_I("RPC Request - Topic: %s, Payload: %s", topic_str, payload);

    /* 从主题中提取ID (格式: v1/devices/me/rpc/request/40) */
    char *last_slash = strrchr(topic_str, '/');
    int request_id = 0;
    if (last_slash != NULL) {
        request_id = atoi(last_slash + 1);
        LOG_D("Extracted RPC request ID: %d", request_id);
    } else {
        LOG_W("Failed to extract RPC request ID from topic: %s", topic_str);
    }

    cJSON *root = cJSON_Parse(payload);
    if (root == NULL) {
        LOG_E("Failed to parse RPC JSON");
        return;
    }

    cJSON *method = cJSON_GetObjectItem(root, "method");
    cJSON *params = cJSON_GetObjectItem(root, "params");

    if (method == NULL ) {
        LOG_E("RPC message missing required fields (method)");
        cJSON_Delete(root);
        return;
    }

    if (!cJSON_IsString(method)) {
        LOG_E("RPC method is not a string");
        cJSON_Delete(root);
        return;
    }

    /* 组成响应主题 */
    char response_topic[100];
    rt_snprintf(response_topic, sizeof(response_topic), "%s%d", TB_RPC_RESPONSE_TOPIC, request_id);

    if (strcmp(method->valuestring, "led1Set") == 0)
    {
        char response[100];
        if (params != NULL) {
            int led_state = cJSON_IsTrue(params) ? 1 : 0;
            /* PIN_HIGH = OFF, PIN_LOW = ON (硬件逻辑反转) */
            rt_pin_write(LED1_PIN, led_state ? PIN_LOW : PIN_HIGH);
            
            rt_strcpy(device_status.led1_status, led_state ? "true" : "false");
            LOG_I("LED1 set to: %s", device_status.led1_status);
            
            rt_snprintf(response, sizeof(response), "{\"led1Status\":\"%s\"}", device_status.led1_status);
        } else {
            LOG_W("led1Set: params is NULL");
            rt_snprintf(response, sizeof(response), "{\"error\":\"params is required\"}");
        }

        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);

    } else if (strcmp(method->valuestring, "led1Status") == 0)
    {
        char response[100];
        /* PIN_HIGH = OFF(false), PIN_LOW = ON(true) (硬件逻辑反转) */
        if(rt_pin_read(LED1_PIN) == PIN_HIGH){
            rt_strcpy(device_status.led1_status, "false");
        } else {
            rt_strcpy(device_status.led1_status, "true");
        }
        rt_snprintf(response, sizeof(response), "{\"led1Status\":\"%s\"}", device_status.led1_status);

        LOG_I("LED1 Status: %s", device_status.led1_status);
        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);

    } else if (strcmp(method->valuestring, "led2Set") == 0)
    {
        char response[100];
        if (params != NULL) {
            int led_state = cJSON_IsTrue(params) ? 1 : 0;
            /* PIN_HIGH = OFF, PIN_LOW = ON (硬件逻辑反转) */
            rt_pin_write(LED2_PIN, led_state ? PIN_LOW : PIN_HIGH);
            
            rt_strcpy(device_status.led2_status, led_state ? "true" : "false");
            LOG_I("LED2 set to: %s", device_status.led2_status);
            
            rt_snprintf(response, sizeof(response), "{\"led2Status\":\"%s\"}", device_status.led2_status);
        } else {
            LOG_W("led2Set: params is NULL");
            rt_snprintf(response, sizeof(response), "{\"error\":\"params is required\"}");
        }

        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);

    } else if (strcmp(method->valuestring, "led2Status") == 0)
    {
        char response[100];
        /* PIN_HIGH = OFF(false), PIN_LOW = ON(true) (硬件逻辑反转) */
        if(rt_pin_read(LED2_PIN) == PIN_HIGH){
            rt_strcpy(device_status.led2_status, "false");
        } else {
            rt_strcpy(device_status.led2_status, "true");
        }
        rt_snprintf(response, sizeof(response), "{\"led2Status\":\"%s\"}", device_status.led2_status);

        LOG_I("LED2 Status: %s", device_status.led2_status);
        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);

    } else if (strcmp(method->valuestring, "beepSet") == 0)
    {
        char response[100];
        if (params != NULL) {
            int beep_state = cJSON_IsTrue(params) ? 1 : 0;
            rt_pin_write(BEEP_PIN, beep_state ? PIN_HIGH : PIN_LOW);
            
            rt_strcpy(device_status.beep_status, beep_state ? "true" : "false");
            LOG_I("Beep set to: %s", device_status.beep_status);
            
            rt_snprintf(response, sizeof(response), "{\"beepStatus\":\"%s\"}", device_status.beep_status);
        } else {
            LOG_W("beepSet: params is NULL");
            rt_snprintf(response, sizeof(response), "{\"error\":\"params is required\"}");
        }

        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);

    } else if (strcmp(method->valuestring, "beepStatus") == 0)
    {
        char response[100];
        if(rt_pin_read(BEEP_PIN) == PIN_LOW){
            rt_strcpy(device_status.beep_status, "false");
        } else {
            rt_strcpy(device_status.beep_status, "true");
        }
        rt_snprintf(response, sizeof(response), "{\"beepStatus\":\"%s\"}", device_status.beep_status);

        LOG_I("Beep Status: %s", device_status.beep_status);
        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);

    }  else {
        LOG_W("Unknown RPC method: %s", method->valuestring);
        char response[150];
        rt_snprintf(response, sizeof(response), "{\"error\":\"Unknown method: %s\"}", method->valuestring);
        LOG_I("Response Topic: %s, Response: %s", response_topic, response);
        paho_mqtt_publish(c, QOS1, response_topic, response);
    }

    cJSON_Delete(root);
}

/**
 * ThingsBoard 属性更新回调函数
 */
static void tb_attributes_callback(MQTTClient *c, MessageData *msg_data)
{
    char *payload = (char *)msg_data->message->payload;
    payload[msg_data->message->payloadlen] = '\0';

    LOG_I("Topic: %.*s, Payload: %s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          payload);

    cJSON *root = cJSON_Parse(payload);
    if (root == NULL) {
        LOG_E("Failed to parse attributes JSON");
        return;
    }
    cJSON *fw_title = cJSON_GetObjectItem(root, "fw_title");
    if (fw_title == NULL ) {
        LOG_E("RPC message missing required fields (fw_title)");
        cJSON_Delete(root);
        return;
    }
    if (strcmp(fw_title->valuestring, "STM32F407V2") == 0)
    {
        char response[150];
        LOG_I("OTA update requested attributes");

        /* 设置环境变量并保存到 Flash */
        ef_set_and_save_env("tb_ota_update", "true");

        /* 发送响应，表示已收到请求 */
        rt_snprintf(response, sizeof(response),
                "{\"status\":\"accepted\",\"message\":\"OTA update will start after reboot\"}");
        LOG_I("Response Topic: %s, Response: %s", TB_TELEMETRY_TOPIC, response);
        paho_mqtt_publish(c, QOS1, TB_TELEMETRY_TOPIC, response);

        /* 延迟一下确保响应已发送 */
        rt_thread_mdelay(1000);

        /* 重启系统，系统启动时会自动检查环境变量并执行OTA更新 */
        LOG_I("System will reboot in 2 seconds to start OTA update...");
        rt_thread_mdelay(2000);

        extern void rt_hw_cpu_reset(void);
        rt_hw_cpu_reset();

    }
    cJSON_Delete(root);
}

/**
 * 默认回调函数
 */
static void tb_default_callback(MQTTClient *c, MessageData *msg_data)
{
    char *payload = (char *)msg_data->message->payload;
    payload[msg_data->message->payloadlen] = '\0';

    LOG_I("Default callback - Topic: %.*s, Payload: %s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          payload);


}

/**
 * MQTT连接成功回调
 */
static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_I("Connected to ThingsBoard MQTT broker");
}

/**
 * MQTT在线回调
 */
static void mqtt_online_callback(MQTTClient *c)
{
    LOG_I("MQTT client online");
    
    /* MQTT完全连接后，发布初始属性 */
    if (!initial_attributes_published) {
        rt_thread_mdelay(500);  /* 稍等片刻确保订阅完成 */
        if (tb_publish_attributes(c, &device_status) == 0) {
            LOG_I("Initial attributes published");
            initial_attributes_published = 1;
        } else {
            LOG_W("Failed to publish initial attributes");
        }
    }
}

/**
 * MQTT离线回调
 */
static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_I("MQTT client offline");
    initial_attributes_published = 0;  /* 重置标志，重连后重新发布初始属性 */
}

/**
 * 上传设备遥测数据 - 只上传CPU温度
 */
static int tb_publish_telemetry(MQTTClient *c, device_status_t *status)
{
    extern float adc_read(void) ;
    char telemetry[100];
    status->cpu = adc_read();
    rt_snprintf(telemetry, sizeof(telemetry),"{\"CPU\":%.1f}",status->cpu);
    return paho_mqtt_publish(c, QOS1, TB_TELEMETRY_TOPIC, telemetry);
}

/**
 * 更新设备属性
 */
static int tb_publish_attributes(MQTTClient *c, device_status_t *status)
{
    char attributes[150];
    rt_snprintf(attributes, sizeof(attributes),
            "{\"firmware_version\":\"1.0\",\"status\":\"online\",\"led1\":%d,\"led2\":%d,\"beep\":%d}",
            status->led1_status, status->led2_status, status->beep_status);

    return paho_mqtt_publish(c, QOS1, "v1/devices/me/attributes", attributes);
}

/**
 * 启动ThingsBoard MQTT客户端
 */
int mqtt_start(void)
{
#define  TB_MQTT_SERVER     "tb_mqtt_server"
#define  TB_TOKEN           "tb_token"



    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char client_id[50] = {0};

    if (is_started) {
        LOG_E("MQTT client is already connected.");
        return -1;
    }


    // 初始化硬件
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(BEEP_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED1_PIN,PIN_HIGH);// LED1 OFF
    rt_pin_write(LED2_PIN,PIN_HIGH);// LED2 OFF
    rt_pin_write(BEEP_PIN,PIN_LOW); // BEEP OFF


    /* 从EasyFlash读取MQTT服务器地址 */
    const char *host = ef_get_env(TB_MQTT_SERVER);
    if (host && rt_strlen(host) > 0) {
        rt_strncpy(device_url, host, sizeof(device_url) - 1);
        device_url[sizeof(device_url) - 1] = '\0';
        LOG_I("TB MQTT server from env: %s", device_url);
    } else {
        LOG_I("TB MQTT server using default: %s", device_url);
    }
    
    /* 从EasyFlash读取设备Token */
    const char *token = ef_get_env(TB_TOKEN);
    if (token && rt_strlen(token) > 0) {
        rt_strncpy(device_token, token, sizeof(device_token) - 1);
        device_token[sizeof(device_token) - 1] = '\0';
        LOG_I("TB device token from env: %s", device_token);
    } else {
        LOG_I("TB device token using default: %s", device_token);
    }

    /* 配置MQTT客户端参数 */
    tb_client.isconnected = 0;
    tb_client.uri = device_url;

    rt_snprintf(client_id, sizeof(client_id), "%s", device_token);

    rt_memcpy(&tb_client.condata, &condata, sizeof(condata));
    tb_client.condata.clientID.cstring = client_id;
    tb_client.condata.username.cstring = device_token;
    tb_client.condata.keepAliveInterval = 60;  /* 设置为60秒，确保在10分钟内有足够的心跳 */
    tb_client.condata.cleansession = 1;
    tb_client.condata.willFlag = 0;

    tb_client.buf_size = tb_client.readbuf_size = 2048;
    tb_client.buf = (uint8_t*)rt_calloc(1, tb_client.buf_size);
    tb_client.readbuf = (uint8_t*)rt_calloc(1, tb_client.readbuf_size);
    if (!(tb_client.buf && tb_client.readbuf)) {
        LOG_E("No memory for MQTT client buffer!");
        return -1;
    }

    tb_client.connect_callback = mqtt_connect_callback;
    tb_client.online_callback = mqtt_online_callback;
    tb_client.offline_callback = mqtt_offline_callback;

    tb_client.messageHandlers[0].topicFilter = rt_strdup(TB_RPC_REQUEST_TOPIC);
    tb_client.messageHandlers[0].callback = tb_rpc_callback;
    tb_client.messageHandlers[0].qos = QOS1;

    tb_client.messageHandlers[1].topicFilter = rt_strdup(TB_ATTRIBUTES_TOPIC);
    tb_client.messageHandlers[1].callback = tb_attributes_callback;
    tb_client.messageHandlers[1].qos = QOS1;

    tb_client.defaultMessageHandler = tb_default_callback;

    /* 启动MQTT客户端 */
    LOG_I("Starting MQTT client with device token: %s", device_token);
    initial_attributes_published = 0;  /* 重置初始属性发布标志 */
    if (paho_mqtt_start(&tb_client) == 0) {
        is_started = 1;
        LOG_I("MQTT client started successfully");
        /* 初始属性将在 online_callback 中发布 */
        return 0;
    } else {
        LOG_E("Failed to start MQTT client");
        return -1;
    }
}

/**
 * 停止MQTT客户端
 */
int mqtt_stop(int argc, char **argv)
{
    is_started = 0;
    initial_attributes_published = 0;  /* 重置初始属性发布标志 */
    return paho_mqtt_stop(&tb_client);
}

/**
 * 发布遥测数据命令 - 只上传CPU温度
 */
int mqtt_telemetry(void)
{
    if (!is_started) {
        LOG_E("MQTT client is not started.");
        return -1;
    }

    /* 检查MQTT是否已完全连接 */
    if (!tb_client.isconnected) {
        LOG_D("MQTT client is not connected yet, skip telemetry publish");
        return -1;
    }

    if (tb_publish_telemetry(&tb_client, &device_status) == 0)
    {
        LOG_D("Telemetry data published");
        return 0;
    } else {
        LOG_E("Failed to publish telemetry data");
        return -1;
    }
}

/**
 * 发布属性数据命令
 */
int mqtt_attributes(int argc, char **argv)
{
    if (!is_started) {
        LOG_E("MQTT client is not started.");
        return -1;
    }

    /* 检查MQTT是否已完全连接 */
    if (!tb_client.isconnected) {
        LOG_W("MQTT client is not connected yet, cannot publish attributes");
        return -1;
    }

    if (tb_publish_attributes(&tb_client, &device_status) == 0) {
        LOG_I("Attributes published");
        return 0;
    } else {
        LOG_E("Failed to publish attributes");
        return -1;
    }
}

/**
 * 检查MQTT连接状态
 */
int mqtt_is_connected(void)
{
    return (is_started && tb_client.isconnected);
}

/**
 * 获取设备状态结构体指针（供外部调用）
 */
device_status_t* mqtt_get_device_status(void)
{
    return &device_status;
}

/* 导出到MSH命令 */
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(mqtt_start, Start ThingsBoard MQTT client [device_token]);
MSH_CMD_EXPORT(mqtt_stop, Stop MQTT client);
MSH_CMD_EXPORT(mqtt_telemetry, Publish telemetry data to ThingsBoard);
MSH_CMD_EXPORT(mqtt_attributes, Publish attributes to ThingsBoard);
#endif /* FINSH_USING_MSH */

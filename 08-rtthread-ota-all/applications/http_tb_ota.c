/*
 * Copyright (c) 2024, Your Name
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-01-01     YSL2020      ThingsBoard HTTP OTA implementation
 * 2024-01-02     YSL2020      Added CRC32 checksum verification
 * 2024-01-03     YSL2020      Fixed data transmission issues
 * 2024-01-04     YSL2020      Fixed CRC32 endianness issue
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
#include <finsh.h>
#include <webclient.h>
#include <fal.h>
#include <cJSON.h>
#include <easyflash.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME          "http_tb_ota"
#ifdef HTTP_TB_OTA_DEBUG
#define DBG_LEVEL                 DBG_LOG
#else
#define DBG_LEVEL                 DBG_INFO
#endif
#define DBG_COLOR
#include <rtdbg.h>

/* 配置参数 */
#define HTTP_TB_OTA_BUFF_LEN          1024
#define HTTP_TB_OTA_HEADER_BUFSZ      2048
#define HTTP_TB_OTA_ATTRIBUTE_TIMEOUT (10 * RT_TICK_PER_SECOND)

/* ThingsBoard OTA属性定义 */
#define TB_FW_CHECKSUM_ATTR          "fw_checksum"
#define TB_FW_CHECKSUM_ALG_ATTR      "fw_checksum_algorithm"
#define TB_FW_SIZE_ATTR              "fw_size"
#define TB_FW_TITLE_ATTR             "fw_title"
#define TB_FW_VERSION_ATTR           "fw_version"
#define TB_FW_STATE_ATTR             "fw_state"

/* OTA状态枚举 */
typedef enum {
    TB_OTA_STATE_IDLE = 0,
    TB_OTA_STATE_DOWNLOADING,
    TB_OTA_STATE_DOWNLOADED,
    TB_OTA_STATE_VERIFYING,
    TB_OTA_STATE_VERIFIED,
    TB_OTA_STATE_UPDATING,
    TB_OTA_STATE_UPDATED,
    TB_OTA_STATE_FAILED
} tb_ota_state_t;

/* 全局变量 */
static const struct fal_partition *dl_part = NULL;
static int current_offset = 0;
static int firmware_size = 0;
static char firmware_title[64] = {0};
static char firmware_version[32] = {0};
static char firmware_checksum[128] = {0};
static char firmware_checksum_alg[32] = {0};

/* ThingsBoard服务器配置 */
static struct {
    char host[64];
    int port;
    char token[128];
    int chunk_size;
} tb_config = {
    .host = "localhost",
    .port = 8080,
    .token = "",
    .chunk_size = HTTP_TB_OTA_BUFF_LEN
};

/* CRC32查找表 */
static uint32_t crc32_table[256];
static int crc32_table_initialized = 0;

/* 函数声明 */
static int http_tb_ota_verify_checksum(void);
static void http_tb_ota_report_state(tb_ota_state_t state);
static int http_tb_ota_download_firmware(void);
static int http_tb_ota_parse_attributes(const char *json_str);
static int http_tb_ota_get_attributes(void);
static int http_tb_ota_chunk_download_handle(char *buffer, int length);
static void http_tb_ota_init_crc32_table(void);
static uint32_t http_tb_ota_calc_flash_crc32(void);
static uint32_t http_tb_ota_reverse_bytes(uint32_t value);

/* 字节序反转函数 - 将32位整数的字节顺序反转 */
static uint32_t http_tb_ota_reverse_bytes(uint32_t value)
{
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

/* 初始化CRC32查找表 */
static void http_tb_ota_init_crc32_table(void)
{
    uint32_t polynomial = 0xEDB88320;
    uint32_t crc;

    for (uint32_t i = 0; i < 256; i++) {
        crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc = crc >> 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_initialized = 1;
}

/* 计算Flash中固件的CRC32 */
static uint32_t http_tb_ota_calc_flash_crc32(void)
{
    if (!dl_part || firmware_size <= 0) {
        LOG_E("CRC32 calculation parameters invalid");
        return 0;
    }

    if (!crc32_table_initialized) {
        http_tb_ota_init_crc32_table();
    }

    uint32_t crc = 0xFFFFFFFF;
    uint8_t buffer[HTTP_TB_OTA_BUFF_LEN];
    int bytes_remaining = firmware_size;
    int read_offset = 0;

    LOG_I("Calculating firmware CRC32 (%d bytes)...", firmware_size);

    while (bytes_remaining > 0) {
        int read_size = (bytes_remaining > HTTP_TB_OTA_BUFF_LEN) ?
                       HTTP_TB_OTA_BUFF_LEN : bytes_remaining;

        if (fal_partition_read(dl_part, read_offset, buffer, read_size) < 0) {
            LOG_E("Read from Flash failed at offset %d", read_offset);
            return 0;
        }

        for (int i = 0; i < read_size; i++) {
            crc = (crc >> 8) ^ crc32_table[(crc ^ buffer[i]) & 0xFF];
        }

        read_offset += read_size;
        bytes_remaining -= read_size;
    }

    return crc ^ 0xFFFFFFFF;
}

/* 进度回调函数 - 单行进度显示 */
static void print_progress(size_t cur_size, size_t total_size)
{
    static int last_percent = -1;
    uint8_t per = (cur_size * 100) / total_size;

    if (per > 100) per = 100;

    /* 只在百分比变化时更新 */
    if (per == last_percent) {
        return;
    }
    last_percent = per;

    /* 创建进度条 */
    char progress_sign[51] = {0};
    int filled = (per * 50) / 100;

    for (int i = 0; i < 50; i++) {
        if (i < filled) {
            progress_sign[i] = '=';
        } else if (i == filled) {
            progress_sign[i] = '>';
        } else {
            progress_sign[i] = ' ';
        }
    }
    progress_sign[50] = '\0';

    /* 使用 \r 在同一行打印进度 */
    rt_kprintf("\rDownload progress: [%s] %03d%%", progress_sign, per);

    /* 下载完成时换行 */
    if (per == 100) {
        rt_kprintf("\n");
    }
}

/* 分块下载处理函数 - 处理从服务器接收的每个数据块 */
static int http_tb_ota_chunk_download_handle(char *buffer, int length)
{
    int ret = 0;
    uint8_t *verify_buffer = NULL;

    if (dl_part == NULL) {
        LOG_E("Download partition not found!");
        ret = -1;
        goto __exit;
    }

    /* 检查是否已达到预期固件大小 */
    if (current_offset >= firmware_size) {
        LOG_W("Reached firmware size (%d), skipping extra data (%d bytes)",
              firmware_size, length);
        ret = 0;
        goto __exit;
    }

    /* 计算实际写入长度以避免超过固件大小 */
    int actual_write_length = length;
    int remaining_bytes = firmware_size - current_offset;

    if (length > remaining_bytes) {
        actual_write_length = remaining_bytes;
        LOG_D("Adjust write length from %d to %d", length, actual_write_length);
    }

    /* 写入数据到flash分区 */
    if (fal_partition_write(dl_part, current_offset, (uint8_t *)buffer, actual_write_length) < 0) {
        LOG_E("Flash write failed at offset: %d", current_offset);
        ret = -1;
        goto __exit;
    }

    /* 验证写入的数据 - 读取回数据并与原始数据比较 */
    verify_buffer = rt_malloc(actual_write_length);
    if (verify_buffer) {
        if (fal_partition_read(dl_part, current_offset, verify_buffer, actual_write_length) < 0) {
            LOG_E("Flash read-back verification failed at offset: %d", current_offset);
            ret = -1;
            goto __exit;
        }

        /* 比较写入和读取的数据 */
        if (memcmp(buffer, verify_buffer, actual_write_length) != 0) {
            LOG_E("Data verification failed at offset: %d", current_offset);
            ret = -1;
            goto __exit;
        }

        rt_free(verify_buffer);
        verify_buffer = NULL;
    }

    /* 更新当前偏移量 */
    current_offset += actual_write_length;

    /* 更新进度显示 */
    print_progress(current_offset, firmware_size);

    /* 完成日志 */
    if (current_offset >= firmware_size) {
        LOG_I("Download completed: %d/%d bytes", current_offset, firmware_size);
    }

__exit:
    /* 释放缓冲区内存 */
    if (buffer) {
        rt_free(buffer);
    }
    if (verify_buffer) {
        rt_free(verify_buffer);
    }
    return ret;
}

/* 从ThingsBoard获取固件属性 */
static int http_tb_ota_get_attributes(void)
{
    struct webclient_session *session = NULL;
    char *response = NULL;
    size_t resp_len = 0;
    char url[256] = {0};
    int ret = -1;

    /* 构建属性URL */
    rt_snprintf(url, sizeof(url), "http://%s:%d/api/v1/%s/attributes",
               tb_config.host, tb_config.port, tb_config.token);

    session = webclient_session_create(HTTP_TB_OTA_HEADER_BUFSZ);
    if (!session) {
        LOG_E("Create session failed");
        goto __exit;
    }

    /* 设置超时 */
    webclient_set_timeout(session, HTTP_TB_OTA_ATTRIBUTE_TIMEOUT);

    /* 添加必需的共享密钥参数 */
    char params[256] = {0};
    rt_snprintf(params, sizeof(params), "sharedKeys=%s,%s,%s,%s,%s",
               TB_FW_CHECKSUM_ATTR, TB_FW_CHECKSUM_ALG_ATTR,
               TB_FW_SIZE_ATTR, TB_FW_TITLE_ATTR, TB_FW_VERSION_ATTR);

    /* 构建完整URL */
    char full_url[512] = {0};
    rt_snprintf(full_url, sizeof(full_url), "%s?%s", url, params);

    LOG_D("Request URL: %s", full_url);

    /* 设置请求头 */
    webclient_header_fields_add(session, "Host: %s:%d\r\n", tb_config.host, tb_config.port);
    webclient_header_fields_add(session, "User-Agent: RT-Thread TB OTA Agent\r\n");
    webclient_header_fields_add(session, "Accept: application/json\r\n");

    /* 发送GET请求 */
    if (webclient_get(session, full_url) != 200) {
        LOG_E("Get attributes failed");
        goto __exit;
    }

    /* 获取响应 */
    if (webclient_response(session, (void **)&response, &resp_len) <= 0 || !response) {
        LOG_E("Get response failed");
        goto __exit;
    }

    LOG_D("Attributes response received, length: %d", resp_len);

    /* 解析JSON响应 */
    if (http_tb_ota_parse_attributes(response) != 0) {
        LOG_E("Parse attributes failed");
        goto __exit;
    }

    ret = 0;

__exit:
    if (response) {
        rt_free(response);
    }
    if (session) {
        webclient_close(session);
    }
    return ret;
}

/* 解析ThingsBoard JSON属性 */
static int http_tb_ota_parse_attributes(const char *json_str)
{
    cJSON *root = cJSON_Parse(json_str);
    cJSON *shared = NULL;
    cJSON *item = NULL;
    int ret = -1;

    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            LOG_E("JSON parse error: %s", error_ptr);
        } else {
            LOG_E("JSON parse failed");
        }
        goto __exit;
    }

    shared = cJSON_GetObjectItem(root, "shared");
    if (!shared) {
        LOG_E("Shared attributes not found");
        cJSON_Delete(root);
        return -1;
    }

    /* 解析固件属性 */
    item = cJSON_GetObjectItem(shared, TB_FW_TITLE_ATTR);
    if (item && cJSON_IsString(item)) {
        rt_strncpy(firmware_title, item->valuestring, sizeof(firmware_title) - 1);
    } else {
        LOG_W("Missing or invalid fw_title attribute");
    }

    item = cJSON_GetObjectItem(shared, TB_FW_VERSION_ATTR);
    if (item && cJSON_IsString(item)) {
        rt_strncpy(firmware_version, item->valuestring, sizeof(firmware_version) - 1);
    } else {
        LOG_W("Missing or invalid fw_version attribute");
    }

    item = cJSON_GetObjectItem(shared, TB_FW_SIZE_ATTR);
    if (item && cJSON_IsNumber(item)) {
        firmware_size = item->valueint;
    } else {
        LOG_W("Missing or invalid fw_size attribute");
    }

    item = cJSON_GetObjectItem(shared, TB_FW_CHECKSUM_ATTR);
    if (item && cJSON_IsString(item)) {
        rt_strncpy(firmware_checksum, item->valuestring, sizeof(firmware_checksum) - 1);
    } else {
        LOG_W("Missing or invalid fw_checksum attribute");
    }

    item = cJSON_GetObjectItem(shared, TB_FW_CHECKSUM_ALG_ATTR);
    if (item && cJSON_IsString(item)) {
        rt_strncpy(firmware_checksum_alg, item->valuestring, sizeof(firmware_checksum_alg) - 1);
    } else {
        LOG_W("Missing or invalid fw_checksum_algorithm attribute");
    }

    /* 验证必需属性 */
    if (firmware_size > 0 && firmware_title[0] != '\0' &&
        firmware_version[0] != '\0' && firmware_checksum[0] != '\0') {
        ret = 0;
        LOG_I("Firmware: %s v%s, Size: %d bytes, Checksum: %s (%s)",
              firmware_title, firmware_version, firmware_size,
              firmware_checksum, firmware_checksum_alg);
    } else {
        LOG_E("Firmware attributes incomplete");
        ret = -1;
    }

__exit:
    if (root) {
        cJSON_Delete(root);
    }
    return ret;
}

/* 从ThingsBoard下载固件 - 主要下载函数 */
static int http_tb_ota_download_firmware(void)
{
    struct webclient_session *session = NULL;
    int ret = 0;
    char url[512] = {0};

    /* 构建固件下载URL */
    rt_snprintf(url, sizeof(url),
               "http://%s:%d/api/v1/%s/firmware?title=%s&version=%s",
               tb_config.host, tb_config.port, tb_config.token,
               firmware_title, firmware_version);

    LOG_D("Download URL: %s", url);

    session = webclient_session_create(HTTP_TB_OTA_HEADER_BUFSZ);
    if (!session) {
        LOG_E("Create session failed");
        ret = -1;
        goto __exit;
    }

    /* 设置超时 */
    webclient_set_timeout(session, HTTP_TB_OTA_ATTRIBUTE_TIMEOUT);

    /* 使用属性中的固件大小 */
    if (firmware_size <= 0) {
        LOG_E("Invalid firmware size: %d", firmware_size);
        ret = -1;
        goto __exit;
    }

    LOG_I("Firmware size: %d bytes", firmware_size);

    /* 准备下载分区 */
    if ((dl_part = fal_partition_find("download")) == NULL) {
        LOG_E("Download partition not found");
        ret = -1;
        goto __exit;
    }

    /* 擦除下载分区为写入做准备 */
    LOG_I("Erasing download partition (%d bytes)...", firmware_size);
    if (fal_partition_erase(dl_part, 0, firmware_size) < 0) {
        LOG_E("Partition erase failed");
        ret = -1;
        goto __exit;
    }

    /* 注册分块下载处理函数 */
    webclient_register_shard_position_function(session, http_tb_ota_chunk_download_handle);

    /* 开始分块下载 */
    current_offset = 0;
    http_tb_ota_report_state(TB_OTA_STATE_DOWNLOADING);

    /* 执行分块下载 */
    ret = webclient_shard_position_function(session, url, 0, firmware_size, tb_config.chunk_size);

    /* 下载完成后取消注册处理函数 */
    webclient_register_shard_position_function(session, NULL);

    if (ret == 0) {
        /* 检查下载完整性 */
        if (current_offset == firmware_size) {
            http_tb_ota_report_state(TB_OTA_STATE_DOWNLOADED);
            LOG_I("Firmware download completed successfully");
        } else {
            LOG_E("Download incomplete: expected %d, actual %d", firmware_size, current_offset);
            http_tb_ota_report_state(TB_OTA_STATE_FAILED);
            ret = -1;
        }
    } else {
        http_tb_ota_report_state(TB_OTA_STATE_FAILED);
        LOG_E("Firmware download failed: %d", ret);
    }

__exit:
    if (session) {
        webclient_close(session);
    }
    return ret;
}

/* 验证固件校验和 */
static int http_tb_ota_verify_checksum(void)
{
    LOG_I("Starting checksum verification: algorithm=%s", firmware_checksum_alg);

    /* 检查校验和算法是否支持 */
    if (rt_strcasecmp(firmware_checksum_alg, "crc32") != 0) {
        LOG_E("Unsupported checksum algorithm: %s", firmware_checksum_alg);
        return -1;
    }

    /* 计算下载固件的CRC32 */
    uint32_t calculated_crc = http_tb_ota_calc_flash_crc32();
    if (calculated_crc == 0) {
        LOG_E("Calculate CRC32 checksum failed");
        return -1;
    }

    /* 对CRC32结果进行字节序反转 */
    uint32_t reversed_crc = http_tb_ota_reverse_bytes(calculated_crc);

    /* 将服务器校验和字符串转换为uint32_t */
    char *endptr;
    uint32_t expected_crc = (uint32_t)strtoul(firmware_checksum, &endptr, 16);
    if (endptr == firmware_checksum) {
        LOG_E("Invalid checksum format: %s", firmware_checksum);
        return -1;
    }

    LOG_I("Checksum verification:");
    LOG_I("  Expected  :  0x%08x", expected_crc);
    LOG_I("  Calculated:  0x%08x", reversed_crc);

    if (reversed_crc == expected_crc) {
        LOG_I("Checksum verification passed");
        return 0;
    } else {
        LOG_E("Checksum verification failed");
        return -1;
    }
}

/* 向ThingsBoard报告OTA状态 */
static void http_tb_ota_report_state(tb_ota_state_t state)
{
    struct webclient_session *session = NULL;
    char url[256] = {0};
    char telemetry[128] = {0};

    /* 构建遥测URL */
    rt_snprintf(url, sizeof(url), "http://%s:%d/api/v1/%s/telemetry",
               tb_config.host, tb_config.port, tb_config.token);

    /* 创建遥测JSON */
    const char *state_str = "IDLE";
    switch (state) {
        case TB_OTA_STATE_DOWNLOADING: state_str = "DOWNLOADING"; break;
        case TB_OTA_STATE_DOWNLOADED: state_str = "DOWNLOADED"; break;
        case TB_OTA_STATE_VERIFYING: state_str = "VERIFYING"; break;
        case TB_OTA_STATE_VERIFIED: state_str = "VERIFIED"; break;
        case TB_OTA_STATE_UPDATING: state_str = "UPDATING"; break;
        case TB_OTA_STATE_UPDATED: state_str = "UPDATED"; break;
        case TB_OTA_STATE_FAILED: state_str = "FAILED"; break;
        default: break;
    }

    rt_snprintf(telemetry, sizeof(telemetry),
               "{\"%s\":\"%s\",\"current_%s\":\"%s\",\"current_%s\":\"%s\"}",
               TB_FW_STATE_ATTR, state_str,
               TB_FW_TITLE_ATTR, firmware_title,
               TB_FW_VERSION_ATTR, firmware_version);

    LOG_D("Reporting state: %s", state_str);

    session = webclient_session_create(HTTP_TB_OTA_HEADER_BUFSZ);
    if (!session) {
        LOG_E("Create state report session failed");
        return;
    }

    /* 设置超时 */
    webclient_set_timeout(session, HTTP_TB_OTA_ATTRIBUTE_TIMEOUT);

    /* 设置请求头 */
    webclient_header_fields_add(session, "Host: %s:%d\r\n", tb_config.host, tb_config.port);
    webclient_header_fields_add(session, "Content-Type: application/json\r\n");
    webclient_header_fields_add(session, "Content-Length: %d\r\n", rt_strlen(telemetry));
    webclient_header_fields_add(session, "User-Agent: RT-Thread TB OTA Agent\r\n");

    /* 发送POST请求 */
    if (webclient_post(session, url, telemetry, rt_strlen(telemetry)) != 200) {
        LOG_E("Report state failed");
    } else {
        LOG_I("State reported: %s", state_str);
    }

    if (session) {
        webclient_close(session);
    }
}

/* 主OTA函数 */
static int http_tb_ota_start(const char *host, int port, const char *token)
{
    int ret = 0;

    /* 验证参数 */
    if (!host || !token) {
        LOG_E("Invalid parameters");
        return -1;
    }

    /* 更新配置 */
    rt_strncpy(tb_config.host, host, sizeof(tb_config.host) - 1);
    tb_config.port = port;
    rt_strncpy(tb_config.token, token, sizeof(tb_config.token) - 1);

    LOG_I("Starting ThingsBoard OTA: http://%s:%d", tb_config.host, tb_config.port);

    /* 步骤1: 获取固件属性 */
    http_tb_ota_report_state(TB_OTA_STATE_IDLE);

    if (http_tb_ota_get_attributes() != 0) {
        LOG_E("Get firmware attributes failed");
        ret = -1;
        goto __exit;
    }

    /* 步骤2: 下载固件 */
    if (http_tb_ota_download_firmware() != 0) {
        LOG_E("Firmware download failed");
        ret = -1;
        goto __exit;
    }

    /* 步骤3: 验证校验和 */
    http_tb_ota_report_state(TB_OTA_STATE_VERIFYING);
    if (http_tb_ota_verify_checksum() != 0) {
        LOG_E("Checksum verification failed");
        http_tb_ota_report_state(TB_OTA_STATE_FAILED);
        ret = -1;
        goto __exit;
    }
    http_tb_ota_report_state(TB_OTA_STATE_VERIFIED);

    /* 步骤4: 更新固件 */
    http_tb_ota_report_state(TB_OTA_STATE_UPDATING);
    LOG_I("Firmware update completed successfully");
    http_tb_ota_report_state(TB_OTA_STATE_UPDATED);

    /* 步骤5: 重启系统 */
    LOG_I("System will reboot in 5 seconds...");
    rt_thread_mdelay(5000);

    extern void rt_hw_cpu_reset(void);
    rt_hw_cpu_reset();

__exit:
    return ret;
}

/* FINSH命令接口 */
void http_tb_ota(int argc, char **argv)
{
    if (argc < 4) {
        rt_kprintf("Usage: http_tb_ota <host> <port> <token>\n");
        rt_kprintf("Example: http_tb_ota xthings.cloud 80 2xGnIJWwereMI3hMVpOt\n");
        return;
    }

    const char *host = argv[1];
    int port = atoi(argv[2]);
    const char *token = argv[3];

    http_tb_ota_start(host, port, token);
}

/* MSH命令导出 */
MSH_CMD_EXPORT(http_tb_ota, ThingsBoard HTTP OTA: http_tb_ota <host> <port> <token>);

/* 初始化函数 */
int http_tb_ota_init(void)
{
#define  TB_OTA_HOST     "tb_ota_host"
#define  TB_OTA_PORT     "tb_ota_port"
#define  TB_OTA_TOKEN    "tb_ota_token"
#define  TB_OTA_UPDATE   "tb_ota_update"

    const char *host = ef_get_env(TB_OTA_HOST);
    if (host) {
        rt_strcpy(tb_config.host, host);
        LOG_I("TB OTA host: %s", host);
    }

    const char *port = ef_get_env(TB_OTA_PORT);
    if (port) {
        tb_config.port = atoi(port);
        LOG_I("TB OTA port: %s", port);
    }

    const char *token = ef_get_env(TB_OTA_TOKEN);
    if (token) {
        rt_strcpy(tb_config.token, token);
        LOG_I("TB OTA token configured");
    }

    const char *update = ef_get_env(TB_OTA_UPDATE);
    if (update && rt_strncmp(update, "true", 4) == 0) {
        LOG_I("Starting OTA update...");
        rt_thread_delay(1000);
        ef_set_env(TB_OTA_UPDATE, "false");
        http_tb_ota_start(tb_config.host, tb_config.port, tb_config.token);
    }

    LOG_I("HTTP ThingsBoard OTA package initialized");
    return 0;
}

//INIT_APP_EXPORT(http_tb_ota_init);

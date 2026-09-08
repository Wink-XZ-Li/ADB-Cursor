/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2026, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    mcu_api.c
 * @author  涂鸦综合协议开发组
 * @version v2.6.2
 * @date    2026.02.04
 * @brief   用户需要主动调用的函数都在该文件内
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#define MCU_API_GLOBAL

#include "mcu_api.h"

#include "protocol.h"
#include "wifi.h"

/**
 * @brief  hex转bcd
 * @param[in] {Value_H} 高字节
 * @param[in] {Value_L} 低字节
 * @return 转换完成后数据
 */
u8 hex_to_bcd(u8 Value_H, u8 Value_L) {
    u8 bcd_value;

    if ((Value_H >= '0') && (Value_H <= '9'))
        Value_H -= '0';
    else if ((Value_H >= 'A') && (Value_H <= 'F'))
        Value_H = Value_H - 'A' + 10;
    else if ((Value_H >= 'a') && (Value_H <= 'f'))
        Value_H = Value_H - 'a' + 10;

    bcd_value = Value_H & 0x0f;

    bcd_value <<= 4;
    if ((Value_L >= '0') && (Value_L <= '9'))
        Value_L -= '0';
    else if ((Value_L >= 'A') && (Value_L <= 'F'))
        Value_L = Value_L - 'a' + 10;
    else if ((Value_L >= 'a') && (Value_L <= 'f'))
        Value_L = Value_L - 'a' + 10;

    bcd_value |= Value_L & 0x0f;

    return bcd_value;
}

/**
 * @brief  求字符串长度
 * @param[in] {str} 字符串地址
 * @return 数据长度
 */
u32 tuya_strlen(const char* str) {
    u32 len = 0;
    if (str == NULL) {
        return 0;
    }

    for (len = 0; *str++ != '\0';) {
        len++;
    }

    return len;
}

/**
 * @brief  把src所指内存区域的前count个字节设置成字符c
 * @param[out] {src} 待设置的内存首地址
 * @param[in] {ch} 设置的字符
 * @param[in] {count} 设置的内存长度
 * @return 待设置的内存首地址
 */
void* tuya_memset(void* src, u8 ch, u16 count) {
    u8* tmp = (u8*)src;

    if (src == NULL) {
        return NULL;
    }

    while (count--) {
        *tmp++ = ch;
    }

    return src;
}

/**
 * @brief  内存拷贝
 * @param[out] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {count} 拷贝数据个数
 * @return 数据处理完后的源地址
 */
void* tuya_memcpy(void* dest, const void* src, u16 count) {
    u8* pdest = (u8*)dest;
    const u8* psrc = (const u8*)src;
    u16 i;

    if (dest == NULL || src == NULL) {
        return NULL;
    }

    if ((pdest <= psrc) || (pdest > psrc + count)) {
        for (i = 0; i < count; i++) {
            pdest[i] = psrc[i];
        }
    } else {
        for (i = count; i > 0; i--) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

/**
 * @brief  字符串拷贝
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @return 数据处理完后的源地址
 */
char* tuya_strcpy(char* dest, const char* src) {
    char* p = dest;
    if ((NULL == dest) || (NULL == src)) {
        return NULL;
    }

    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
    return p;
}

char* tuya_strncpy(char* dest, const char* src, u16 count) {
    char* tmp = dest;
    while (count) {
        if ((*tmp = *src) != 0)
            src++;
        tmp++;
        count--;
    }
    return dest;
}

/**
 * @brief  字符串比较
 * @param[in] {s1} 字符串 1
 * @param[in] {s2} 字符串 2
 * @return 大小比较值
 * -         0:s1=s2
 * -         <0:s1<s2
 * -         >0:s1>s2
 */
i32 tuya_strcmp(char* s1, char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/**
 * @brief  将int类型拆分四个字节
 * @param[in] {number} 4字节原数据
 * @param[out] {value} 处理完成后4字节数据
 * @return Null
 */
void int_to_byte(u32 number, u8 value[4]) {
    value[0] = number >> 24;
    value[1] = number >> 16;
    value[2] = number >> 8;
    value[3] = number & 0xff;
}

/**
 * @brief  将4字节合并为1个32bit变量
 * @param[in] {value} 4字节数组
 * @return 合并完成后的32bit变量
 */
u32 byte_to_int(const u8 value[4]) {
    u32 nubmer = 0;

    nubmer = (u32)value[0];
    nubmer <<= 8;
    nubmer |= (u32)value[1];
    nubmer <<= 8;
    nubmer |= (u32)value[2];
    nubmer <<= 8;
    nubmer |= (u32)value[3];

    return nubmer;
}

/**
 * @brief  raw型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
u8 mcu_dp_raw_update(u8 dpid, const u8 value[], u16 len) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;
    //
    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_RAW);
    //
    send_len = set_wifi_uart_byte(send_len, len / 0x100);
    send_len = set_wifi_uart_byte(send_len, len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)value, len);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  bool型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
u8 mcu_dp_bool_update(u8 dpid, u8 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_BOOL);
    //
    send_len = set_wifi_uart_byte(send_len, 0);
    send_len = set_wifi_uart_byte(send_len, 1);
    //
    if (value == FALSE) {
        send_len = set_wifi_uart_byte(send_len, FALSE);
    } else {
        send_len = set_wifi_uart_byte(send_len, 1);
    }

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  value型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
u8 mcu_dp_value_update(u8 dpid, u32 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_VALUE);
    //
    send_len = set_wifi_uart_byte(send_len, 0);
    send_len = set_wifi_uart_byte(send_len, 4);
    //
    send_len = set_wifi_uart_byte(send_len, value >> 24);
    send_len = set_wifi_uart_byte(send_len, value >> 16);
    send_len = set_wifi_uart_byte(send_len, value >> 8);
    send_len = set_wifi_uart_byte(send_len, value & 0xff);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  string型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
u8 mcu_dp_string_update(u8 dpid, const u8 value[], u16 len) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;
    //
    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_STRING);
    //
    send_len = set_wifi_uart_byte(send_len, len / 0x100);
    send_len = set_wifi_uart_byte(send_len, len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)value, len);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  enum型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
u8 mcu_dp_enum_update(u8 dpid, u8 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_ENUM);
    //
    send_len = set_wifi_uart_byte(send_len, 0);
    send_len = set_wifi_uart_byte(send_len, 1);
    //
    send_len = set_wifi_uart_byte(send_len, value);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  fault型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
u8 mcu_dp_fault_update(u8 dpid, u32 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_BITMAP);
    //
    send_len = set_wifi_uart_byte(send_len, 0);

    if ((value | 0xff) == 0xff) {
        send_len = set_wifi_uart_byte(send_len, 1);
        send_len = set_wifi_uart_byte(send_len, value);
    } else if ((value | 0xffff) == 0xffff) {
        send_len = set_wifi_uart_byte(send_len, 2);
        send_len = set_wifi_uart_byte(send_len, value >> 8);
        send_len = set_wifi_uart_byte(send_len, value & 0xff);
    } else {
        send_len = set_wifi_uart_byte(send_len, 4);
        send_len = set_wifi_uart_byte(send_len, value >> 24);
        send_len = set_wifi_uart_byte(send_len, value >> 16);
        send_len = set_wifi_uart_byte(send_len, value >> 8);
        send_len = set_wifi_uart_byte(send_len, value & 0xff);
    }

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

u8 mcu_dp_struct_update(u8 dpid, const void* ptr_to_struct, u16 struct_size) {
    u16 send_len = 0;
    u8 dp_data_sz[2];
    UINT16_HTON(dp_data_sz, struct_size);

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_STRUCT);
    //
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[0]);
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[1]);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)ptr_to_struct, struct_size);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

u8 mcu_dp_array_update(u8 dpid, const void* ptr_to_array, u16 array_total_size) {
    u16 send_len = 0;
    u8 dp_data_sz[2];
    UINT16_HTON(dp_data_sz, array_total_size);

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_ARRAY);
    //
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[0]);
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[1]);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)ptr_to_array, array_total_size);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

#ifdef MCU_DP_UPLOAD_SYN
/**
 * @brief  raw型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
u8 mcu_dp_raw_update_syn(u8 dpid, const u8 value[], u16 len) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;
    //
    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_RAW);
    //
    send_len = set_wifi_uart_byte(send_len, len / 0x100);
    send_len = set_wifi_uart_byte(send_len, len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)value, len);

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  bool型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
u8 mcu_dp_bool_update_syn(u8 dpid, u8 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_BOOL);
    //
    send_len = set_wifi_uart_byte(send_len, 0);
    send_len = set_wifi_uart_byte(send_len, 1);
    //
    if (value == FALSE) {
        send_len = set_wifi_uart_byte(send_len, FALSE);
    } else {
        send_len = set_wifi_uart_byte(send_len, 1);
    }

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  value型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
u8 mcu_dp_value_update_syn(u8 dpid, u32 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_VALUE);
    //
    send_len = set_wifi_uart_byte(send_len, 0);
    send_len = set_wifi_uart_byte(send_len, 4);
    //
    send_len = set_wifi_uart_byte(send_len, value >> 24);
    send_len = set_wifi_uart_byte(send_len, value >> 16);
    send_len = set_wifi_uart_byte(send_len, value >> 8);
    send_len = set_wifi_uart_byte(send_len, value & 0xff);

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  string型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
u8 mcu_dp_string_update_syn(u8 dpid, const u8 value[], u16 len) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;
    //
    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_STRING);
    //
    send_len = set_wifi_uart_byte(send_len, len / 0x100);
    send_len = set_wifi_uart_byte(send_len, len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)value, len);

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  enum型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
u8 mcu_dp_enum_update_syn(u8 dpid, u8 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_ENUM);
    //
    send_len = set_wifi_uart_byte(send_len, 0);
    send_len = set_wifi_uart_byte(send_len, 1);

    send_len = set_wifi_uart_byte(send_len, value);

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

/**
 * @brief  fault型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
u8 mcu_dp_fault_update_syn(u8 dpid, u32 value) {
    u16 send_len = 0;

    if (stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_BITMAP);
    //
    send_len = set_wifi_uart_byte(send_len, 0);

    if ((value | 0xff) == 0xff) {
        send_len = set_wifi_uart_byte(send_len, 1);
        send_len = set_wifi_uart_byte(send_len, value);
    } else if ((value | 0xffff) == 0xffff) {
        send_len = set_wifi_uart_byte(send_len, 2);
        send_len = set_wifi_uart_byte(send_len, value >> 8);
        send_len = set_wifi_uart_byte(send_len, value & 0xff);
    } else {
        send_len = set_wifi_uart_byte(send_len, 4);
        send_len = set_wifi_uart_byte(send_len, value >> 24);
        send_len = set_wifi_uart_byte(send_len, value >> 16);
        send_len = set_wifi_uart_byte(send_len, value >> 8);
        send_len = set_wifi_uart_byte(send_len, value & 0xff);
    }

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

u8 mcu_dp_struct_update_syn(u8 dpid, const void* ptr_to_struct, u16 struct_size) {
    u16 send_len = 0;
    u8 dp_data_sz[2];
    UINT16_HTON(dp_data_sz, struct_size);

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_STRUCT);
    //
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[0]);
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[1]);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)ptr_to_struct, struct_size);

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}

u8 mcu_dp_array_update_syn(u8 dpid, const void* ptr_to_array, u16 array_total_size) {
    u16 send_len = 0;
    u8 dp_data_sz[2];
    UINT16_HTON(dp_data_sz, array_total_size);

    send_len = set_wifi_uart_byte(send_len, dpid);
    send_len = set_wifi_uart_byte(send_len, DP_TYPE_ARRAY);
    //
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[0]);
    send_len = set_wifi_uart_byte(send_len, dp_data_sz[1]);
    //
    send_len = set_wifi_uart_buffer(send_len, (u8*)ptr_to_array, array_total_size);

    wifi_uart_write_frame(STATE_UPLOAD_SYN_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}
#endif

/**
 * @brief  mcu获取bool型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
u8 mcu_get_dp_download_bool(const u8 value[], u16 len) {
    TUYA_UNUSED(len);
    return (value[0]);
}

/**
 * @brief  mcu获取enum型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
u8 mcu_get_dp_download_enum(const u8 value[], u16 len) {
    TUYA_UNUSED(len);
    return (value[0]);
}

/**
 * @brief  mcu获取value型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
u32 mcu_get_dp_download_value(const u8 value[], u16 len) {
    TUYA_UNUSED(len);
    return (byte_to_int(value));
}

/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
void uart_receive_input(u8 value) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请在串口接收中断中调用uart_receive_input(value),串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行"
#endif

    if (1 == rx_buf_out - rx_buf_in) {
        // 串口接收缓存已满
    } else if ((rx_buf_in > rx_buf_out) && ((unsigned long int)(rx_buf_in - rx_buf_out) >= sizeof(wifi_uart_rx_buf))) {
        // 串口接收缓存已满
    } else {
        // 串口接收缓存未满
        if (rx_buf_in >= (u8*)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf))) {
            rx_buf_in = (u8*)(wifi_uart_rx_buf);
        }

        *rx_buf_in++ = value;
    }
}

/**
 * @brief  串口接收多个字节数据暂存处理
 * @param[in] {value} 串口要接收的数据的源地址
 * @param[in] {data_len} 串口要接收的数据的数据长度
 * @return Null
 * @note   如需要支持一次多字节缓存，可调用该函数
 */
void uart_receive_buff_input(u8 value[], u16 data_len) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请在需要一次缓存多个字节串口数据处调用此函数,串口数据由MCU_SDK处理,用户请勿再另行处理,完成后删除该行"
#endif

    u16 i = 0;
    for (i = 0; i < data_len; i++) {
        uart_receive_input(value[i]);
    }
}

#ifdef CONFIG_MCU_SDK_TEST_ONLY_USE_NEW_DISPATCHER

typedef u8 DISPATCHER_STATUS_E;
#define DISPATCHER_STATUS_WAIT_SYNC_1 0x00
#define DISPATCHER_STATUS_WAIT_SYNC_2 0x01
#define DISPATCHER_STATUS_RECV_HEADER 0x02
#define DISPATCHER_STATUS_RECV_DATA 0x03
#define DISPATCHER_STATUS_RECV_CHECKSUM 0x04
#define DISPATCHER_STATUS_DUMMY_RECV 0x05

#endif

/**
 * @brief  wifi串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void wifi_uart_service(void) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请直接在main函数的while(1){}中添加wifi_uart_service(),调用该函数不要加任何条件判断,完成后删除该行"
#endif

#ifndef CONFIG_MCU_SDK_TEST_ONLY_USE_NEW_DISPATCHER

    static u16 rx_in = 0;
    u16 offset = 0;
    u16 rx_value_len = 0;

    while ((rx_in < sizeof(wifi_data_process_buf)) && with_data_rxbuff() > 0) {
        wifi_data_process_buf[rx_in++] = take_byte_rxbuff();
    }

    if (rx_in < PROTOCOL_HEAD)
        return;

    while ((rx_in - offset) >= PROTOCOL_HEAD) {
        if (wifi_data_process_buf[offset + HEAD_FIRST] != FRAME_FIRST) {
            offset++;
            TUYA_DBG_EXEC(TUYA_PRINT("header 1 error, got %02X", wifi_data_process_buf[offset + HEAD_FIRST]));
            continue;
        }

        if (wifi_data_process_buf[offset + HEAD_SECOND] != FRAME_SECOND) {
            TUYA_DBG_EXEC(TUYA_PRINT("header 2 error, got %02X", wifi_data_process_buf[offset + HEAD_SECOND]));
            offset++;
            continue;
        }

        if (wifi_data_process_buf[offset + PROTOCOL_VERSION] != MCU_RX_VER) {
            offset += 2;
            continue;
        }

        rx_value_len = wifi_data_process_buf[offset + LENGTH_HIGH] * 0x100;
        rx_value_len += (wifi_data_process_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);

        if (rx_value_len > sizeof(wifi_data_process_buf) + PROTOCOL_HEAD) {
            TUYA_DBG_EXEC(TUYA_PRINT("length overflow, got %d.", rx_value_len));
            TUYA_DBG_EXEC(TUYA_PRINT("whole process buffer and rx buffer discarded."));
            rx_buf_in = rx_buf_out = wifi_uart_rx_buf;
            break;
        }

        if ((rx_in - offset) < rx_value_len) {
            break;
        }

        // 数据接收完成
        if (get_check_sum((u8*)wifi_data_process_buf + offset, rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            // 校验出错
            TUYA_DBG_EXEC(TUYA_PRINT("crc error (crc:0x%X  but data:0x%X)\r\n", get_check_sum((u8*)wifi_data_process_buf + offset, rx_value_len - 1), wifi_data_process_buf[offset + rx_value_len - 1]));
            offset += 3;
            continue;
        }

        data_handle(offset);
        offset += rx_value_len;
    }  // end while

    rx_in -= offset;
    if (rx_in > 0) {
        tuya_memcpy((i8*)wifi_data_process_buf, (const i8*)wifi_data_process_buf + offset, rx_in);
    }

#else

    STATIC DISPATCHER_STATUS_E dispatcher_status = DISPATCHER_STATUS_WAIT_SYNC_1;
    STATIC u16 proc_buf_next_writeable_index = 0;
    STATIC u16 expected_byte_count = 0;
    STATIC bool_t overflow_detected = FALSE;

    u8 recv_byte = 0;

    while (with_data_rxbuff() > 0) {
        recv_byte = take_byte_rxbuff();
        switch (dispatcher_status) {
            case DISPATCHER_STATUS_WAIT_SYNC_1: {
                if (recv_byte != 0x55) {
                    TUYA_DBG_EXEC(TUYA_PRINT("Sync failed at 0x55, got 0x%02x", recv_byte));
                    break;
                }
                dispatcher_status = DISPATCHER_STATUS_WAIT_SYNC_2;
                wifi_data_process_buf[proc_buf_next_writeable_index++] = recv_byte;
            } break;

            case DISPATCHER_STATUS_WAIT_SYNC_2: {
                if (recv_byte != 0xAA) {
                    TUYA_DBG_EXEC(TUYA_PRINT("Sync failed at 0xAA, got 0x%02x", recv_byte));
                    break;
                }
                dispatcher_status = DISPATCHER_STATUS_RECV_HEADER;
                wifi_data_process_buf[proc_buf_next_writeable_index++] = recv_byte;
                expected_byte_count = 4;
            } break;

            case DISPATCHER_STATUS_RECV_HEADER: {
                wifi_data_process_buf[proc_buf_next_writeable_index++] = recv_byte;
                expected_byte_count--;
                if (expected_byte_count == 0) {
                    if (wifi_data_process_buf[PROTOCOL_VERSION] != MCU_RX_VER) {
                        TUYA_DBG_EXEC(TUYA_PRINT("Unexcepted version: %02x", wifi_data_process_buf[PROTOCOL_VERSION]));
                        goto sync_lost;
                    }
                    expected_byte_count = (u16)(wifi_data_process_buf[LENGTH_HIGH] << 8) | wifi_data_process_buf[LENGTH_LOW];
                    if (expected_byte_count > sizeof(wifi_data_process_buf) - PROTOCOL_HEAD) {
                        TUYA_DBG_EXEC(TUYA_PRINT("Overflow detected, len: %d", expected_byte_count));
                        overflow_detected = TRUE;
                        switch (on_process_buffer_overflow(wifi_data_process_buf[FRAME_TYPE], expected_byte_count)) {
                            case PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_WHOLE_COMMAND:  // 放弃整个指令
                            default:
                                expected_byte_count += 1;  // 加上checksum
                                dispatcher_status = DISPATCHER_STATUS_DUMMY_RECV;
                                TUYA_DBG_EXEC(TUYA_PRINT("Whole command discarded, do dummy read for %d bytes", expected_byte_count));
                                break;
                            case PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT_AND_RX_BUFFER:  // 放弃当前和接收缓冲区数据
                                rx_buf_in = rx_buf_out = wifi_uart_rx_buf;
                                TUYA_DBG_EXEC(TUYA_PRINT("Current command discarded, rx buffer cleared."));
                                goto sync_lost;
                            case PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT:  // 仅放弃当前命令
                                TUYA_DBG_EXEC(TUYA_PRINT("Current command discarded."));
                                goto sync_lost;
                        }
                    }
                    TUYA_DBG_EXEC(TUYA_PRINT("Got full head, cmd is 0x%02x, len is %d", wifi_data_process_buf[FRAME_TYPE], expected_byte_count));
                    if (expected_byte_count == 0) {
                        dispatcher_status = DISPATCHER_STATUS_RECV_CHECKSUM;
                    } else {
                        dispatcher_status = DISPATCHER_STATUS_RECV_DATA;
                    }
                }
            } break;

            case DISPATCHER_STATUS_RECV_DATA: {
                wifi_data_process_buf[proc_buf_next_writeable_index++] = recv_byte;
                expected_byte_count--;
                if (expected_byte_count == 0) {
                    TUYA_DBG_EXEC(TUYA_PRINT("Got full data"));
                    dispatcher_status = DISPATCHER_STATUS_RECV_CHECKSUM;
                }
            } break;

            case DISPATCHER_STATUS_RECV_CHECKSUM: {
                if (get_check_sum((u8*)wifi_data_process_buf, proc_buf_next_writeable_index) != recv_byte) {
                    TUYA_DBG_EXEC(TUYA_PRINT("crc error (crc:0x%X  but data:0x%X)\r\n", get_check_sum((u8*)wifi_data_process_buf, proc_buf_next_writeable_index - 1), wifi_data_process_buf[proc_buf_next_writeable_index - 1]));
                    goto sync_lost;
                }
                wifi_data_process_buf[proc_buf_next_writeable_index++] = recv_byte;

                TUYA_DBG_EXEC(TUYA_PRINT("Got full command, processing..."));
                if (!overflow_detected || on_first_command_after_overflow((const u8*)wifi_data_process_buf, proc_buf_next_writeable_index)) {
                    data_handle(0);
                    overflow_detected = FALSE;
                } else {
                    TUYA_DBG_EXEC(TUYA_PRINT("command rejected after overflow"));
                }

                // 准备接收下一个数据包
                dispatcher_status = DISPATCHER_STATUS_WAIT_SYNC_1;
                proc_buf_next_writeable_index = 0;
                expected_byte_count = 0;
            } break;

            case DISPATCHER_STATUS_DUMMY_RECV: {
                expected_byte_count--;
                if (expected_byte_count == 0) {
                    dispatcher_status = DISPATCHER_STATUS_WAIT_SYNC_1;
                    proc_buf_next_writeable_index = 0;
                    expected_byte_count = 0;
                }
            } break;
        }

        continue;
    sync_lost:
        TUYA_DBG_EXEC(TUYA_PRINT("Sync lost, resetting dispatcher state"));
        dispatcher_status = DISPATCHER_STATUS_WAIT_SYNC_1;
        proc_buf_next_writeable_index = 0;
        expected_byte_count = 0;
    }

#endif

    return;
}

/**
 * @brief  协议串口初始化函数
 * @param  Null
 * @return Null
 * @note   在MCU初始化代码中调用该函数
 */
void wifi_protocol_init(void) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error " 请在main函数中添加wifi_protocol_init()完成wifi协议初始化,并删除该行"
#endif

    rx_buf_in = (u8*)wifi_uart_rx_buf;
    rx_buf_out = (u8*)wifi_uart_rx_buf;

    stop_update_flag = DISABLE;

#ifndef WIFI_CONTROL_SELF_MODE
    wifi_work_state = WIFI_SATE_UNKNOW;
#endif
}

#ifndef WIFI_CONTROL_SELF_MODE
/**
 * @brief  MCU获取复位wifi成功标志
 * @param  Null
 * @return 复位标志
 * -           0(RESET_WIFI_ERROR):失败
 * -           1(RESET_WIFI_SUCCESS):成功
 * @note   1:MCU主动调用mcu_reset_wifi()后调用该函数获取复位状态
 *         2:如果为模块自处理模式,MCU无须调用该函数
 */
u8 mcu_get_reset_wifi_flag(void) {
    return reset_wifi_flag;
}

/**
 * @brief  MCU主动重置wifi工作模式
 * @param  Null
 * @return Null
 * @note   1:MCU主动调用,通过mcu_get_reset_wifi_flag()函数获取重置wifi是否成功
 *         2:如果为模块自处理模式,MCU无须调用该函数
 */
void mcu_reset_wifi(void) {
    reset_wifi_flag = RESET_WIFI_ERROR;

    wifi_uart_write_frame(WIFI_RESET_CMD, MCU_TX_VER, 0);
}

/**
 * @brief  获取设置wifi状态成功标志
 * @param  Null
 * @return wifimode flag
 * -           0(SET_WIFICONFIG_ERROR):失败
 * -           1(SET_WIFICONFIG_SUCCESS):成功
 * @note   1:MCU主动调用mcu_set_wifi_mode()后调用该函数获取复位状态
 *         2:如果为模块自处理模式,MCU无须调用该函数
 */
u8 mcu_get_wifimode_flag(void) {
    return set_wifimode_flag;
}

/**
 * @brief  MCU设置wifi工作模式
 * @param[in] {mode} 进入的模式
 * @ref        0(SMART_CONFIG):进入smartconfig模式
 * @ref        1(AP_CONFIG):进入AP模式
 * @return Null
 * @note   1:MCU主动调用
 *         2:成功后,可判断set_wifi_config_state是否为TRUE;TRUE表示为设置wifi工作模式成功
 *         3:如果为模块自处理模式,MCU无须调用该函数
 */
void mcu_set_wifi_mode(u8 mode) {
    u8 length = 0;

    set_wifimode_flag = SET_WIFICONFIG_ERROR;

    length = set_wifi_uart_byte(length, mode);

    wifi_uart_write_frame(WIFI_MODE_CMD, MCU_TX_VER, length);
}

/**
 * @brief  MCU主动获取当前wifi工作状态
 * @param  Null
 * @return wifi work state
 * -          SMART_CONFIG_STATE: smartconfig配置状态
 * -          AP_STATE: AP配置状态
 * -          WIFI_NOT_CONNECTED: WIFI配置成功但未连上路由器
 * -          WIFI_CONNECTED: WIFI配置成功且连上路由器
 * -          WIFI_CONN_CLOUD: WIFI已经连接上云服务器
 * -          WIFI_LOW_POWER: WIFI处于低功耗模式
 * -          SMART_AND_AP_STATE: WIFI smartconfig&AP 模式
 * @note   如果为模块自处理模式,MCU无须调用该函数
 */
u8 mcu_get_wifi_work_state(void) {
    return wifi_work_state;
}
#endif

#ifdef SUPPORT_GREEN_TIME
/**
 * @brief  MCU获取格林时间
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_get_green_time(void) {
    wifi_uart_write_frame(GET_ONLINE_TIME_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/**
 * @brief  MCU获取系统时间,用于校对本地时钟
 * @param  Null
 * @return Null
 * @note   MCU主动调用完成后在mcu_write_rtctime函数内校对rtc时钟
 */
void mcu_get_system_time(void) {
    wifi_uart_write_frame(GET_LOCAL_TIME_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef WIFI_TEST_ENABLE
/**
 * @brief  mcu发起wifi功能测试
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_wifitest(void) {
    wifi_uart_write_frame(WIFI_TEST_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef WIFI_HEARTSTOP_ENABLE
/**
 * @brief  通知WIFI模组关闭心跳
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void wifi_heart_stop(void) {
    wifi_uart_write_frame(HEAT_BEAT_STOP, MCU_TX_VER, 0);
}
#endif

#ifdef GET_WIFI_STATUS_ENABLE
/**
 * @brief  获取当前wifi联网状态
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_get_wifi_connect_status(void) {
    wifi_uart_write_frame(GET_WIFI_STATUS_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  流服务发送
 * @param[in] {id} ID号
 * @param[in] {buffer} 发送包的地址
 * @param[in] {buf_len} 发送包长度
 * @return 流服务传输结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   MCU需要自行实现该功能
 */
u8 stream_trans_send(u32 id, u8* buffer, u32 buf_len) {
    u32 map_offset = 0;
    u32 pack_num = 0;
    u32 rest_length = 0;
    i32 this_len = STREM_PACK_LEN;
    i32 cnt;

    if (stop_update_flag == ENABLE)
        return ERROR;

    pack_num = buf_len / STREM_PACK_LEN;
    rest_length = buf_len - pack_num * STREM_PACK_LEN;
    if (rest_length > 0) {
        pack_num++;
    }

    for (cnt = 0; cnt < pack_num; cnt++, map_offset += this_len) {
        if (cnt == pack_num - 1 && rest_length > 0) {
            this_len = rest_length;
        } else {
            this_len = STREM_PACK_LEN;
        }

        if (ERROR == stream_trans(id, map_offset, buffer + map_offset, this_len)) {
            // mcu正在升级中，不可以进行流服务传输
            // printf("is upgrade\n");
            return ERROR;
        }

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请根据提示，自行完善流服务发送代码，完成后请删除该行"
#endif

        /*  如果MCU可以使用多进程或多线程，可以将此函数放在一个单独的进程或线程里执行，并打开下面所示的代码  */
        /*
        while(0xff == stream_status); //等待模块回复

        if(0x00 != stream_status) {
            //失败  可在 stream_trans_send_result 函数中查看错误码
            return ERROR;
        }
        */

        /*  如果MCU不支持多进程或多线程，使用此函数时每次只发一包（数据长度不大于STREM_PACK_LEN），
        发送完之后在 stream_trans_send_result 函数中处理模块返回的结果，发送成功时再次调用此函数发送下一包，注意包偏移要增加  */
    }

    return SUCCESS;
}

/**
 * @brief  多地图流服务发送
 * @param[in] {id} 地图流服务会话ID
 * @param[in] {sub_id} 子地图ID
 * @param[in] {sub_id_pro_mode} 子地图ID数据处理方式
 * @ref       0x00: 继续累加
 * @ref       0x01: 清除上传的数据
 * @param[in] {buffer} 数据包发送的地址
 * @param[in] {buf_len} 数据包发送的长度
 * @return 流服务传输结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   MCU需要自行实现该功能
 */
u8 maps_stream_trans_send(u32 id, u8 sub_id, u8 sub_id_pro_mode, u8* buffer, u32 buf_len) {
    u32 map_offset = 0;
    u32 pack_num = 0;
    u32 rest_length = 0;
    u8 pro_ver = 0;  // 地图服务协议版本 默认为0
    i32 this_len = STREM_PACK_LEN;
    i32 cnt;

    if (stop_update_flag == ENABLE) return SUCCESS;

    pack_num = buf_len / STREM_PACK_LEN;
    rest_length = buf_len - pack_num * STREM_PACK_LEN;
    if (rest_length > 0) {
        pack_num++;
    }

    for (cnt = 0; cnt < pack_num; cnt++, map_offset += this_len) {
        if (cnt == pack_num - 1 && rest_length > 0) {
            this_len = rest_length;
        } else {
            this_len = STREM_PACK_LEN;
        }

        if (ERROR == maps_stream_trans(pro_ver, id, sub_id, sub_id_pro_mode, map_offset, buffer + map_offset, this_len)) {
            // mcu正在升级中，不可以进行流服务传输
            // printf("is upgrade\n");
            return ERROR;
        }

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请根据提示，自行完善流服务发送代码，完成后请删除该行"
#endif
        /*  如果MCU可以使用多进程或多线程，可以将此函数放在一个单独的进程或线程里执行，并打开下面所示的代码  */
        /*
        while(0xff == maps_stream_status); //等待模块回复

        if(0x00 != maps_stream_status) {
            //失败  可在 maps_stream_trans_send_result 函数中查看错误码
            return ERROR;
        }
        */

        /*  如果MCU不支持多进程或多线程，使用此函数时每次只发一包（数据长度不大于STREM_PACK_LEN），
        发送完之后在 maps_stream_trans_send_result 函数中处理模块返回的结果，发送成功时再次调用此函数发送下一包，注意包偏移要增加  */
    }

    return SUCCESS;
}
#endif

#ifdef WIFI_CONNECT_TEST_ENABLE
/**
 * @brief  mcu发起wifi功能测试(连接指定路由)
 * @param[in] {ssid_buf} 存放路由器名称字符串数据的地址(ssid长度最大支持32个字节)
 * @param[in] {passwd_buffer} 存放路由器名称字符串数据的地址(passwd长度最大支持64个字节)
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_connect_wifitest(u8* ssid_buf, u8* passwd_buffer) {
    u16 send_len = 0;

    if (tuya_strlen(ssid_buf) > 32 || tuya_strlen(passwd_buffer) > 64) {
        // printf("ssid_buf or passwd_buffer is too long!");
        return;
    }

    send_len = set_wifi_uart_buffer(send_len, "{\"ssid\":\"", tuya_strlen("{\"ssid\":\""));
    send_len = set_wifi_uart_buffer(send_len, ssid_buf, tuya_strlen(ssid_buf));
    send_len = set_wifi_uart_buffer(send_len, "\",\"password\":\"", tuya_strlen("\",\"password\":\""));
    send_len = set_wifi_uart_buffer(send_len, passwd_buffer, tuya_strlen(passwd_buffer));
    send_len = set_wifi_uart_buffer(send_len, "\"}", tuya_strlen("\"}"));

    wifi_uart_write_frame(WIFI_CONNECT_TEST_CMD, MCU_TX_VER, send_len);
}
#endif

#ifdef GET_MODULE_MAC_ENABLE
/**
 * @brief  获取模块MAC
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_get_module_mac(void) {
    wifi_uart_write_frame(GET_MAC_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef IR_TX_RX_TEST_ENABLE
/**
 * @brief  mcu发起红外进入收发产测
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_ir_test(void) {
    wifi_uart_write_frame(IR_TX_RX_TEST_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef MODULE_EXPANDING_SERVICE_ENABLE
/**
 * @brief  打开模块时间服务通知
 * @param[in] {time_type} 时间类型
 * @ref       0x00: 格林时间
 * @ref       0x01: 本地时间
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void open_module_time_serve(u8 time_type) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, 0x01);
    send_len = set_wifi_uart_byte(send_len, time_type);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  主动请求天气服务数据
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void request_weather_serve(void) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, 0x03);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  打开模块重置状态通知
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void open_module_reset_state_serve(void) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, 0x04);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}
#endif

#ifdef BLE_RELATED_FUNCTION_ENABLE
/**
 * @brief  mcu发起蓝牙功能性测试（扫描指定蓝牙信标）
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_BLE_test(void) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, 0x01);
    wifi_uart_write_frame(BLE_RELATIVE_CMD, MCU_TX_VER, send_len);
}
#endif

#ifdef VOICE_MODULE_PROTOCOL_ENABLE
/**
 * @brief  获取语音状态码
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void get_voice_state(void) {
    wifi_uart_write_frame(GET_VOICE_STATE_CMD, MCU_TX_VER, 0);
}

/**
 * @brief  MIC静音设置
 * @param[in] {set_val} 静音设置值
 * @ref       0x00: mic开启
 * @ref       0x01: mic静音
 * @ref       0xA0: 查询静音状态
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void set_voice_MIC_silence(u8 set_val) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, set_val);
    wifi_uart_write_frame(MIC_SILENCE_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  speaker音量设置
 * @param[in] {set_val} 音量设置值
 * @ref       0~10: 音量范围
 * @ref       0xA0: 查询音量值
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void set_speaker_voice(u8 set_val) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, set_val);
    wifi_uart_write_frame(SET_SPEAKER_VOLUME_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  音频产测
 * @param[in] {set_val} 音频产测值
 * @ref       0x00: 关闭音频产测
 * @ref       0x01: mic1音频环路测试
 * @ref       0x02: mic2音频环路测试
 * @ref       0xA0: 查询当前产测状态
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_test(u8 test_val) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, test_val);
    wifi_uart_write_frame(VOICE_TEST_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  唤醒产测
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_awaken_test(void) {
    wifi_uart_write_frame(VOICE_AWAKEN_TEST_CMD, MCU_TX_VER, 0);
}

/**
 * @brief  语音模组MCU功能设置
 * @param[in] {play} 播放/暂停功能 1(播放) / 0(暂停)
 * @param[in] {bt_play} 蓝牙开关功能 1(开) / 0(关)
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_mcu_fun_set(u8 play, u8 bt_play) {
    u16 send_len = 0;
    u8 str[50];

    // MCU设置暂仅支持”播放/暂停” ”蓝牙开关”

    sprintf((i8*)str, "{\"play\":%s,\"bt_play\":%s}", play ? "true" : "false", bt_play ? "true" : "false");

    send_len = set_wifi_uart_byte(send_len, 0x00);
    send_len = set_wifi_uart_buffer(send_len, str, tuya_strlen(str));
    wifi_uart_write_frame(VOICE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}
#endif

#if defined(BLE_RELATED_FUNCTION_ENABLE)
/**
 * @brief 查询设备蓝牙连接状态
 */
void query_bt_connect_state() {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, BLE_RELATIVE_SUB_CMD_DEV_STATE_QUERY);
    wifi_uart_write_frame(BLE_RELATIVE_CMD, MCU_TX_VER, send_len);
}
#endif

#if defined(DP_WITH_TYPE_ENABLE)

/**
 * @brief 附类型DP功能设置
 *
 * @param status 0x00: Disable 0x01: Enable
 */
void dp_with_type_enable(u8 status) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, DP_WITH_TYPE_EXTEND_SUB_CMD_START);
    send_len = set_wifi_uart_byte(send_len, status);
    wifi_uart_write_frame(DP_WITH_TYPE_EXTEND_CMD, MCU_TX_VER, send_len);
}

#endif

#if defined(FAN_PRODUCT_SERVICE_ENABLE)
/**
 * @brief 风扇类功能测试
 * 例如：风速步长20，保持时间5，模组依次控制电机0%-20%-40%-60%-80%-100%，每个挡位的间隔时间为5秒
 *
 * @param step 风速步长 (%)
 * @param keep_time 保持时间 (秒)
 */
void fan_product_test_fun_test(u8 step, u8 keep_time) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, FAN_PRODUCT_SERVICE_SUB_CMD_TEST);
    send_len = set_wifi_uart_byte(send_len, step);
    send_len = set_wifi_uart_byte(send_len, keep_time);
    wifi_uart_write_frame(DP_WITH_TYPE_EXTEND_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief 风扇类产品占空比测试
 * 当设置的占空比为70%的时候，PWM1输出70%，PWM2输出0%，检测到过零信号后PWM1输出0%、PWM2输出70%。
 *
 * @param duty 占空比 (%)
 */
void fan_product_test_duty_set(u8 duty) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, FAN_PRODUCT_SERVICE_SUB_CMD_SET_DUTY);
    send_len = set_wifi_uart_byte(send_len, duty);
    wifi_uart_write_frame(DP_WITH_TYPE_EXTEND_CMD, MCU_TX_VER, send_len);
}
#endif

#if defined(CLOUD_STORAGE_FUNCTION_ENABLE)

/**
 * @brief 云存储上传启动
 *
 *
 * @param type     文件类型 0x00: LOG 0x01: NILM
 * @param name     文件名
 * @param len      文件长度 (Byte)
 * @param pkg_size 单包大小 (Byte)
 */
void cloud_storage_start_upload(u8 type, const char* name, u32 len, u16 pkg_size) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 确保缓冲区可以容纳JSON
#endif
    char json_buf[64];
    u16 json_sz = sprintf(json_buf, "{\"type\":%d,\"name\":\"%s\",\"len\":%u,\"segment\":%d}", type, name, len, pkg_size);
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, CLOUD_STONAGE_TRANS_SUB_CMD_UPLOAD_START);
    send_len = set_wifi_uart_buffer(send_len, json_buf, json_sz);
    wifi_uart_write_frame(CLOUD_STRONAGE_TRANS_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief 云存储文件上传
 *
 *
 * @param offset  包偏移 (Byte)
 * @param buf_ptr 指向待上传数据的指针
 * @param length  待上传数据长度 (Byte)
 */
void cloud_storage_data_upload(u32 offset, const u8* buf_ptr, u16 length) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, CLOUD_STONAGE_TRANS_SUB_CMD_UPLOAD_DATA);
    send_len = set_wifi_uart_buffer(send_len, buf_ptr, length);
    wifi_uart_write_frame(CLOUD_STRONAGE_TRANS_CMD, MCU_TX_VER, send_len);
}

#endif

#if defined(AP_TRANSPARENT_TRANS_TEST_ENABLE)
/**
 * @brief AP透传上行数据发送
 *
 *
 * @param buf_ptr 指向待发送数据的指针
 * @param length  待发送数据长度 (Byte)
 */
void ap_transparent_trans_upstream(const u8* buf_ptr, u16 length) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, AP_TRANSPARENT_TRANS_TEST_SUB_CMD_UPSTREAM_DATA);
    send_len = set_wifi_uart_buffer(send_len, buf_ptr, length);
    wifi_uart_write_frame(CLOUD_STRONAGE_TRANS_CMD, MCU_TX_VER, send_len);
}
#endif

#if defined(FACTORY_RECOVERY_FUNCTION_ENABLE)
void request_factory_recovery() {
    wifi_uart_write_frame(FACTORY_RECOVERY_CMD, MCU_TX_VER, 0);
}
#endif

#if defined(HIBERNATE_ENABLE)
/**
 * @brief 请求模块进入休眠模式
 *
 */
void hibernate_enter_request(void) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, HIBERNATE_SUB_CMD_HIBERNATE_REQUEST);
    wifi_uart_write_frame(HIBERNATE_CMD, MCU_TX_VER, send_len);
}
#endif

#if defined(SBUS_SERVICE_ENABLE)

/**
 * @brief 查询软总监状态
 *
 */
void sbus_status_query(void) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, MODULE_EXTEND_FUN_SUB_CMD_SBUS);
    send_len = set_wifi_uart_byte(send_len, 0x01);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief 软总线发送数据
 *
 *
 * @param value         指向待发送数据的指针
 * @param value_length  待发送数据长度
 */
void sbus_send_data(const u8* value, u8 value_length) {
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, MODULE_EXTEND_FUN_SUB_CMD_SBUS);
    send_len = set_wifi_uart_byte(send_len, 0x02);
    send_len = set_wifi_uart_byte(send_len, value_length);
    send_len = set_wifi_uart_buffer(send_len, value, (u16)value_length);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

#endif
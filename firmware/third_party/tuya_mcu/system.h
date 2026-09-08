/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2026, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    system.h
 * @author  涂鸦综合协议开发组
 * @version v2.6.2
 * @date    2026.02.04
 * @brief   串口数据处理，用户无需关心该文件实现内容
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "tuya_type.h"

#ifdef SYSTEM_GLOBAL
  #define SYSTEM_EXTERN
#else
  #define SYSTEM_EXTERN   extern
#endif

//=============================================================================
//Byte order of the frame
//=============================================================================
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1        
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6

//=============================================================================
//Data frame type
//=============================================================================
#define         HEAT_BEAT_CMD                   0                               //心跳包
#define         PRODUCT_INFO_CMD                1                               //产品信息
#define         WORK_MODE_CMD                   2                               //查询MCU 设定的模块工作模式	
#define         WIFI_STATE_CMD                  3                               //wifi工作状态	
#define         WIFI_RESET_CMD                  4                               //重置wifi
#define         WIFI_MODE_CMD                   5                               //选择smartconfig/AP模式	
#define         DATA_QUERT_CMD                  6                               //命令下发
#define         STATE_UPLOAD_CMD                7                               //状态上报	 
#define         STATE_QUERY_CMD                 8                               //状态查询   
#define         UPDATE_START_CMD                0x0a                            //升级开始
#define         UPDATE_TRANS_CMD                0x0b                            //升级传输
#define         GET_ONLINE_TIME_CMD             0x0c                            //获取系统时间(格林威治时间)
#define         FACTORY_MODE_CMD                0x0d                            //进入产测模式    
#define         WIFI_TEST_CMD                   0x0e                            //wifi功能测试
#define         GET_LOCAL_TIME_CMD              0x1c                            //获取本地时间
#define         WEATHER_OPEN_CMD                0x20                            //打开天气          
#define         WEATHER_DATA_CMD                0x21                            //天气数据
#define         STATE_UPLOAD_SYN_CMD            0x22                            //状态上报（同步）
#define         STATE_UPLOAD_SYN_RECV_CMD       0x23                            //状态上报结果通知（同步）
#define         HEAT_BEAT_STOP                  0x25                            //关闭WIFI模组心跳
#define         STREAM_TRANS_CMD                0x28                            //流数据传输
#define         GET_WIFI_STATUS_CMD             0x2b                            //获取当前wifi联网状态
#define         WIFI_CONNECT_TEST_CMD           0x2c                            //wifi功能测试(连接指定路由)
#define         GET_MAC_CMD                     0x2d                            //获取模块mac
#define         GET_IR_STATUS_CMD               0x2e                            //红外状态通知
#define         IR_TX_RX_TEST_CMD               0x2f                            //红外进入收发产测
#define         MAPS_STREAM_TRANS_CMD           0x30                            //流数据传输(支持多张地图)
#define         FILE_DOWNLOAD_START_CMD         0x31                            //文件下载启动
#define         FILE_DOWNLOAD_TRANS_CMD         0x32                            //文件下载数据传输
#define         BLE_RELATIVE_CMD                0x35                            //蓝牙功能性测试（扫描指定蓝牙信标）
#define           BLE_RELATIVE_SUB_CMD_SCAN_TEST                      0x01
#define           BLE_RELATIVE_SUB_CMD_DEV_STATE_REPORT               0x04
#define           BLE_RELATIVE_SUB_CMD_DEV_STATE_QUERY                0x05
#define           BLE_RELATIVE_SUB_CMD_BEACON_RMT_DATA_NOTIFY         0x06
#define           BLE_RELATIVE_SUB_CMD_BEACON_RMT_BIND_UNBIND_NOTIFY  0x07
#define           BLE_RELATIVE_SUB_CMD_SUB_DEV_DATA_NOTIFY            0x08
#define         GET_VOICE_STATE_CMD             0x60                            //获取语音状态码
#define         MIC_SILENCE_CMD                 0x61                            //MIC静音设置
#define         SET_SPEAKER_VOLUME_CMD          0x62                            //speaker音量设置
#define         VOICE_TEST_CMD                  0x63                            //语音模组音频产测
#define         VOICE_AWAKEN_TEST_CMD           0x64                            //语音模组唤醒产测
#define         VOICE_EXTEND_FUN_CMD            0x65                            //语音模组扩展功能


#define         MODULE_EXTEND_FUN_CMD           0x34                            //模块拓展服务
#define           MODULE_EXTEND_FUN_SUB_CMD_START_TIME_NOTIFY   0x01
#define           MODULE_EXTEND_FUN_SUB_CMD_SYNC_TIME           0x02
#define           MODULE_EXTEND_FUN_SUB_CMD_REQUIRE_WEATHER     0x03
#define           MODULE_EXTEND_FUN_SUB_CMD_START_RESET_NOTIFY  0x04
#define           MODULE_EXTEND_FUN_SUB_CMD_RESET_NOTIFY        0x05
#define           MODULE_EXTEND_FUN_SUB_CMD_WIFI_REMOTE         0x20
#define           MODULE_EXTEND_FUN_SUB_CMD_GET_MODULE_INFO     0x07
#define           MODULE_EXTEND_FUN_SUB_CMD_SET_LOG_LEVEL    0x08
#define           MODULE_EXTEND_FUN_SUB_CMD_RECORD_DP_REPORT 0x0B
#define           MODULE_EXTEND_FUN_SUB_CMD_SBUS                0x0C
#define             MODULE_EXTEND_FUN_SUB_CMD_SBUS_CMD_STATE_NOTIFY 0x01
#define             MODULE_EXTEND_FUN_SUB_CMD_SUBS_CMD_DATA_SEND    0x02


#define         DP_WITH_TYPE_EXTEND_CMD                       0x36                            // 附命令DP
#define           DP_WITH_TYPE_EXTEND_SUB_CMD_START           0x01
#define           DP_WITH_TYPE_EXTEND_SUB_CMD_DOWNLOAD        0x02
#define           DP_WITH_TYPE_EXTEND_SUB_CMD_UPLOAD          0x03

#define         PRODUCT_TEST_CMD                                0x39                            // 成品产测
#define           PRODUCT_TEST_SUB_CMD_STATE_NOTIFY             0x00
#define           PRODUCT_TEST_SUB_CMD_KEY_TEST                 0x03
#define           PRODUCT_TEST_SUB_CMD_LED_TEST                 0x04
#define           PRODUCT_TEST_SUB_CMD_TRANSPARENT_TRANS_TEST   0x05
#define           PRODUCT_TEST_SUB_CMD_MODULE_TEST_STATE_NOTIFY 0x06
#define           PRODUCT_TEST_SUB_CMD_TEST_RESULT_REPORT       0x07

#define         CN_IOT_EXTEND_CMD               0x70                            // CN-IOT 扩展命令

#define         FAN_PRODUCT_SERVICE_CMD                 0x72                            // 风扇类产品服务
#define           FAN_PRODUCT_SERVICE_SUB_CMD_TEST      0x01
#define           FAN_PRODUCT_SERVICE_SUB_CMD_SET_DUTY  0x02

#define         USER_DEFINED_CMD                0xEF                            // 用户自定义命令

#define         MATTER_COMMON_CMD               0x66                            // matter类通用固件
#define           MATTER_COMMON_CMD_IDENTIFY_STATE_CHANGE_NOTIFY  0x00
#define           MATTER_COMMON_CMD_EVENT_NOTIFY                  0x01

#define         CLOUD_STRONAGE_TRANS_CMD        0x69                            // 云存储数据传输
#define           CLOUD_STONAGE_TRANS_SUB_CMD_UPLOAD_START        0x01
#define           CLOUD_STONAGE_TRANS_SUB_CMD_UPLOAD_DATA         0x02

#define         AP_TRANSPARENT_TRANS_TEST_CMD   0x80                            // AP透传测试指令
#define           AP_TRANSPARENT_TRANS_TEST_SUB_CMD_UPSTREAM_DATA   0x01
#define           AP_TRANSPARENT_TRANS_TEST_SUB_CMD_DOWNSTREAM_DATA 0x02

#define         FACTORY_RECOVERY_CMD            0xF4                            // 恢复出厂设置

#define         DP_CACHE_GET_CMD                0x90                            // 获取DP缓存指令

#define         HIBERNATE_CMD                   0x91                            // 休眠功能            
#define           HIBERNATE_SUB_CMD_HIBERNATE_NOTIFY  0x00
#define           HIBERNATE_SUB_CMD_WAKEUP_NOTIFY     0x01
#define           HIBERNATE_SUB_CMD_HIBERNATE_REQUEST 0x02


#define         OFFLINE_VOICE_CTRL_CMD            0x95




//=============================================================================
#define MCU_RX_VER              0x00                                            //模块发送帧协议版本号
#define MCU_TX_VER              0x03                                            //MCU 发送帧协议版本号(默认)
#define PROTOCOL_HEAD           0x07                                            //固定协议头长度
#define FRAME_FIRST             0x55                                            //帧头第一字节
#define FRAME_SECOND            0xaa                                            //帧头第二字节
//============================================================================= 



SYSTEM_EXTERN volatile u8 wifi_data_process_buf[PROTOCOL_HEAD + WIFI_DATA_PROCESS_LMT];     //串口数据处理缓存
SYSTEM_EXTERN volatile u8 wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];         //串口接收缓存
SYSTEM_EXTERN volatile u8 wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];        //串口发送缓存
//
SYSTEM_EXTERN volatile u8 *rx_buf_in;
SYSTEM_EXTERN volatile u8 *rx_buf_out;

SYSTEM_EXTERN volatile u8 stop_update_flag;                                                 //ENABLE:停止一切数据上传  TY_DISABLE:恢复一切数据上传

#ifndef WIFI_CONTROL_SELF_MODE
SYSTEM_EXTERN volatile u8 reset_wifi_flag;                                                  //重置wifi标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN volatile u8 set_wifimode_flag;                                                //设置WIFI工作模式标志(TRUE:成功/FALSE:失败)
SYSTEM_EXTERN volatile u8 wifi_work_state;                                                  //wifi模块当前工作状态
#endif

#ifdef WIFI_STREAM_ENABLE
SYSTEM_EXTERN volatile u8 stream_status;                                                             //流服务发包返回状态
SYSTEM_EXTERN volatile u8 maps_stream_status;                                                        //多地图流服务发包返回状态
#endif

/**
 * @brief  写wifi_uart字节
 * @param[in] {dest} 缓存区其实地址
 * @param[in] {byte} 写入字节值
 * @return 写入完成后的总长度
 */
u16 set_wifi_uart_byte(u16 dest, u8 byte);

/**
 * @brief  写wifi_uart_buffer
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {len} 数据长度
 * @return 写入结束的缓存地址
 */
u16 set_wifi_uart_buffer(u16 dest, const u8 *src, u16 len);

/**
 * @brief  计算校验和
 * @param[in] {pack} 数据源指针
 * @param[in] {pack_len} 计算校验和长度
 * @return 校验和
 */
u8 get_check_sum(u8 *pack, u16 pack_len);

/**
 * @brief  向wifi串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_frame(u8 fr_type, u8 fr_ver, u16 len);

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  流数据传输
 * @param[in] {id} 流服务标识
 * @param[in] {offset} 偏移量
 * @param[in] {buffer} 数据地址
 * @param[in] {buf_len} 数据长度
 * @return Null
 * @note   Null
 */
u8 stream_trans(u16 id, u32 offset, u8 *buffer, u16 buf_len);

/**
 * @brief  多地图流数据传输
 * @param[in] {pro_ver} 地图服务协议版本
 * @param[in] {id} 地图流服务会话ID
 * @param[in] {sub_id} 子地图ID
 * @param[in] {sub_id_pro_mode} 子地图ID数据处理方式
 * @ref           0x00:继续累加
 * @ref           0x00:清除上传的数据
 * @param[in] {offset} 偏移量
 * @param[in] {buffer} 数据地址
 * @param[in] {buf_len} 数据长度
 * @return Null
 * @note   Null
 */
u8 maps_stream_trans(u8 pro_ver, u16 id, u8 sub_id, u8 sub_id_pro_mode, 
                                u32 offset, u8 *buffer, u16 buf_len);
#endif

/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void data_handle(u16 offset);

/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据
 */
u8 with_data_rxbuff(void);

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
u8 take_byte_rxbuff(void);
#endif
  
  

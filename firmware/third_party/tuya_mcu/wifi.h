/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2026, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    wifi.h
 * @author  涂鸦综合协议开发组
 * @version v2.6.2
 * @date    2026.02.04
 * @brief   用户无需关心该文件实现内容
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef __WIFI_H_
#define __WIFI_H_

#include "stdio.h"
#include "string.h"
#include "protocol.h"
#include "system.h"
#include "mcu_api.h"
#include "tuya_type.h"

//=============================================================================
//定义常量
//=============================================================================
#ifndef TRUE
#define      TRUE                1
#endif

#ifndef FALSE
#define         FALSE            0
#endif

#ifndef NULL
#define         NULL             ((void *) 0)
#endif

#ifndef SUCCESS
#define         SUCCESS          1
#endif

#ifndef ERROR
#define         ERROR            0
#endif

#ifndef INVALID
#define         INVALID          0xFF
#endif

#ifndef ENABLE
#define         ENABLE           1
#endif

#ifndef DISABLE
#define         DISABLE          0
#endif
//=============================================================================
//dp数据点类型
//=============================================================================
#define         DP_TYPE_RAW                     0x00        //RAW 类型
#define         DP_TYPE_BOOL                    0x01        //bool 类型
#define         DP_TYPE_VALUE                   0x02        //value 类型
#define         DP_TYPE_STRING                  0x03        //string 类型
#define         DP_TYPE_ENUM                    0x04        //enum 类型
#define         DP_TYPE_BITMAP                  0x05        //fault 类型
#define         DP_TYPE_STRUCT                  0x06        // struct 类型
#define         DP_TYPE_ARRAY                   0x07        // array 类型

//=============================================================================
//WIFI工作状态
//=============================================================================
#define         SMART_CONFIG_STATE              0x00
#define         AP_STATE                        0x01
#define         WIFI_NOT_CONNECTED              0x02
#define         WIFI_CONNECTED                  0x03
#define         WIFI_CONN_CLOUD                 0x04
#define         WIFI_LOW_POWER                  0x05
#define         SMART_AND_AP_STATE              0x06
#define         WIFI_SATE_UNKNOW                0xff
//=============================================================================
//wifi配网的方式
//=============================================================================
#define         SMART_CONFIG                    0x0  
#define         AP_CONFIG                       0x1   

//=============================================================================
//wifi复位状态
//=============================================================================
#define         RESET_WIFI_ERROR                0
#define         RESET_WIFI_SUCCESS              1

//=============================================================================
//wifi配置复位状态
//=============================================================================
#define         SET_WIFICONFIG_ERROR            0
#define         SET_WIFICONFIG_SUCCESS          1

//=============================================================================
//MCU固件升级状态
//=============================================================================
#define         FIRM_STATE_UN_SUPPORT           0x00                            //不支持 MCU 升级
#define         FIRM_STATE_WIFI_UN_READY        0x01                            //模块未就绪
#define         FIRM_STATE_GET_ERROR            0x02                            //云端升级信息查询失败
#define         FIRM_STATE_NO                   0x03                            //无需升级（云端无更新版本）
#define         FIRM_STATE_START                0x04                            //需升级，等待模块发起升级操作

//=============================================================================
//WIFI和mcu的工作方式 
//=============================================================================
#define         UNION_WORK                      0x0                             //mcu模块与wifi配合处理
#define         WIFI_ALONE                      0x1                             //wifi模块自处理

//=============================================================================
//系统工作模式
//=============================================================================
#define         NORMAL_MODE                     0x00                            //正常工作状态
#define         FACTORY_MODE                    0x01                            //工厂模式	
#define         UPDATE_MODE                     0x02                            //升级模式	 

//=============================================================================
//配网方式选择
//=============================================================================
#define         CONFIG_MODE_DEFAULT             "0"                             //默认配网方式
#define         CONFIG_MODE_LOWPOWER            "1"                             //低功耗配网方式
#define         CONFIG_MODE_SPECIAL             "2"                             //特殊配网方式  

//=============================================================================
//蓝牙连接状态
//=============================================================================
#define         BT_STATE_UNBINDED_DISCONNECTED  0x00
#define         BT_STATE_UNBINDED_CONNECTED     0x01
#define         BT_STATE_BINDED_DISCONNECTED    0x02
#define         BT_STATE_BINDED_CONNECTED       0x03
#define         BT_STATE_UNKNOWN                0x04



#define UINT32_NTOH(buf) ((u32)((u32) * ((buf) + 0) << 24 | (u32) * ((buf) + 1) << 16 | (u32) * ((buf) + 2) << 8 | (u32) * ((buf) + 3) << 0))
#define UINT32_HTON(buf, val) ((buf)[0] = ((val) >> 24) & 0xFF, (buf)[1] = ((val) >> 16) & 0xFF, (buf)[2] = ((val) >> 8) & 0xFF, (buf)[3] = ((val) >> 0) & 0xFF)

#define UINT16_NTOH(buf) ((u16) * ((buf) + 0) << 8 | ((u16) * ((buf) + 1) << 0))
#define UINT16_HTON(buf, val) ((buf)[0] = ((val) >> 8) & 0xFF, (buf)[1] = ((val) >> 0) & 0xFF)

#if defined(CONFIG_TUYA_ENABLE_TEST)
#define TUYA_PRINT(...)      \
    do {                     \
        printf(__VA_ARGS__); \
        printf("\n");        \
    } while (0)
#else
void _tuya_debug_print(const char* fmt, ...);
#define TUYA_PRINT _tuya_debug_print
#endif

#if defined(CONFIG_TUYA_ENABLE_TEST)
#define TUYA_DBG_EXEC(expr) expr
#else
#define TUYA_DBG_EXEC(expr)
#endif

#if defined(__C51__) || defined(__CX51__)
// idk why a c complier will warn (void)unused;
#define TUYA_UNUSED(var) (var = (var))
#else
#define TUYA_UNUSED(var) ((void)(var))
#endif

#define TUYA_MAX(x, y) ((x) > (y) ? (x) : (y))
#define TUYA_MIN(x, y) ((x) < (y) ? (x) : (y))

//=============================================================================
//下发命令
//=============================================================================
typedef struct {
  u8 dp_id;                              //dp序号
  u8 dp_type;                            //dp类型
} DOWNLOAD_CMD_S;

#endif


#if defined (CONFIG_TUYA_ENABLE_TEST)
#include <stdio.h>
#endif
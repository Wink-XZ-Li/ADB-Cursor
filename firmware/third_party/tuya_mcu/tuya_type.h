/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2026, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    tuya_type.h
 * @author  涂鸦综合协议开发组
 * @version v2.6.2
 * @date    2026.02.04
 * @brief   用户无需关心该文件实现内容
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef  __TUYA_TYPE_H__
#define  __TUYA_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__IAR_SYSTEMS_ICC__) 
    #define VIRTUAL_FUNC __weak
#else 
    #define VIRTUAL_FUNC __attribute__((weak))
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef STATIC
#define STATIC static
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef SIZEOF
#define SIZEOF sizeof
#endif

#ifndef INLINE
#define INLINE inline
#endif


#ifndef NULL
    #ifdef __cplusplus
    #define NULL 0
    #else
    #define NULL ((void *)0)
    #endif
#endif


#ifndef bool_t
typedef unsigned char   bool_t;
#endif

#ifndef i8
typedef signed char   i8;
#endif

#ifndef u8
typedef unsigned char   u8;
#endif

#ifndef i16
typedef signed short   i16;
#endif

#ifndef u16
typedef unsigned short   u16;
#endif

#ifndef i32
typedef signed int  i32;
#endif

#ifndef u32
typedef unsigned int u32;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#if defined(CONFIG_MCU_SDK_TEST_ONLY_USE_NEW_DISPATCHER)

typedef u8 PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_E;
#define PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_WHOLE_COMMAND          0x00   // 消耗掉命令声称的长度的数据但不做处理
#define PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT_AND_RX_BUFFER  0x01   // 丢弃掉当前已接收和接收缓冲区的数据
#define PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT                0x02   // 仅丢弃当前已接收的数据

#endif

#ifdef __cplusplus
}
#endif 

#endif

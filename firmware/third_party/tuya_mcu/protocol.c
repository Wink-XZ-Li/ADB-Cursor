/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2026, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    protocol.c
 * @author  涂鸦综合协议开发组
 * @version v2.6.2
 * @date    2026.02.04
 * @brief
 *                       *******非常重要，一定要看哦！！！********
 *          1. 用户在此文件中实现数据下发/上报功能
 *          2. DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
 *          3. 当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的wifi_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

#include "protocol.h"

#include "wifi.h"

#ifdef WEATHER_ENABLE
/**
 * @var    weather_choose
 * @brief  天气数据参数选择数组
 * @note   用户可以自定义需要的参数，注释或者取消注释即可，注意更改
 */
const i8* weather_choose[WEATHER_CHOOSE_CNT] = {
    "temp",
    "humidity",
    "condition",
    "pm25",
    /*"pressure",
    "realFeel",
    "uvi",
    "tips",
    "windDir",
    "windLevel",
    "windSpeed",
    "sunRise",
    "sunSet",
    "aqi",
    "so2 ",
    "rank",
    "pm10",
    "o3",
    "no2",
    "co",
    "conditionNum",*/
};
#endif

/******************************************************************************
                              第一步:初始化
1:在需要使用到wifi相关文件的文件中include "wifi.h"
2:在MCU初始化中调用mcu_api.c文件中的wifi_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的wifi_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**
******************************************************************************/
#if defined(CONFIG_TUYA_ENABLE_TEST)
#include "dp_var.code_snippet"
#else
const DOWNLOAD_CMD_S download_cmd[] =
{
  {DPID_SWITCH, DP_TYPE_BOOL},
  {DPID_TEMP_SET, DP_TYPE_VALUE},
  {DPID_TEMP_CURRENT, DP_TYPE_VALUE},
  {DPID_MODE, DP_TYPE_ENUM},
  {DPID_FAN_SPEED_ENUM, DP_TYPE_ENUM},
  {DPID_TEMP_UNIT_CONVERT, DP_TYPE_ENUM},
  {DPID_FAULT, DP_TYPE_BITMAP},
  {DPID_TEMP_CURRENT_F, DP_TYPE_VALUE},
  {DPID_TEMP_SET_F, DP_TYPE_VALUE},
  {DPID_SLEEP, DP_TYPE_BOOL},
  {DPID_SCREEN_SAVER, DP_TYPE_BOOL},
  {DPID_USAGE_TIME, DP_TYPE_VALUE},
};

#endif

/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(u8 value) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请将MCU串口发送函数填入该函数,并删除该行"
#endif
#if defined(CONFIG_TUYA_ENABLE_TEST)
    printf("%02X ", value);
#endif
    /*
        //Example:
        extern void Uart_PutChar(u8 value);
        Uart_PutChar(value);	                                //串口发送函数
    */
}

#if defined(CONFIG_MCU_SDK_TEST_ONLY_USE_NEW_DISPATCHER)

PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_E on_process_buffer_overflow(u8 cmd, u16 expected_len) {
    /**
     * 在生产环境下最大接收的命令长度全部已知，出现缓冲区溢出错误往往是不寻常的，可能预示着潜在的攻击风险
     * 如果你的产品有安全性要求，建议在此直接复位模组和MCU
     * 如果要求高可用性，可以返回不同的值来选择不同的处理方式
     *
     * PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_WHOLE_COMMAND         按命令声明的长度消耗整条命令但不处理
     * PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT_AND_RX_BUFFER 放弃当前已接收的数据和接收缓冲区的所有数据
     * PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT               仅放弃当前已接收的数据
     */
    TUYA_UNUSED(cmd);
    TUYA_UNUSED(expected_len);
    return PROCESS_BUFFER_OVERFLOW_HANDLE_METHOD_DISCARD_CURRENT_AND_RX_BUFFER;
}

bool_t on_first_command_after_overflow(const u8* frame, u16 frame_len) {
    /**
     * 在生产环境下，缓冲区溢出可能预示潜在的攻击
     * 缓冲区溢出后，我们无法判断这条命令真正来自模组还是上一条命令的数据段段中的恶意指令
     * 在这个函数中，你有机会判断这些命令是否安全，返回FALSE可以阻止这条命令执行，并在下一条命令执行前继续调用这个函数
     *
     * 例如，如果这条命令试图对MCU进行升级，或置DP值为危险的组合，返回FALSE来阻止命令执行
     *
     * 如果产品对安全性不敏感，直接返回TRUE即可
     */
    TUYA_UNUSED( frame);
    TUYA_UNUSED( frame_len);
    return TRUE;
}


#endif

/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

// 自动化生成数据上报函数

/**
 * @brief  系统所有dp点信息上传,实现APP和muc数据同步
 * @param  Null
 * @return Null
 * @note   此函数SDK内部需调用，MCU必须实现该函数内数据上报功能，包括只上报和可上报可下发型数据
 */
void all_data_update(void){
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请在此处理可下发可上报数据及只上报数据示例,处理完成后删除该行"
#endif
    /*
    //此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数
    mcu_dp_bool_update(DPID_SWITCH,当前开关); //BOOL型数据上报;
    mcu_dp_value_update(DPID_TEMP_SET,当前温度设置); //VALUE型数据上报;
    mcu_dp_value_update(DPID_TEMP_CURRENT,当前当前温度); //VALUE型数据上报;
    mcu_dp_enum_update(DPID_MODE,当前模式); //枚举型数据上报;
    mcu_dp_enum_update(DPID_FAN_SPEED_ENUM,当前风速); //枚举型数据上报;
    mcu_dp_enum_update(DPID_TEMP_UNIT_CONVERT,当前温标切换); //枚举型数据上报;
    mcu_dp_fault_update(DPID_FAULT,当前故障告警); //故障型数据上报;
    mcu_dp_value_update(DPID_TEMP_CURRENT_F,当前当前温度_F); //VALUE型数据上报;
    mcu_dp_value_update(DPID_TEMP_SET_F,当前目标温度_F); //VALUE型数据上报;
    mcu_dp_bool_update(DPID_SLEEP,当前睡眠模式开关); //BOOL型数据上报;
    mcu_dp_bool_update(DPID_SCREEN_SAVER,当前屏保开关); //BOOL型数据上报;
    mcu_dp_value_update(DPID_USAGE_TIME,当前使用时长); //VALUE型数据上报;

    */
}

/******************************************************************************
                                WARNING!!!
                            2:所有数据上报处理
自动化代码模板函数,具体请用户自行实现数据处理
******************************************************************************/
#if defined(CONFIG_TUYA_ENABLE_TEST)
#include "dp_id_handle.code_snippet"
#else
    /*****************************************************************************
函数名称 : dp_download_switch_handle
功能描述 : 针对DPID_SWITCH的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_switch_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为BOOL
    unsigned char ret;
    //0:off/1:on
    unsigned char switch_1;
    
    switch_1 = mcu_get_dp_download_bool(value,length);
    if(switch_1 == 0) {
        //bool off
    }else {
        //bool on
    }
  
    //There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_SWITCH,switch_1);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_temp_set_handle
功能描述 : 针对DPID_TEMP_SET的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_temp_set_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long temp_set;
    
    temp_set = mcu_get_dp_download_value(value,length);
    /*
    //VALUE type data processing
    
    */
    
    //There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TEMP_SET,temp_set);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_mode_handle
功能描述 : 针对DPID_MODE的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_mode_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char mode;
    
    mode = mcu_get_dp_download_enum(value,length);
    switch(mode) {
        case 0:
        break;
        
        case 1:
        break;
        
        case 2:
        break;
        
        case 3:
        break;
        
        default:
    
        break;
    }
    
    //There should be a report after processing the DP
    ret = mcu_dp_enum_update(DPID_MODE, mode);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_fan_speed_enum_handle
功能描述 : 针对DPID_FAN_SPEED_ENUM的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_fan_speed_enum_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char fan_speed_enum;
    
    fan_speed_enum = mcu_get_dp_download_enum(value,length);
    switch(fan_speed_enum) {
        case 0:
        break;
        
        case 1:
        break;
        
        case 2:
        break;
        
        case 3:
        break;
        
        default:
    
        break;
    }
    
    //There should be a report after processing the DP
    ret = mcu_dp_enum_update(DPID_FAN_SPEED_ENUM, fan_speed_enum);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_temp_unit_convert_handle
功能描述 : 针对DPID_TEMP_UNIT_CONVERT的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_temp_unit_convert_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为ENUM
    unsigned char ret;
    unsigned char temp_unit_convert;
    
    temp_unit_convert = mcu_get_dp_download_enum(value,length);
    switch(temp_unit_convert) {
        case 0:
        break;
        
        case 1:
        break;
        
        default:
    
        break;
    }
    
    //There should be a report after processing the DP
    ret = mcu_dp_enum_update(DPID_TEMP_UNIT_CONVERT, temp_unit_convert);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_temp_set_f_handle
功能描述 : 针对DPID_TEMP_SET_F的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_temp_set_f_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为VALUE
    unsigned char ret;
    unsigned long temp_set_f;
    
    temp_set_f = mcu_get_dp_download_value(value,length);
    /*
    //VALUE type data processing
    
    */
    
    //There should be a report after processing the DP
    ret = mcu_dp_value_update(DPID_TEMP_SET_F,temp_set_f);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_sleep_handle
功能描述 : 针对DPID_SLEEP的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_sleep_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为BOOL
    unsigned char ret;
    //0:off/1:on
    unsigned char sleep;
    
    sleep = mcu_get_dp_download_bool(value,length);
    if(sleep == 0) {
        //bool off
    }else {
        //bool on
    }
  
    //There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_SLEEP,sleep);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}
/*****************************************************************************
函数名称 : dp_download_screen_saver_handle
功能描述 : 针对DPID_SCREEN_SAVER的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
static unsigned char dp_download_screen_saver_handle(const unsigned char value[], unsigned short length)
{
    //示例:当前DP类型为BOOL
    unsigned char ret;
    //0:off/1:on
    unsigned char screen_saver;
    
    screen_saver = mcu_get_dp_download_bool(value,length);
    if(screen_saver == 0) {
        //bool off
    }else {
        //bool on
    }
  
    //There should be a report after processing the DP
    ret = mcu_dp_bool_update(DPID_SCREEN_SAVER,screen_saver);
    if(ret == SUCCESS)
        return SUCCESS;
    else
        return ERROR;
}

#endif

/******************************************************************************
                                WARNING!!!
此部分函数用户请勿修改!!
******************************************************************************/

/**
 * @brief  dp下发处理函数
 * @param[in] {dpid} dpid 序号
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {length} dp数据长度
 * @return dp处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   该函数用户不能修改
 */
u8 dp_download_handle(u8 dpid, const u8 value[], u16 length) {
    /*********************************
    当前函数处理可下发/可上报数据调用
    具体函数内需要实现下发数据处理
    完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
    ***********************************/
    u8 ret = SUCCESS;
    switch (dpid) {
#if defined(CONFIG_TUYA_ENABLE_TEST)
#include "dp_handle.code_snippet"
#else
                case DPID_SWITCH:
            //开关处理函数
            ret = dp_download_switch_handle(value,length);
        break;
        case DPID_TEMP_SET:
            //温度设置处理函数
            ret = dp_download_temp_set_handle(value,length);
        break;
        case DPID_MODE:
            //模式处理函数
            ret = dp_download_mode_handle(value,length);
        break;
        case DPID_FAN_SPEED_ENUM:
            //风速处理函数
            ret = dp_download_fan_speed_enum_handle(value,length);
        break;
        case DPID_TEMP_UNIT_CONVERT:
            //温标切换处理函数
            ret = dp_download_temp_unit_convert_handle(value,length);
        break;
        case DPID_TEMP_SET_F:
            //目标温度_F处理函数
            ret = dp_download_temp_set_f_handle(value,length);
        break;
        case DPID_SLEEP:
            //睡眠模式开关处理函数
            ret = dp_download_sleep_handle(value,length);
        break;
        case DPID_SCREEN_SAVER:
            //屏保开关处理函数
            ret = dp_download_screen_saver_handle(value,length);
        break;

#endif
        default:
            break;
    }
    return ret;
}

/**
 * @brief  获取所有dp命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
u8 get_download_cmd_total(void) {
    return (sizeof(download_cmd) / sizeof(download_cmd[0]));
}

/******************************************************************************
                                WARNING!!!
此代码为SDK内部调用,请按照实际dp数据实现函数内部数据
******************************************************************************/

#ifdef SUPPORT_MCU_FIRM_UPDATE
/**
 * @brief  升级包大小选择
 * @param[in] {package_sz} 升级包大小
 * @ref           0x00: 256byte (默认)
 * @ref           0x01: 512byte
 * @ref           0x02: 1024byte
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void upgrade_package_choose(u8 package_sz) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现请自行实现升级包大小选择代码,完成后请删除该行"
#endif
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, package_sz);
    wifi_uart_write_frame(UPDATE_START_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  MCU进入固件升级模式
 * @param[in] {value} 固件缓冲区
 * @param[in] {position} 当前数据包在于固件位置
 * @param[in] {length} 当前固件包长度(固件包长度为0时,表示固件包发送完成)
 * @return Null
 * @note   MCU需要自行实现该功能
 */
u8 mcu_firm_update_handle(const u8 value[], u32 position, u16 length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成MCU固件升级代码,完成后请删除该行"
#endif
    if (length == 0) {
        // 固件数据发送完成

    } else {
        // 固件数据处理
    }

    return SUCCESS;
}
#endif

#ifdef SUPPORT_GREEN_TIME
/**
 * @brief  获取到的格林时间
 * @param[in] {time} 获取到的格林时间数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_get_greentime(u8 time[]) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成相关代码,并删除该行"
#endif
    /*
    time[0] 为是否获取时间成功标志，为 0 表示失败，为 1表示成功
    time[1] 为年份，0x00 表示 2000 年
    time[2] 为月份，从 1 开始到12 结束
    time[3] 为日期，从 1 开始到31 结束
    time[4] 为时钟，从 0 开始到23 结束
    time[5] 为分钟，从 0 开始到59 结束
    time[6] 为秒钟，从 0 开始到59 结束
    */
    if (time[0] == 1) {
        // 正确接收到wifi模块返回的格林数据
        TUYA_DBG_EXEC(TUYA_PRINT("Got Greenwich time 20%02d-%02d-%02d %02d:%02d:%02d", time[1], time[2], time[3], time[4], time[5], time[6]));
    } else {
        // 获取格林时间出错,有可能是当前wifi模块未联网
        TUYA_DBG_EXEC(TUYA_PRINT("Got Greenwich time FAILED!"));
    }
}
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/**
 * @brief  MCU校对本地RTC时钟
 * @param[in] {time} 获取到的格林时间数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_write_rtctime(u8 time[]) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成RTC时钟写入代码,并删除该行"
#endif
    /*
    Time[0] 为是否获取时间成功标志，为 0 表示失败，为 1表示成功
    Time[1] 为年份，0x00 表示 2000 年
    Time[2] 为月份，从 1 开始到12 结束
    Time[3] 为日期，从 1 开始到31 结束
    Time[4] 为时钟，从 0 开始到23 结束
    Time[5] 为分钟，从 0 开始到59 结束
    Time[6] 为秒钟，从 0 开始到59 结束
    Time[7] 为星期，从 1 开始到 7 结束，1代表星期一
   */
    if (time[0] == 1) {
        // 正确接收到wifi模块返回的本地时钟数据
        TUYA_DBG_EXEC(TUYA_PRINT("Got local time 20%02d-%02d-%02d %02d:%02d:%02d weekday: %d", time[1], time[2], time[3], time[4], time[5], time[6], time[7]));
    } else {
        // 获取本地时钟数据出错,有可能是当前wifi模块未联网
        TUYA_DBG_EXEC(TUYA_PRINT("Got Greenwich time FAILED!"));
    }
}
#endif

#ifdef WIFI_TEST_ENABLE
/**
 * @brief  wifi功能测试反馈
 * @param[in] {result} wifi功能测试结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @param[in] {rssi} 测试成功表示wifi信号强度/测试失败表示错误类型
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void wifi_test_result(u8 result, u8 rssi) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现wifi功能测试成功/失败代码,完成后请删除该行"
#endif
    if (result == 0) {
        // 测试失败
        TUYA_DBG_EXEC(TUYA_PRINT("WiFi scan test FAILED!"));
        if (rssi == 0x00) {
            // 未扫描到名称为tuya_mdev_test路由器,请检查
            TUYA_DBG_EXEC(TUYA_PRINT("ap with ssid 'tuya_mdev_test' not found."));
        } else if (rssi == 0x01) {
            // 模块未授权
            TUYA_DBG_EXEC(TUYA_PRINT("module may not be authorized."));
        }
    } else {
        // 测试成功
        // rssi为信号强度(0-100, 0信号最差，100信号最强)
        TUYA_DBG_EXEC(TUYA_PRINT("WiFi scan test pass. Signal strength: %d%%", rssi));
    }
}
#endif

#ifdef WEATHER_ENABLE
/**
 * @brief  mcu打开天气服务
 * @param  Null
 * @return Null
 */
void mcu_open_weather(void) {
    i32 i = 0;
    i8 buffer[13] = {0};
    u8 weather_len = 0;
    u16 send_len = 0;

    weather_len = sizeof(weather_choose) / sizeof(weather_choose[0]);

    for (i = 0; i < weather_len; i++) {
        buffer[0] = sprintf(buffer + 1, "w.%s", weather_choose[i]);
        send_len = set_wifi_uart_buffer(send_len, (u8*)buffer, buffer[0] + 1);
    }

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请根据提示，自行完善打开天气服务代码，完成后请删除该行"
#endif
    /*
    //当获取的参数有和时间有关的参数时(如:日出日落)，需要搭配t.unix或者t.local使用，需要获取的参数数据是按照格林时间还是本地时间
    buffer[0] = sprintf(buffer+1,"t.unix"); //格林时间   或使用  buffer[0] = sprintf(buffer+1,"t.local"); //本地时间
    send_len = set_wifi_uart_buffer(send_len, (u8 *)buffer, buffer[0]+1);
    */

    buffer[0] = sprintf(buffer + 1, "w.date.%d", WEATHER_FORECAST_DAYS_NUM);
    send_len = set_wifi_uart_buffer(send_len, (u8*)buffer, buffer[0] + 1);

    wifi_uart_write_frame(WEATHER_OPEN_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  打开天气功能返回用户自处理函数
 * @param[in] {res} 打开天气功能返回结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @param[in] {err} 错误码
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void weather_open_return_handle(u8 res, u8 err) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成打开天气功能返回数据处理代码,完成后请删除该行"
#endif
    u8 err_num = 0;

    if (res == 1) {
        // 打开天气返回成功
    } else if (res == 0) {
        // 打开天气返回失败
        // 获取错误码
        err_num = err;
    }
}

/**
 * @brief  天气数据用户自处理函数
 * @param[in] {name} 参数名
 * @param[in] {type} 参数类型
 * @ref       0: int 型
 * @ref       1: string 型
 * @param[in] {data} 参数值的地址
 * @param[in] {day} 哪一天的天气  0:表示当天 取值范围: 0~6
 * @ref       0: 今天
 * @ref       1: 明天
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void weather_data_user_handle(i8* name, u8 type, const u8* weather_data, i8 day) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "这里仅给出示例，请自行完善天气数据处理代码,完成后请删除该行"
#endif
    i32 value_int;
    i8 value_string[50];  // 由于有的参数内容较多，这里默认为50。您可以根据定义的参数，可以适当减少该值

    tuya_memset(value_string, '\0', 50);

    // 首先获取数据类型
    if (type == 0) {  // 参数是INT型
        value_int = weather_data[0] << 24 | weather_data[1] << 16 | weather_data[2] << 8 | weather_data[3];
    } else if (type == 1) {
        tuya_strncpy(value_string, weather_data, sizeof(value_string));
    }

    // 注意要根据所选参数类型来获得参数值！！！
    if (tuya_strcmp(name, "temp") == 0) {
        TUYA_DBG_EXEC(TUYA_PRINT("day:%d temp value is:%d\r\n", day, value_int));  // int 型
    } else if (tuya_strcmp(name, "humidity") == 0) {
        TUYA_DBG_EXEC(TUYA_PRINT("day:%d humidity value is:%d\r\n", day, value_int));  // int 型
    } else if (tuya_strcmp(name, "pm25") == 0) {
        TUYA_DBG_EXEC(TUYA_PRINT("day:%d pm25 value is:%d\r\n", day, value_int));  // int 型
    } else if (tuya_strcmp(name, "condition") == 0) {
        TUYA_DBG_EXEC(TUYA_PRINT("day:%d condition value is:%s\r\n", day, value_string));  // string 型
    }
}
#endif

#ifdef MCU_DP_UPLOAD_SYN
/**
 * @brief  状态同步上报结果
 * @param[in] {result} 结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_upload_syn_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成状态同步上报结果代码,并删除该行"
#endif

    if (result == 0) {
        // 同步上报出错
        TUYA_DBG_EXEC(TUYA_PRINT("Sync report dp FAILED."));
    } else {
        // 同步上报成功
        TUYA_DBG_EXEC(TUYA_PRINT("Sync report dp SUCCESSFUL."));
    }
}
#endif

#ifdef GET_WIFI_STATUS_ENABLE
/**
 * @brief  获取 WIFI 状态结果
 * @param[in] {result} 指示 WIFI 工作状态
 * @ref       0x00: wifi状态 1 smartconfig 配置状态
 * @ref       0x01: wifi状态 2 AP 配置状态
 * @ref       0x02: wifi状态 3 WIFI 已配置但未连上路由器
 * @ref       0x03: wifi状态 4 WIFI 已配置且连上路由器
 * @ref       0x04: wifi状态 5 已连上路由器且连接到云端
 * @ref       0x05: wifi状态 6 WIFI 设备处于低功耗模式
 * @ref       0x06: wifi状态 7 WIFI 设备处于smartconfig&AP配置状态
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_wifi_status(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成获取 WIFI 状态结果代码,并删除该行"
#endif
    switch (result) {
        case 0:
            // wifi工作状态1
            TUYA_DBG_EXEC(TUYA_PRINT("wifi under smartconfig mode."));
            break;

        case 1:
            // wifi工作状态2
            TUYA_DBG_EXEC(TUYA_PRINT("wifi under ap config mode."));
            break;

        case 2:
            // wifi工作状态3
            TUYA_DBG_EXEC(TUYA_PRINT("wifi configured but not connected to router."));
            break;

        case 3:
            // wifi工作状态4
            TUYA_DBG_EXEC(TUYA_PRINT("wifi configured and connected to router."));
            break;

        case 4:
            // wifi工作状态5
            TUYA_DBG_EXEC(TUYA_PRINT("wifi connected to cloud."));
            break;

        case 5:
            // wifi工作状态6
            TUYA_DBG_EXEC(TUYA_PRINT("wifi under low power mode."));
            break;

        case 6:
            // wifi工作状态7
            TUYA_DBG_EXEC(TUYA_PRINT("wifi under smartconfig & ap config mode."));
            break;

        default:
            break;
    }
}
#endif

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  流服务发送结果
 * @param[in] {result} 结果
 * @ref       0x00: 成功
 * @ref       0x01: 流服务功能未开启
 * @ref       0x02: 流服务器未连接成功
 * @ref       0x03: 数据推送超时
 * @ref       0x04: 传输的数据长度错误
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void stream_trans_send_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "这里仅给出示例，请自行完善流服务发送结果处理代码,完成后请删除该行"
#endif
    switch (result) {
        case 0x00:
            // 成功
            TUYA_DBG_EXEC(TUYA_PRINT("stream send SUCCESSFUL."));
            break;

        case 0x01:
            // 流服务功能未开启
            TUYA_DBG_EXEC(TUYA_PRINT("stream not enable."));
            break;

        case 0x02:
            // 流服务器未连接成功
            TUYA_DBG_EXEC(TUYA_PRINT("server connect timeout."));
            break;

        case 0x03:
            // 数据推送超时
            TUYA_DBG_EXEC(TUYA_PRINT("data push timeout"));
            break;

        case 0x04:
            // 传输的数据长度错误
            TUYA_DBG_EXEC(TUYA_PRINT("data length invalid"));
            break;

        default:
            break;
    }
}

/**
 * @brief  多地图流服务发送结果
 * @param[in] {result} 结果
 * @ref       0x00: 成功
 * @ref       0x01: 失败
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void maps_stream_trans_send_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "这里仅给出示例，请自行完善多地图流服务发送结果处理代码,完成后请删除该行"
#endif
    switch (result) {
        case 0x00:
            // 成功
            TUYA_DBG_EXEC(TUYA_PRINT("map stream send SUCCESSFUL."));
            break;

        case 0x01:
            // 失败
            TUYA_DBG_EXEC(TUYA_PRINT("map stream send FAILED."));
            break;

        default:
            break;
    }
}
#endif

#ifdef WIFI_CONNECT_TEST_ENABLE
/**
 * @brief  路由信息接收结果通知
 * @param[in] {result} 模块是否成功接收到正确的路由信息
 * @ref       0x00: 失败
 * @ref       0x01: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void wifi_connect_test_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现wifi功能测试成功/失败代码,完成后请删除该行"
#endif
    if (result == 0) {
        // 路由信息接收失败，请检查发出的路由信息包是否是完整的JSON数据包
        TUYA_DBG_EXEC(TUYA_PRINT("wifi connect test FAILED."));
    } else {
        // 路由信息接收成功，产测结果请注意WIFI_STATE_CMD指令的wifi工作状态
        TUYA_DBG_EXEC(TUYA_PRINT("wifi connect test PASSED."));
    }
}
#endif

#ifdef GET_MODULE_MAC_ENABLE
/**
 * @brief  获取模块mac结果
 * @param[in] {mac} 模块 MAC 数据
 * @ref       mac[0]: 为是否获取mac成功标志，0x00 表示成功，0x01 表示失败
 * @ref       mac[1]~mac[6]: 当获取 MAC地址标志位如果mac[0]为成功，则表示模块有效的MAC地址
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_get_mac(u8 mac[]) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成mac获取代码,并删除该行"
#endif
    /*
    mac[0]为是否获取mac成功标志，0x00 表示成功，为0x01表示失败
    mac[1]~mac[6]:当获取 MAC地址标志位如果mac[0]为成功，则表示模块有效的MAC地址
   */

    if (mac[0] == 1) {
        // 获取mac出错
        TUYA_DBG_EXEC(TUYA_PRINT("FAILED to get module MAC addr."));
    } else {
        // 正确接收到wifi模块返回的mac地址
        TUYA_DBG_EXEC(TUYA_PRINT("Got module mac addr 02X:02X:02X:02X:02X:02X.", mac[1], mac[2], mac[3], mac[4], mac[5], mac[6]));
    }
}
#endif

#ifdef GET_IR_STATUS_ENABLE
/**
 * @brief  获取红外状态结果
 * @param[in] {result} 指示红外状态
 * @ref       0x00: 红外状态 1 正在发送红外码
 * @ref       0x01: 红外状态 2 发送红外码结束
 * @ref       0x02: 红外状态 3 红外学习开始
 * @ref       0x03: 红外状态 4 红外学习结束
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_ir_status(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成红外状态代码,并删除该行"
#endif
    switch (result) {
        case 0:
            // 红外状态 1
            TUYA_DBG_EXEC(TUYA_PRINT("IR sending"));
            break;

        case 1:
            // 红外状态 2
            TUYA_DBG_EXEC(TUYA_PRINT("IR send finish"));
            break;

        case 2:
            // 红外状态 3
            TUYA_DBG_EXEC(TUYA_PRINT("IR learn start"));
            break;

        case 3:
            // 红外状态 4
            TUYA_DBG_EXEC(TUYA_PRINT("IR learn end"));
            break;

        default:
            break;
    }

    wifi_uart_write_frame(GET_IR_STATUS_CMD, MCU_TX_VER, 0);
}
#endif

#ifdef IR_TX_RX_TEST_ENABLE
/**
 * @brief  红外进入收发产测结果通知
 * @param[in] {result} 模块是否成功接收到正确的信息
 * @ref       0x00: 失败
 * @ref       0x01: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void ir_tx_rx_test_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现红外进入收发产测功能测试成功/失败代码,完成后请删除该行"
#endif
    if (result == 0) {
        // 红外进入收发产测成功
    } else {
        // 红外进入收发产测失败，请检查发出的数据包
    }
}
#endif

#ifdef FILE_DOWNLOAD_ENABLE
/**
 * @brief  文件下载包大小选择
 * @param[in] {package_sz} 文件下载包大小
 * @ref       0x00: 256 byte (默认)
 * @ref       0x01: 512 byte
 * @ref       0x02: 1024 byte
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void file_download_package_choose(u8 package_sz) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现请自行实现文件下载包大小选择代码,完成后请删除该行"
#endif
    u16 send_len = 0;
    send_len = set_wifi_uart_byte(send_len, package_sz);
    wifi_uart_write_frame(FILE_DOWNLOAD_START_CMD, MCU_TX_VER, send_len);
}

/**
 * @brief  文件包下载模式
 * @param[in] {value} 数据缓冲区
 * @param[in] {position} 当前数据包在于文件位置
 * @param[in] {length} 当前文件包长度(长度为0时,表示文件包发送完成)
 * @return 数据处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   MCU需要自行实现该功能
 */
u8 file_download_handle(const u8 value[], u32 position, u16 length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行完成文件包下载代码,完成后请删除该行"
#endif
    if (length == 0) {
        // 文件包数据发送完成

    } else {
        // 文件包数据处理
    }

    return SUCCESS;
}
#endif

#ifdef MODULE_EXPANDING_SERVICE_ENABLE
/**
 * @brief  打开模块时间服务通知结果
 * @param[in] {value} 数据缓冲区
 * @param[in] {length} 数据长度
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void open_module_time_serve_result(const unsigned char value[], unsigned short length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现模块时间服务通知结果代码,完成后请删除该行"
#endif

    if (0x02 != length) {
        // 数据长度错误
        return;
    }

    if (value[1] == 0) {
        // 服务开启成功
    } else {
        // 服务开启失败
    }
}

void module_time_sync_handler(const unsigned char cmd_bytes[], unsigned short cmd_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 在此处完善时间同步代码(0x3402)
#endif
    if (0x09 != cmd_length) {
        // 数据长度错误
        return;
    }

    unsigned char time_type = cmd_bytes[1];  // 0x00:格林时间  0x01:本地时间
    unsigned char time_data[7];

    tuya_memcpy(time_data, cmd_bytes + 2, cmd_length - 2);
    /*
    Data[0]为年份, 0x00表示2000年
    Data[1]为月份，从1开始到12结束
    Data[2]为日期，从1开始到31结束
    Data[3]为时钟，从0开始到23结束
    Data[4]为分钟，从0开始到59结束
    Data[5]为秒钟，从0开始到15结束
    Data[6]为星期，从1开始到7结束，1代表星期一
    */

    TUYA_DBG_EXEC(TUYA_PRINT("Got local time 20%02d-%02d-%02d %02d:%02d:%02d weekday: %d",
                  time_data[0], time_data[1], time_data[2], time_data[3], time_data[4], time_data[5], time_data[6]));

    // 在此处添加时间数据处理代码，time_type为时间类型

    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len, cmd_length);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

void request_weather_handler(const unsigned char cmd_datas[], unsigned short cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 主动请求天气服务数据结果返回，完成后删除该error指令
#endif

    u8 result = cmd_datas[1];
    if (result == 0x00) {
        // 成功
        TUYA_DBG_EXEC(TUYA_PRINT("request weather data SUCCESSFUL"));
    } else {
        // 失败
        TUYA_DBG_EXEC(TUYA_PRINT("request weather data FAILED"));
    }
}

void module_start_reset_notify_handler(const unsigned char cmd_bytes[], unsigned short cmd_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 打开模组重置通知结果，完成后删除该error指令
#endif

    if (0x02 != cmd_length) {
        // 数据长度错误
        return;
    }

    if (cmd_bytes[1] == 0) {
        // 成功
    } else {
        // 失败
    }
}

void module_reset_notify_handler(const unsigned char cmd_bytes[], unsigned short cmd_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 模组重置通知，完成后删除该error指令
#endif

    if (0x02 != cmd_length) {
        // 数据长度错误
        return;
    }

    switch (cmd_bytes[1]) {
        case 0x00:
            // 模块本地重置

            break;
        case 0x01:
            // APP远程重置

            break;
        case 0x02:
            // APP恢复出厂重置

            break;
        default:
            break;
    }

    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len, MODULE_EXTEND_FUN_SUB_CMD_START_RESET_NOTIFY);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

void module_wifi_remote_handler(const unsigned char cmd_datas[], unsigned short cmd_data_length) {
    u8 type = cmd_datas[1];
    u8 cmd = cmd_datas[2];
    u8 cmd_data = cmd_datas[3];
    u16 send_len = 0;
    switch (type) {
        case 0xFF:
            // 通用品类
            break;

        case 0x01:
            // 照明
            break;

        default:
            break;
    }

    send_len = set_wifi_uart_byte(send_len, MODULE_EXTEND_FUN_SUB_CMD_WIFI_REMOTE);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
}

void get_module_info_handler(const unsigned char cmd_datas[], unsigned short cmd_data_length) {
    // 获取所有当前支持的数据信息	0xff
    // AP ssid名称	0x01
    // 国家码	0x02
    // SN信息	0x03
    // FFS授权记录	0x04
    // 模组版本号	0x05
    // WiFi配置信息	0x06
    // uuid信息	0x07
    // 固件key信息	0x08

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 获取WiFi模组信息返回，完成后删除该error指令
#endif

    u8 result = cmd_datas[1];
    const char* json_buf = &cmd_datas[2];

    if (result == 0x01) {
        // 失败

        return;
    }

    // TODO: 此处解析jsonbuf中的json来获取返回
    /*
    {
      "ap":    "xxxxx";
      "cc":    xx;
      "sn":    "xxxx";
      "ffs":   x,
      "sw":    "x.x.x",
      "wcfg": { "ssid": "xxxx", "pwd": "xxxx", "uuid": "xxxx", "key": "xxxx" }
    }
    */
}

void set_module_log_level_handler(const unsigned char cmd_datas[], unsigned short cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 调整模组日志等级结果，完成后删除该error指令
#endif

    u8 result = cmd_datas[1];
    if (result == 0x00) {
        // 成功
        TUYA_DBG_EXEC(TUYA_PRINT("Adjust module log level SUCCESSFUL."));
    } else {
        // 失败
        TUYA_DBG_EXEC(TUYA_PRINT("Adjust module log level FAILED."));
    }
}

#if defined(RECORD_TYPE_DP_ENABLE)

void module_record_dp_report_handler(const unsigned char cmd_datas[], unsigned short cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 记录型DP上报结果，完成后删除该error指令
#endif

    u8 result = cmd_datas[1];
    switch (result) {
        case 0x00:
            // 成功上报到云端
            break;

        case 0x01:
            // 上报成功且滞留
            break;

        case 0x02:
            // 上报失败
            break;
        case 0x03:
            // 数据内容非法

            break;
    }
}

#endif

#if defined(SBUS_SERVICE_ENABLE)
void sbus_service_handler(const unsigned char cmd_datas[], unsigned short cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 自行完善软总线服务，完成后删除该error指令
#endif

    u8 sbus_cmd = cmd_datas[1];
    u8 result;
    u8 data_length;
    const u8* data_ptr;
    const u8* data_end;
    switch (sbus_cmd) {
        case 0x01:
            // 状态通知
            result = cmd_datas[2];
            TUYA_DBG_EXEC(TUYA_PRINT("sbus state change to: %d", result));
            switch (result) {
                case 0x00:
                    // 空闲
                    TUYA_DBG_EXEC(TUYA_PRINT("Idle"));
                    break;
                case 0x01:
                    // 绑定
                    TUYA_DBG_EXEC(TUYA_PRINT("Bind"));
                    break;
                case 0x02:
                    // 连接
                    TUYA_DBG_EXEC(TUYA_PRINT("Connected"));
                    break;
                case 0x03:
                    // 断联
                    TUYA_DBG_EXEC(TUYA_PRINT("Disconnected"));
                    break;
                case 0x04:
                    // 解绑
                    TUYA_DBG_EXEC(TUYA_PRINT("Unbind"));
                    break;
            }
            break;

        case 0x02:
            if (cmd_data_length == 3) {
                // 发送反馈
                result = cmd_datas[2];
                if (result == 0x00) {
                    // 成功
                    TUYA_DBG_EXEC(TUYA_PRINT("sbus send SUCCESSFUL."));
                } else {
                    // 失败
                    TUYA_DBG_EXEC(TUYA_PRINT("sbus send FAILED."));
                }
            } else {
                // 接受到消息
                data_end = &cmd_datas[0] + cmd_data_length;
                while (data_ptr < data_end) {
                    data_length = *data_ptr;
                    data_ptr += 1;
                    // TODO: 处理本条消息

                    data_ptr += data_length;
                }
            }
            break;
        default:
            TUYA_DBG_EXEC(TUYA_PRINT("Unknownm sbus command %02X", sbus_cmd));
            break;
    }
}

#endif

#endif

#ifdef BLE_RELATED_FUNCTION_ENABLE
/**
 * @brief  蓝牙功能性测试结果
 * @param[in] {value} 数据缓冲区
 * @param[in] {length} 数据长度
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void BLE_test_result(const u8 value[], u16 length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现蓝牙功能性测试结果代码,完成后请删除该行"
#endif

    u8 sub_cmd = value[0];
    u8 result = value[1];
    u8 rssi = value[2];

    if (0x03 != length) {
        // 数据长度错误
        return;
    }

    if (0x01 != sub_cmd) {
        // 子命令错误
        return;
    }

    if (result == 0) {
        // 测试失败
        if (rssi == 0x00) {
            // 未扫描到名称为 ty_mdev蓝牙信标,请检查
        } else if (rssi == 0x01) {
            // 模块未授权
        }
    } else if (result == 0x01) {
        // 测试成功
        // rssi为信号强度(0-100, 0信号最差，100信号最强)
    }
}

void ble_connect_state_report(const u8 cmd_datas[], u16 cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 此处处理模组主动发送的蓝牙连接状态,完成后请删除该行
#endif
    u8 state = cmd_datas[1];
    u16 send_len = 0;
    TUYA_DBG_EXEC(TUYA_PRINT("BT state change to %02X", state));
    switch (state) {
        case BT_STATE_UNBINDED_DISCONNECTED:
            break;
        case BT_STATE_UNBINDED_CONNECTED:
            break;
        case BT_STATE_BINDED_DISCONNECTED:
            break;
        case BT_STATE_BINDED_CONNECTED:
            break;
        case BT_STATE_UNKNOWN:
            break;
    }
    send_len = set_wifi_uart_byte(send_len, BLE_RELATIVE_SUB_CMD_DEV_STATE_REPORT);
    wifi_uart_write_frame(BLE_RELATIVE_CMD, MCU_TX_VER, send_len);
}

void ble_connect_state_query_result(const u8 cmd_datas[], u16 cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 此处处理查询返回的蓝牙连接状态,完成后请删除该行
#endif
    u8 state = cmd_datas[1];
    TUYA_DBG_EXEC(TUYA_PRINT("Got BT state %02X", state));
    switch (state) {
        case BT_STATE_UNBINDED_DISCONNECTED:
            break;
        case BT_STATE_UNBINDED_CONNECTED:
            break;
        case BT_STATE_BINDED_DISCONNECTED:
            break;
        case BT_STATE_BINDED_CONNECTED:
            break;
        case BT_STATE_UNKNOWN:
            break;
    }
}

void ble_beacon_remote_data_notify(const u8 cmd_datas[], u16 cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 此处处理蓝牙beacon遥控器数据，详细定义参考文档，完成后请删除该行
#endif
    const u8 category_id = cmd_datas[1];
    const u8 ctrl_cmd = cmd_datas[2];
    const u8 ctrl_datas[4] = {cmd_datas[3], cmd_datas[4], cmd_datas[5], cmd_datas[6]};

    TUYA_DBG_EXEC(TUYA_PRINT("beacon remote cmd: category_id: %02X, cmd: %02X, data: %02X %02X %02X %02X",
                  category_id, ctrl_cmd, ctrl_datas[0], ctrl_datas[1], ctrl_datas[2], ctrl_datas[3]));
    // TODO
}

void ble_beacon_remote_bind_unbind_notify(const u8 cmd_datas[], u16 cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 此处处理蓝牙beacon遥控器绑定解绑事件，完成后请删除该行
#endif
    const u8 event = cmd_datas[1];     // 0x00: 解绑  0x01: 绑定
    const u8 group_id = cmd_datas[2];  // 群组id
    // TODO
}

void ble_sub_dev_data_notify(const u8 cmd_datas[], u16 cmd_data_length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 此处处理蓝牙子设备数据，完成后请删除该行
#endif
    const u8 mac_addr[6] = {cmd_datas[1], cmd_datas[2], cmd_datas[3], cmd_datas[4], cmd_datas[5], cmd_datas[6]};
    const u32 seq = UINT32_NTOH(&cmd_datas[7]);
    const u8* ptr_to_data = &cmd_datas[11];
    const u16 data_length = cmd_data_length - 11;
    // TODO: ...
}

void BLE_relative_fun(const u8 cmd_datas[], u16 cmd_data_length) {
    u8 sub_cmd = cmd_datas[0];
    switch (sub_cmd) {
        case BLE_RELATIVE_SUB_CMD_SCAN_TEST:
            BLE_test_result(cmd_datas, cmd_data_length);
            break;
        case BLE_RELATIVE_SUB_CMD_DEV_STATE_REPORT:
            ble_connect_state_report(cmd_datas, cmd_data_length);
            break;
        case BLE_RELATIVE_SUB_CMD_DEV_STATE_QUERY:
            ble_connect_state_query_result(cmd_datas, cmd_data_length);
            break;
        case BLE_RELATIVE_SUB_CMD_BEACON_RMT_DATA_NOTIFY:
            ble_beacon_remote_data_notify(cmd_datas, cmd_data_length);
            break;
        case BLE_RELATIVE_SUB_CMD_BEACON_RMT_BIND_UNBIND_NOTIFY:
            ble_beacon_remote_bind_unbind_notify(cmd_datas, cmd_data_length);
            break;
        case BLE_RELATIVE_SUB_CMD_SUB_DEV_DATA_NOTIFY:
            ble_sub_dev_data_notify(cmd_datas, cmd_data_length);
            break;
    }
}

#endif

#ifdef VOICE_MODULE_PROTOCOL_ENABLE
/**
 * @brief  获取语音状态码结果
 * @param[in] {result} 语音状态码
 * @ref       0x00: 空闲
 * @ref       0x01: mic静音状态
 * @ref       0x02: 唤醒
 * @ref       0x03: 正在录音
 * @ref       0x04: 正在识别
 * @ref       0x05: 识别成功
 * @ref       0x06: 识别失败
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_voice_state_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现获取语音状态码结果处理代码,完成后请删除该行"
#endif

    TUYA_DBG_EXEC(TUYA_PRINT("voice state: %d", result));
    switch (result) {
        case 0:
            // 空闲
            TUYA_DBG_EXEC(TUYA_PRINT("idle."));
            break;

        case 1:
            // mic静音状态
            TUYA_DBG_EXEC(TUYA_PRINT("mic silence."));
            break;

        case 2:
            // 唤醒
            TUYA_DBG_EXEC(TUYA_PRINT("wakeup."));
            break;

        case 3:
            // 正在录音
            TUYA_DBG_EXEC(TUYA_PRINT("recording."));
            break;

        case 4:
            // 正在识别
            TUYA_DBG_EXEC(TUYA_PRINT("regonizing."));
            break;

        case 5:
            // 识别成功
            TUYA_DBG_EXEC(TUYA_PRINT("regonize SUCCESSFUL."));
            break;

        case 6:
            // 识别失败
            TUYA_DBG_EXEC(TUYA_PRINT("regonize FAILED."));
            break;

        default:
            break;
    }
}

/**
 * @brief  MIC静音设置结果
 * @param[in] {result} 语音状态码
 * @ref       0x00: mic 开启
 * @ref       0x01: mic 静音
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void set_voice_MIC_silence_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现MIC静音设置处理代码,完成后请删除该行"
#endif
    if (result == 0) {
        // mic 开启
        TUYA_DBG_EXEC(TUYA_PRINT("MIC enable"));
    } else {
        // mic 静音
        TUYA_DBG_EXEC(TUYA_PRINT("MIC disable"));
    }
}

/**
 * @brief  speaker音量设置结果
 * @param[in] {result} 音量值
 * @ref       0~10: 音量范围
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void set_speaker_voice_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现speaker音量设置结果处理代码,完成后请删除该行"
#endif
}

/**
 * @brief  音频产测结果
 * @param[in] {result} 音频产测状态
 * @ref       0x00: 关闭音频产测
 * @ref       0x01: mic1音频环路测试
 * @ref       0x02: mic2音频环路测试
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void voice_test_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现音频产测结果处理代码,完成后请删除该行"
#endif
    if (result == 0x00) {
        // 关闭音频产测
    } else if (result == 0x01) {
        // mic1音频环路测试
    } else if (result == 0x02) {
        // mic2音频环路测试
    }
}

/**
 * @brief  唤醒产测结果
 * @param[in] {result} 唤醒返回值
 * @ref       0x00: 唤醒成功
 * @ref       0x01: 唤醒失败(10s超时失败)
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void voice_awaken_test_result(u8 result) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现唤醒产测结果处理代码,完成后请删除该行"
#endif
    if (result == 0x00) {
        // 唤醒成功
    } else if (result == 0x01) {
        // 唤醒失败
    }
}

/**
 * @brief  语音模组扩展功能
 * @param[in] {value} 数据缓冲区
 * @param[in] {length} 数据长度
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void voice_module_extend_fun(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    u8 play;
    u8 bt_play;
    u16 send_len = 0;

    switch (sub_cmd) {
        case 0x00: {  // 子命令  MCU功能设置
            if (0x02 != length) {
                // 数据长度错误
                return;
            }

            if (value[1] == 0) {
                // 成功
            } else {
                // 失败
            }
        } break;

        case 0x01: {  // 子命令  状态通知
            if (0x02 > length) {
                // 数据长度错误
                return;
            }

            u8 play = 0xff;
            u8 bt_play = 0xff;

            const i8* str_buff = (const i8*)&value[1];
            const i8* str_result = NULL;

            str_result = strstr(str_buff, "play") + tuya_strlen("play") + 2;
            if (NULL == str_result) {
                // 数据错误
                goto ERR_EXTI;
            }

            if (0 == memcmp(str_result, "true", tuya_strlen("true"))) {
                play = 1;
            } else if (0 == memcmp(str_result, "false", tuya_strlen("false"))) {
                play = 0;
            } else {
                // 数据错误
                goto ERR_EXTI;
            }

            str_result = strstr(str_buff, "bt_play") + tuya_strlen("bt_play") + 2;
            if (NULL == str_result) {
                // 数据错误
                goto ERR_EXTI;
            }

            if (0 == memcmp(str_result, "true", tuya_strlen("true"))) {
                bt_play = 1;
            } else if (0 == memcmp(str_result, "false", tuya_strlen("false"))) {
                bt_play = 0;
            } else {
                // 数据错误
                goto ERR_EXTI;
            }
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error "请自行实现语音模组状态通知处理代码,完成后请删除该行"
#endif
            // MCU设置暂仅支持”播放/暂停” ”蓝牙开关”
            // play    播放/暂停功能  1(播放) / 0(暂停)
            // bt_play 蓝牙开关功能   1(开)   / 0(关)

            send_len = 0;
            send_len = set_wifi_uart_byte(send_len, sub_cmd);
            send_len = set_wifi_uart_byte(send_len, 0x00);
            wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
        } break;

        default:
            break;
    }

    return;

ERR_EXTI:
    send_len = 0;
    send_len = set_wifi_uart_byte(send_len, sub_cmd);
    send_len = set_wifi_uart_byte(send_len, 0x01);
    wifi_uart_write_frame(MODULE_EXTEND_FUN_CMD, MCU_TX_VER, send_len);
    return;
}
#endif

#if defined(DP_WITH_TYPE_ENABLE)
void dp_type_extended_result(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    u8 result;
    u8 source;
    u16 send_len = 0;
    const u8* data_ptr;
    const u8* data_end;
    u16 data_len;
    u8 dp_id;
    u8 dp_type;

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 在此实现附类型DP相关逻辑
#endif

    switch (sub_cmd) {
        case DP_WITH_TYPE_EXTEND_SUB_CMD_START:
            result = value[DATA_START + 1];
            if (result == 0x00) {
                // 操作成功

            } else if (result == 0x01) {
                // 操作失败
            }
            break;
        case DP_WITH_TYPE_EXTEND_SUB_CMD_DOWNLOAD:
            // 处理DP下发
            source = value[DATA_START + 2];
            switch (source) {
                case 0x00:  // 未知来源
                    break;
                case 0x01:  // 局域网
                    break;
                case 0x02:  // 广域网
                    break;
                case 0x03:  // 本地定时
                    break;
                case 0x04:  // 广域网场景联动
                    break;
                case 0x05:  // 可靠的信道
                    break;
                case 0x06:  // 蓝牙
                    break;
                case 0x07:  // 本地场景联动
                    break;
                case 0xF0:  // 离线语音模组
                    break;
            }
            data_end = &value[DATA_START + length];
            data_ptr = &value[DATA_START + 2];
            while (data_ptr < data_end) {
                dp_id = *data_ptr;
                data_ptr++;
                dp_type = *data_ptr;
                data_ptr++;
                data_len = (u16)data_ptr[0] << 8 | data_ptr[1];
                data_ptr += 2;
                dp_download_handle(dp_id, data_ptr, data_len);
                data_ptr += data_len;
            }

            break;
    }
}
#endif

#if defined(PRODUCT_TEST_ENABLE)

void product_test_fun(const u8 value[], u16 length) {
#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 产测相关逻辑和实际产品强相关，此处不做具体实现。请用户根据产品实际情况完成相关逻辑。
#endif

    u8 sub_cmd = value[0];
    u16 send_len = 0;
    u16 buf_len;

    u8 result;
    char json_buf[32];
    switch (sub_cmd) {
        case PRODUCT_TEST_SUB_CMD_STATE_NOTIFY:
            switch (value[1]) {
                case 0x00:
                    // 进入产测
                    TUYA_DBG_EXEC(TUYA_PRINT("enter product test."));
                    break;
                case 0x01:
                    // 退出产测
                    TUYA_DBG_EXEC(TUYA_PRINT("exit product test."));
                    break;
            }
            send_len = set_wifi_uart_byte(send_len, sub_cmd);
            send_len = set_wifi_uart_byte(send_len, value[1]);
            send_len = set_wifi_uart_byte(send_len, 0x00);
            wifi_uart_write_frame(PRODUCT_TEST_CMD, MCU_TX_VER, send_len);
            break;
        case PRODUCT_TEST_SUB_CMD_KEY_TEST:
            TUYA_DBG_EXEC(TUYA_PRINT("key test."));

            break;
        case PRODUCT_TEST_SUB_CMD_LED_TEST:
            TUYA_DBG_EXEC(TUYA_PRINT("led test."));
            send_len = set_wifi_uart_byte(send_len, sub_cmd);

            break;
        case PRODUCT_TEST_SUB_CMD_TRANSPARENT_TRANS_TEST:
            break;
        case PRODUCT_TEST_SUB_CMD_MODULE_TEST_STATE_NOTIFY:
            break;
        case PRODUCT_TEST_SUB_CMD_TEST_RESULT_REPORT:
            break;
        default:
            TUYA_DBG_EXEC(TUYA_PRINT("Unknown subcmd %02X", sub_cmd));
            break;
    }
}

#endif

#if defined(CN_IOT_EXTENDED_ENABLE)

void cn_iot_extended_fun(const u8 value[], u16 length) {
    TUYA_UNUSED(value);
    TUYA_UNUSED(length);
}

#endif

#if defined(FAN_PRODUCT_SERVICE_ENABLE)

void fan_product_service_fun(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    u8 result;
    switch (sub_cmd) {
        case FAN_PRODUCT_SERVICE_SUB_CMD_TEST:  // 风扇功能测试
            result = value[1];
            if (result == 0x00) {
                TUYA_DBG_EXEC(TUYA_PRINT("Fan product test OK"));
                // 成功
            } else {
                // 失败
                TUYA_DBG_EXEC(TUYA_PRINT("Fan product test FAILED"));
            }
            break;
        case FAN_PRODUCT_SERVICE_SUB_CMD_SET_DUTY:  // 占空比测试
            result = value[1];
            if (result == 0x00) {
                // 成功
                TUYA_DBG_EXEC(TUYA_PRINT("Fan product test duty OK"));

            } else {
                // 失败
                TUYA_DBG_EXEC(TUYA_PRINT("Fan product test duty FAILED"));
            }
            break;
    }
}
#endif

#if defined(USER_DEFINE_COMMAND_ENABLE)

void user_defined_fun(const u8 value[], u16 length) {
    TUYA_UNUSED(value);
    TUYA_UNUSED(length);
    // 用户自定义命令
}

#endif

#if defined(MATTER_COMMON_FUNCTION_ENABLE)

void matter_common_fun(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    u8 status;
    u8 result;
    u16 send_len = 0;

    switch (sub_cmd) {
        case MATTER_COMMON_CMD_IDENTIFY_STATE_CHANGE_NOTIFY:
            status = value[1];
            TUYA_DBG_EXEC(TUYA_PRINT("Matter indentify state change: %d", status));
            switch (status) {
                case 0x00:
                    // identify start
                    break;
                case 0x01:
                    // identify stop
                    break;
                case 0x02:
                    // identify effect Blink
                    break;
                case 0x03:
                    // identify effect Breathe
                    break;
                case 0x04:
                    // identify effect Okay
                    break;
                case 0x05:
                    // identify effect Channel change
                    break;
                case 0x06:
                    // identify effect Finish
                    break;
                case 0x07:
                    // identify effect Stop
                    break;
            }

            // TODO: 成功: 0 失败: 1
            // result = ?????;

            send_len = set_wifi_uart_byte(send_len, sub_cmd);
            send_len = set_wifi_uart_byte(send_len, result);
            wifi_uart_write_frame(MATTER_COMMON_CMD, MCU_TX_VER, send_len);
            break;

        case MATTER_COMMON_CMD_EVENT_NOTIFY:
            status = value[1];
            TUYA_DBG_EXEC(TUYA_PRINT("Matter event: %d", status));
            switch (status) {
                case 0x00:
                    // 正常上电启动
                    TUYA_DBG_EXEC(TUYA_PRINT("Power on"));
                    break;
                case 0x01:
                    // 配网窗口开启
                    TUYA_DBG_EXEC(TUYA_PRINT("net config window open"));
                    break;
                case 0x02:
                    // 配网成功
                    TUYA_DBG_EXEC(TUYA_PRINT("net config SUCCESSFUL."));
                    break;
                case 0x03:
                    // 配网失败
                    TUYA_DBG_EXEC(TUYA_PRINT("net config FAILED."));
                    break;
                case 0x04:
                    // 重置（暂未上报）
                    TUYA_DBG_EXEC(TUYA_PRINT("reset."));
                    break;
                case 0x05:
                    // 配网窗口关闭
                    TUYA_DBG_EXEC(TUYA_PRINT("net config window close"));
                    break;
                case 0x06:
                    // 移除配网信息
                    TUYA_DBG_EXEC(TUYA_PRINT("net config info remove"));
                    break;
            }

            // TODO: 成功: 0 失败: 1
            // result = ?????;

            send_len = set_wifi_uart_byte(send_len, sub_cmd);
            send_len = set_wifi_uart_byte(send_len, result);
            wifi_uart_write_frame(MATTER_COMMON_CMD, MCU_TX_VER, send_len);
            break;
    }
}

#endif

#if defined(CLOUD_STORAGE_FUNCTION_ENABLE)

void cloud_stonage_fun(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    u8 result;
    u16 send_len = 0;

    switch (sub_cmd) {
        case CLOUD_STONAGE_TRANS_SUB_CMD_UPLOAD_START:
            // TODO: 成功: 0 失败: 1
            result = value[1];
            if (result == 0x00) {
                TUYA_DBG_EXEC(TUYA_PRINT("Cloud stonage start OK"));
            } else {
                TUYA_DBG_EXEC(TUYA_PRINT("Cloud stonage start FAILED"));
            }
            break;
        case CLOUD_STONAGE_TRANS_SUB_CMD_UPLOAD_DATA:
            // TODO:
            result = value[1];
            if (result == 0x00) {
                TUYA_DBG_EXEC(TUYA_PRINT("Cloud stonage upload frame OK"));
            } else {
                TUYA_DBG_EXEC(TUYA_PRINT("Cloud stonage upload frame FAILED"));
            }
            break;
        default:
            TUYA_DBG_EXEC(TUYA_PRINT("Unknown subcmd %02X", sub_cmd));
            break;
    }
}

#endif

#if defined(AP_TRANSPARENT_TRANS_TEST_ENABLE)

void ap_transparent_trans_test_fun(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    const u8* buf_ptr;
    u16 buf_len;
    u16 send_len = 0;
    switch (sub_cmd) {
        case AP_TRANSPARENT_TRANS_TEST_SUB_CMD_UPSTREAM_DATA:
            buf_ptr = &value[1];
            buf_len = length - 1;

            // TODO: 处理数据
            // ...

            send_len = set_wifi_uart_byte(send_len, sub_cmd);
            wifi_uart_write_frame(AP_TRANSPARENT_TRANS_TEST_CMD, MCU_TX_VER, send_len);
            break;

        case AP_TRANSPARENT_TRANS_TEST_SUB_CMD_DOWNSTREAM_DATA:

            // TODO: do something

            break;
    }
}

#endif

#if defined(FACTORY_RECOVERY_FUNCTION_ENABLE)

void factory_recovery_result() {
    TUYA_DBG_EXEC(TUYA_PRINT("Got reset event"));
    // TODO: do something
}

#endif

#if defined(DP_CACHE_ENABLE)

void dp_cache_get_result(const u8 value[], u16 length) {
    u8 result = value[0];
    u8 count;
    const u8* data_start;
    const u8* data_end;
    if (result == 0) {
        // 失败
        return;
    }
    count = value[1];
    data_start = &value[2];
    data_end = &value[length];

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 此处完成相关逻辑 data_start指向第一个数据，data_end指向最后一个数据的后一个元素
#endif
}

#endif

#if defined(HIBERNATE_ENABLE)

void hibernate_fun(const u8 value[], u16 length) {
    u8 sub_cmd = value[0];
    u8 reason;
    u8 allow_hibernate;
    u16 send_len = 0;

    switch (sub_cmd) {
        case HIBERNATE_SUB_CMD_HIBERNATE_NOTIFY:
            TUYA_DBG_EXEC(TUYA_PRINT(" module request enter hibernate."));
            // TODO: 模组请求休眠，此处应该检查是否有未上传的数据，如有，回复0x00阻止模组休眠
            allow_hibernate = 0x01;  // TODO:  0x00: 不允许睡眠， 模组会在5秒后重试  0x01 允许睡眠，模组将立刻睡眠
            send_len = set_wifi_uart_byte(send_len, HIBERNATE_SUB_CMD_HIBERNATE_NOTIFY);
            send_len = set_wifi_uart_byte(send_len, allow_hibernate);
            wifi_uart_write_frame(HIBERNATE_CMD, MCU_TX_VER, send_len);
            break;
        case HIBERNATE_SUB_CMD_WAKEUP_NOTIFY:
            // TODO: 唤醒
            TUYA_DBG_EXEC(TUYA_PRINT("wakeup."));

            send_len = set_wifi_uart_byte(send_len, HIBERNATE_SUB_CMD_WAKEUP_NOTIFY);
            wifi_uart_write_frame(HIBERNATE_CMD, MCU_TX_VER, send_len);
            break;
        case HIBERNATE_SUB_CMD_HIBERNATE_REQUEST:
            reason = value[1];
            if (reason == 0) {
                // 允许进睡眠
                TUYA_DBG_EXEC(TUYA_PRINT("Allow to hibernate"));
            } else {
                // 不允许进睡眠，原因：
                // Bit0：唤醒IO未设置为无效电平, 1：任务状态；0：非任务中；
                // Bit1：ota任务, 1：任务状态；0：非任务中；
                // Bit2：设备状态同步，1：任务状态；0：非任务中；
                // Bit3：其他任务处理，1：任务状态；0：非任务中；
                // Bit4-bit7：预留，默认0；
                TUYA_DBG_EXEC(TUYA_PRINT(
                    "Reject to hibernate because: "
                    "Wakeup IO invalid: %s, "
                    "OTA task: %s, "
                    "sync task: %s, "
                    "other task: %s",
                    reason & 0x01 ? "yes" : "no",
                    reason & 0x02 ? "yes" : "no",
                    reason & 0x04 ? "yes" : "no",
                    reason & 0x08 ? "yes" : "no"));
            }
            break;
    }
}

#endif

#if defined(OFFLINE_VOICE_CTRL_ENABLE)

void offline_voice_ctrl(const u8 cmd_datas[], u16 cmd_data_length) {
    u32 wkup_id = UINT32_NTOH(&cmd_datas[0]);
    u16 send_len = 0;
    u8 result = 0;

    TUYA_DBG_EXEC(TUYA_PRINT("Got offline ctrl event, wakeup id is %08X", wkup_id));

#ifndef CONFIG_TUYA_DISABLE_ALL_ERROR_MARCO
#error 参考协议文档和唤醒词ID定义，完成对各个唤醒词的处理
#endif

    switch (wkup_id) {
        // TODO: 参考offline_voice_worc_<lang>.h中唤醒词ID的定义 (请按需引入目标语言的定义标头)
        // 唤醒词定义名称遵循 OFFLINE_VOICE_WORD_<lang>_<type>_<action>的格式，详细定义参考标头定义旁的注释或相关文档
        // 建议收到唤醒词后，更新对应DP的值

        /*
        EXAMPLE:

        case OFFLINE_VOICE_WORD_CN_CTRL_POWER_ON: // 空调电源开
            // 处理电源开逻辑
            mcu_dp_bool_update(DP_ID_POWER_ON, 1);
            break;

        case OFFLINE_VOICE_WORD_CN_CTRL_POWER_OFF: // 空调电源关
            // 处理电源开逻辑
            mcu_dp_bool_update(DP_ID_POWER_ON, 0);
            break;

        */
    }
    result = 0x00;  // TODO: 0x00: 成功；0x01: 失败
    send_len = set_wifi_uart_byte(send_len, result);
    wifi_uart_write_frame(OFFLINE_VOICE_CTRL_CMD, MCU_TX_VER, send_len);
}

#endif

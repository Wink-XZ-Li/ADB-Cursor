#if !defined(_OFFLINE_VOICE_WORD_ZH_CN_)
#define _OFFLINE_VOICE_WORD_ZH_CN_

                                                                                   // -----------------------------------------------------------
                                                                                   // | 语义          | 可识别说法
                                                                                   // |               | "()"内的字词表示“可选的”
                                                                                   // |               | "[]"用于显式分隔各组字词
                                                                                   // |               | "/"隔开的字词表示其中任一字词即可匹配
// 唤醒词                                                                          // |---------------|------------------------------------------
#define OFFLINE_VOICE_WORD_ZH_CN_WKUP_1                                0x00000001  // | 唤醒          | 你好涂鸦
#define OFFLINE_VOICE_WORD_ZH_CN_WKUP_2                                0x00000002  // | 唤醒          | 小智同学
#define OFFLINE_VOICE_WORD_ZH_CN_WKUP_3                                0x00000003  // | 唤醒          | 智能管家
// 退出语音交互命令                                                                // |---------------|-------------------------------------------
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_EXIT                             0x00010001  // | 退出语音交互  | [退出/关闭]语音
// 常规命令词                                                                      // |---------------|-------------------------------------------
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_POWER_ON                         0x00020001  // | 开机          | 开机
                                                                                   // |               | [开/打开/开启]设备
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_POWER_OFF                        0x00020002  // | 关机          | 关机
                                                                                   // |               | [关/关闭/关掉]设备
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_AUTO                        0x00020003  // | 自动模式      | 自动
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_COOLING                     0x00020004  // | 制冷模式      | 制冷
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_HEATING                     0x00020005  // | 制热模式      | 制热
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_DRY                         0x00020006  // | 除湿模式      | 除湿
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_POWER_SAVE                  0x00020007  // | 节能模式      | 节能
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_FORMALDEHYDE_REMOVE         0x00020008  // | 除醛模式      | 除醛
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_MANUAL                      0x00020009  // | 手动模式      | 手动
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_SLEEP                       0x0002000A  // | 睡眠模式      | 睡眠
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MODE_AIR_SUPPLY                  0x0002000B  // | 送风模式      | 送风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TEMPERATURE_INC                  0x0002000C  // | 升高温度      | [调高/提高]温度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TEMPERATURE_DEC                  0x0002000D  // | 降低温度      | [调低/降低]温度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TEMPERATURE_MAX                  0x0002000E  // | 最高温度      | 最高温度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TEMPERATURE_MIN                  0x0002000F  // | 最低温度      | 最低温度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_FAN_SPPED_UP                     0x00020010  // | 调高风速      | [调高/提高]风速
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_FAN_SPEED_DOWN                   0x00020011  // | 降低风速      | [调低/降低]风速
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_FAN_SPEED_MAX                    0x00020012  // | 最大风速      | 最大风速
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_FAN_SPEED_MIN                    0x00020013  // | 最小风速      | 最低风速
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_VOLUME_DOWN                      0x00020014  // | 调小音量      | 调小音量
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_VOLUME_UP                        0x00020015  // | 调大音量      | 调大音量
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_NEG_ION_ON                       0x00020016  // | 打开负离子    | 开负离子
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_NEG_ION_OFF                      0x00020017  // | 关闭负离子    | 关负离子
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_LIGHT_ON                         0x00020018  // | 打开灯光      | 开灯光
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_LIGHT_OFF                        0x00020019  // | 关闭灯光      | 关灯光
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SWING_ON                         0x0002001A  // | 打开摆风      | 开摆风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SWING_OFF                        0x0002001B  // | 关闭摆风      | 关摆风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SWING_VERTICAL_ON                0x0002001C  // | 打开上下摆风  | 左右摆风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SWING_VERTICAL_OFF               0x0002001D  // | 关闭上下摆风  | 关左右摆风  
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SWING_HORIZON_ON                 0x0002001E  // | 打开左右摆风  | 上下摆风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SWING_HORIZON_OFF                0x0002001F  // | 关闭左右摆风  | 关上下摆风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SLEEP_ON                         0x00020020  // | 打开睡眠      | (请)开睡眠
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SLEEP_OFF                        0x00020021  // | 关闭睡眠      | (请)关睡眠
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_DISPLAY_ON                       0x00020022  // | 打开屏显      | 开屏显
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_DISPLAY_OFF                      0x00020023  // | 关闭屏显      | 关屏显
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_UV_STERILIZATION_ON              0x00020024  // | 打开UV杀菌    | 开UV
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_UV_STERILIZATION_OFF             0x00020025  // | 关闭UV杀菌    | 关UV
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_HUMIDIFICATION_ON                0x00020026  // | 打开加湿      | 开加湿
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_HUMIDIFICATION_OFF               0x00020027  // | 关闭加湿      | 关加湿
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_HEATING_ON                       0x00020028  // | 打开加热      | 开加热
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_HEATING_OFF                      0x00020029  // | 关闭加热      | 关加热
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_FRESH_AIR_ON                     0x0002002A  // | 打开新风      | 开新风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_FRESH_AIR_OFF                    0x0002002B  // | 关闭新风      | 关新风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_GENTLE_WIND_ON                   0x0002002C  // | 打开柔风      | 开柔风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_GENTLE_WIND_OFF                  0x0002002D  // | 关闭柔风      | 关柔风
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SURROUND_WIND_ON                 0x0002002E  // | 打开环抱风    | 开环抱风    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SURROUND_WIND_OFF                0x0002002F  // | 关闭环抱风    | 关环抱风 
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_QUICK_COOL_ON                    0x00020030  // | 打开速冷      | 开速冷
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_QUICK_COOL_OFF                   0x00020031  // | 关闭速冷      | 关速冷
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MOLD_PROOF_ON                    0x00020032  // | 打开防霉      | 开防霉
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_MOLD_PROOF_OFF                   0x00020033  // | 关闭防霉      | 关防霉
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_AUX_HEATING_ON                   0x00020034  // | 打开电辅热    | 开电辅热 
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_AUX_HEATING_OFF                  0x00020035  // | 关闭电辅热    | 关电辅热  
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SELF_CLEANING_ON                 0x00020036  // | 打开自清洁    | 开自清洁    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_SELF_CLEANING_OFF                0x00020037  // | 关闭自清洁    | 关自清洁    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_OFF                        0x00020038  // | 关闭定时      | 关闭定时
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_1_HR                       0x00020039  // | 定时一小时    | 定时一小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_2_HR                       0x0002003A  // | 定时二小时    | 定时[二小时/两小时]    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_3_HR                       0x0002003B  // | 定时三小时    | 定时三小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_4_HR                       0x0002003C  // | 定时四小时    | 定时四小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_5_HR                       0x0002003D  // | 定时五小时    | 定时五小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_6_HR                       0x0002003E  // | 定时六小时    | 定时六小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_7_HR                       0x0002003F  // | 定时七小时    | 定时七小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_8_HR                       0x00020040  // | 定时八小时    | 定时八小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_9_HR                       0x00020041  // | 定时九小时    | 定时九小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_10_HR                      0x00020042  // | 定时十小时    | 定时十小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_11_HR                      0x00020043  // | 定时十一小时  | 定时十一小时    
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_TIMER_12_HR                      0x00020044  // | 定时十二小时  | 定时十二小时 
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_16_DEGREE                        0x00020045  // | 十六度        | 十六度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_17_DEGREE                        0x00020046  // | 十七度        | 十七度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_18_DEGREE                        0x00020047  // | 十八度        | 十八度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_19_DEGREE                        0x00020048  // | 十九度        | 十九度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_20_DEGREE                        0x00020049  // | 二十度        | 二十度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_21_DEGREE                        0x0002004A  // | 二十一度      | 二十一度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_22_DEGREE                        0x0002004B  // | 二十二度      | 二十二度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_23_DEGREE                        0x0002004C  // | 二十三度      | 二十三度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_24_DEGREE                        0x0002004D  // | 二十四度      | 二十四度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_25_DEGREE                        0x0002004E  // | 二十五度      | 二十五度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_26_DEGREE                        0x0002004F  // | 二十六度      | 二十六度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_27_DEGREE                        0x00020050  // | 二十七度      | 二十七度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_28_DEGREE                        0x00020051  // | 二十八度      | 二十八度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_29_DEGREE                        0x00020052  // | 二十九度      | 二十九度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_30_DEGREE                        0x00020053  // | 三十度        | 三十度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_31_DEGREE                        0x00020054  // | 三十一度      | 三十一度
#define OFFLINE_VOICE_WORD_ZH_CN_CTRL_32_DEGREE                        0x00020055  // | 三十二度      | 三十二度


    
#endif


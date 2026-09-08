# 硬件资源表（阶段 0）

| 资源 | 脚位 / 模块 | 来源 | 状态 |
| --- | --- | --- | --- |
| 日志 UART | P0.5 TX / P0.6 RX，USCI0，115200 | 提示词 + 手册 USTX0/USRX0 + 实板 | **已确认** |
| 电源板 UART | P4.4 TX / P4.5 RX，USCI2，4800 8N1 | 提示词 + 8763 手册 USTX2/USRX2 + 官方 USCIX Demo | **已确认**（阶段 2；收发轮询） |
| WiFi UART | P2.1 TX / P2.0 RX，UART0，9600 8N1 | 提示词 + PcbDoc `TX_WIFI`/`RX_WIFI` + 官方 `Uart_Init.c` | **阶段 4**：Timer2 波特率，RX 中断 4；不抢红外 Timer1 |
| WiFi 供电 | P2.6 拉低（`P_Wifi_Power`） | 用户确认 + PcbDoc | **阶段 4**：推挽输出 0，模块才有 3.3 V |
| 掉电记忆 | 独立 EEPROM `IAPADE=0x02` 扇区 0 | 8763 手册 + 官方 IAP 时序；不改 Option | **阶段 4C** |
| MCU OTA | APROM Boot `[0x0000,0x1000)` / RUN `[0x1000,0x8800)` / DL `[0x8800,0x10000)`；标志 EEPROM `0x0200` | 授权 skill；Option 由 SOC 工具设置 | **阶段 4B** |
| 8763 未引出 | P0.0–P0.3、P1.4–P1.7、P3.4–P3.7、P4.6–P4.7、P5 | 官方头文件 `SC95F8763_NIO_Init` | 已按宏配置推挽 |
| 心跳 GPIO | 曾用 P0.4 | 与 TK28 冲突 | **1B 起停用** |
| 系统时钟 | 按 32 MHz 计算波特率 | 手册 IRC 档位 + 115200 可读 | **与 32 MHz 相符**。4B 改 Option 时必须保持 32 MHz |
| WDT | `WDTCON` bit4 CLRWDT | 手册；Option ENWDT 未改 | 循环中喂狗 |
| 时基 | Timer0 模式 1，Fsys，1 ms 重装 | 手册 + 官方 Demo 公式 | **已确认**（阶段 0） |
| TM1640 | P3.1 SCLK / P3.0 DIN | 1A 实板 | **已确认** |
| 触摸 | TK8/10/18/21/23/28 | Sense_Lib 配置 + 手册脚位 | **已确认**（1B 0.1.2，日志 `KEY tk=`） |
| 蜂鸣 | P2.4 | 用户确认；规格书：仅上电鸣 | **已确认** |
| 红外 | P3.2（SOP28 pin 12，网名 `P_REC`，REC1 WH1738） | 授权 PcbDoc + 8763 SOP28 手册；无 EXTI | **已确认**（阶段 3）；Timer1 50 µs 采样 |
| 室内 NTC | P2.3 / AIN7（SOP28 pin 22，网名 `AD_NTC`） | 授权 PcbDoc + 8763 ADC；用户提供 10K 表；实板 AD 随温度升高，`adc>>4` 查表 | **已确认**（阶段 3）；ADC 轮询，不开中断 6 |

ORBEK 原理图无文字层，视觉识别互相矛盾，**不作为脚位冻结依据**。

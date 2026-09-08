# 资料来源与复用清单

记录本次实际使用的输入。阶段 4B 已读授权 OTA skill，未读旧 Boot 源码。

## 本提示词与工程

| 项 | 说明 |
| --- | --- |
| 工作目录 | `D:\嵌入式项目文件夹\ADB-Cursor` |
| 提示词 | 用户阶段 -1 实验说明（2026-09-07） |
| 模型 | Cursor Grok 4.6 |

## 允许的原始资料（存在性核对）

均在 `D:\格力空调ADB\`，日期约 2026-08-03 / 08-08，均存在。

| 文件 | 大小 | 阶段 -1 用途 |
| --- | --- | --- |
| 房车空调ADB电控规格书更新Wi-Fi部分5月25号.doc | 585728 | 未成功提取正文（Word COM 失败）。未扩大读取范围。 |
| ORBEK-LK-GLADB-D V1.0(1)(1) (1).pdf | 175986 | 原理图。无文字层。 |
| ORBEK-LK-GLADB-D V1.0(1).PcbDoc | 1826304 | 阶段 3：OLE 解析确认 `P_REC`→U8 pin12。未读旧工程。 |
| HY-3129-1.pdf | 50952 | 数码管外形，阶段 1A 再用。 |
| HY-6007逻辑图__（改）(3).pdf | 44300 | 指示灯条外形，阶段 1A 再用。 |
| C5337152_...TM1640...PDF | 1363837 | 未读。阶段 1A。 |
| 内外机通信协议_260209-终.docx | 28084 | 阶段 2：已提取帧格式（`66 99` / `55 5A`、4800 8N1）。未读旧工程协议代码。 |
| protocol_uforlynlgj5xx3zg_20260803.pdf | 146763 | 阶段 4：PID `uforlynlgj5xx3zg`、9600、DP 1/2/3/4/5/19/22/23/24/25/120/150。未读旧工程。 |
| mcu_sdk_FOGATTI...20260803.zip | 55707 | 阶段 4：官方 MCU_SDK v2.6.2。已复制 `mcu_sdk/` 到 `firmware/third_party/tuya_mcu/`。C51 只编译自写 `tuya_link.c`（官方协议子集），不链 `protocol.c`。 |
| 红外遥控器协议.doc | 233472 | 阶段 3：载波 38 kHz、系统码 `0x56`、120 bit、A–N。模式位按表（001 热 / 010 冷）。未读旧工程红外代码。 |

## 官方芯片与工具链

| 项 | 版本 / 标识 | 用途 | 复用范围 |
| --- | --- | --- | --- |
| Keil C51 | C51.exe V9.53.0.0，路径 `D:\C51\BIN` | 编译 | 工具链 |
| μVision | V5.11.2.0，`D:\UV4\UV4.exe`（2014-07-31） | 编译 / 计划烧录 | 工具链 |
| STARTUP.A51 | Keil C51 8.01，2014-01-29，SHA256 `0298D03E...104011` | 复位启动 | 官方启动文件。目录内已有，未覆盖。 |
| SC95F876x_C.H | 赛元 V1.0 2022-01-20，SHA256 `FE6BCA192C3D029BEE048647FB619207E9793A955D8654E3DFA3362825DC5BBC` | SFR / 未引出脚宏 | 自 `D:\C51\SOC` 复制到 `firmware/third_party/sinone/`，未改内容 |
| 数据手册 中文 | [SC95F8767_8766_8765_8763_8762v1.0cn](https://www.socmcu.com/upfile/SC95F8767_8766_8765_8763_8762v1.0cn.pdf)，SHA256 `647C0C15A00E9390BF237B2E8625A29EA1407CC8E8521A352430E70D5AAB40D0` | USCI0、WDT、封装 | 官方手册 |
| 数据手册 英文 | [v0.1en](https://socmcu.com/upfile/SC95F8767_8766_8765_8763_8762v0.1en%20.pdf)，SHA256 `ED453C4ECF29AEFD9034A61BF6C977465212CA7F6067D51E76F32B93D551BD27` | 同上 | 官方手册 |
| 应用指南 | 赛元 SC95F 系列 MCU 应用指南 V1.8 / V2.0（官网） | USCI TI/RI 写1清0、TX 上拉 | 只取 UART 注意项，未下载同类产品工程 |
| 官方 Demo USCIX_Init.c | 同上 Demo 目录 | 阶段 2：USCI2 UART 的 P4.4/P4.5、`USXINX=2`、`TMCON|=0xC0`、`IE2|=0x02`、interrupt 16 | 未复制 Demo 工程或业务 |
| 官方 Demo ADC_Init.c | 8763 Demo | 阶段 3：`ADCCON`/`ADCCFG`/`ADCVH`/`ADCVL`、启动位与 12 位拼法 | 轮询 EOC，不使能中断 6；未复制平均/中断业务 |
| 用户粘贴 10K NTC 表 | 对话 2026-09-08 | `c_AD_TO_TEMPER_10K_TABLE` 下标 0–72℃ | 未打开旧工程取表；0.1.3 按 `adc>>4` 查 |
| SOC Programming Tool | `D:\SOC Programming Tool` 及 Enhance | 阶段 4B：你用它改 Option 并烧 `releases\factory.hex` | 官方烧录工具 |
| 通用 skill | `build-keil` / `flash-keil` / `serial-monitor` / `workflow` / `sc95-ota-dual-zone` | 编译、烧录、串口、4B OTA 地图与状态机 | 不引入旧产品代码；OTA skill 未打开对照工程 |

未使用：旧 ADB 除 Sense_Lib 外的源码 / test2 / Boot 工程源码。涂鸦只用来自授权 zip 的 MCU_SDK 头文件与 DP/PID 及 `system.c` 的 `0x0A`/`0x0B` 帧格式说明，不编译官方 `protocol.c`。

## Sense_Lib 复制（阶段 1B，授权例外）

来源目录：`D:\嵌入式项目文件夹\ADB\Sense_Lib`（仅该目录 4 个文件）。  
副本：`firmware/third_party/sense_lib/`。未改内容。

| 文件 | 字节 | SHA256 | 用途 |
| --- | --- | --- | --- |
| `SC95F8X6X_HighSensitive_lib_T1_L_V2.1.0.LIB` | 16210 | `F3DE0612C1B9E17073F1D10123A366D832E54C5F4E1EACD1B2B8683F7AA35B7D` | 赛元高灵敏触摸算法库（LARGE）。副本改名为 `SC95F8X6X_HighSensitive.LIB` 以便 LX51，内容未改。 |
| `S_TouchKeyCFG.h` | 930 | `E33FF0D49217FA50865E29AB9EA07A80B43F36A3681E7A96D9A9195431088F82` | 6 通道触摸配置 |
| `TKDriver.C` | 12752 | `F93516262912DC67740F284A50603997605A9B203D3C0BBD9D1279AF1035C875` | 官方扫描/中断封装 |
| `TKDriver.h` | 1582 | `0D5E542DCC8CC61560775FF9CA1C3FF2CE12E7C3FC964F3931382DD2185C1F23` | 库接口 |

通道掩码 `0x10A40500` → TK8/10/18/21/23/28（手册对应 P1.0、P1.2、P2.2、P2.5、P2.7、P0.4）。P0.4 不再作心跳 GPIO。  
适配：`TKDriver.C` 中按键位判断不再使用 `PSW.CY`（LARGE 下 `>>` 走 `?C?ULSHR`，CY 不可靠）。链接把 CODE 放到 `0x80` 之后，避免 `main` 盖住 `0x005B` 触摸中断向量。  
调用顺序按赛元 TouchKey 应用指南 T1：TK 脚先强推挽输出高，`EA=1` 后 `TouchKeyInit()`，每轮 `TouchKeyScan()` 后必须 `TouchKeyRestart()`。  
未复制旧工程 HMI/按键名表；日志用 `KEY tk=N`。

## 人工技术提示（2026-09-07 14:40）

用户反馈 `DISP grid=0..15` 不正常，并要求按本仓库 `docs/led_segment_map.md` 再对。
随后要求测试改为：WiFi、模式循环（一次一灯）、风速循环（一次一灯）、数码管循环。
未打开 GL-ST-ADB 或旧 ADB 源码核对 `DisplayChangeData`。

## 目录内已有文件（未删除）

2026-09-07 11:34–11:36 出现的 Keil 空工程骨架：

- `ADB-Cursor.uvproj`：MCS-51，器件误选 **SC95F8673**，调试器 `SOC_8051_Driver.dll`，仅含 STARTUP。
- 构建日志：μVision 已汇编 STARTUP，缺 `main` 导致 `?C_START` 警告。
- 无 Git。未继承任何仓库历史。

处理：保留 STARTUP.A51 与现有调试器 DLL 配置；不覆盖该启动文件；仅改正器件名/头文件、打开 HEX、加入本阶段源文件。阶段 1B 为 Sense_Lib 的 xdata，将 `XDATALEN` 改为 `2000H`（8 KB）。

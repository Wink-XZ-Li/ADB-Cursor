# 资料来源与复用清单

记录本次实际使用的输入。OTA skill 尚未读取。

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
| ORBEK-LK-GLADB-D V1.0(1)(1) (1).pdf | 175986 | 原理图。无文字层，仅渲染核对。 |
| HY-3129-1.pdf | 50952 | 数码管外形，阶段 1A 再用。 |
| HY-6007逻辑图__（改）(3).pdf | 44300 | 指示灯条外形，阶段 1A 再用。 |
| C5337152_...TM1640...PDF | 1363837 | 未读。阶段 1A。 |
| 内外机通信协议_260209-终.docx | 28084 | 阶段 2：已提取帧格式（`66 99` / `55 5A`、4800 8N1）。未读旧工程协议代码。 |
| protocol_uforlynlgj5xx3zg_20260803.pdf | 146763 | 未读。阶段 4。 |
| mcu_sdk_FOGATTI...20260803.zip | 55707 | 未读。阶段 4。 |
| 红外遥控器协议.doc | 233472 | 未读。阶段 3。 |

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
| 官方 Demo USCI0_Init.c | `D:\C51\SOC_KEIL_Setup\DEMO\SC95F8767_8766_8765_8763_128K_Demo_Code\C\USCI0_Init.c` | 核对 P0.5/P0.6 与 OTCON/US0CON 初始化顺序 | 未复制 Demo 工程或业务代码 |
| SOC Programming Tool | `D:\SOC Programming Tool` 及 Enhance | 已登记，阶段 -1 不改 Option、不用其改芯片配置 | 官方烧录工具存在 |
| 通用 skill | `build-keil` / `flash-keil` / `serial-monitor` / `workflow` | 编译、烧录、串口。已检查：显式工程路径，不读取范围外工程 | 不引入旧产品代码 |

未使用：`sc95-ota-dual-zone` skill、涂鸦 SDK、旧 ADB 除 Sense_Lib 外的源码 / test2 / Boot 工程。

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

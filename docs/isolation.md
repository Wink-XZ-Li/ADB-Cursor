# 隔离与接触记录

## 进程列表附带接触

列举 UV4 进程时看到命令行：

- `D:\嵌入式项目文件夹\ADB\ADB.uvproj`（旧 ADB 工程，10:16 起已打开）
- `D:\嵌入式项目文件夹\ADB-Cursor\ADB-Cursor.uvproj`（本工程 GUI）

只见到路径，**未打开、未读、未比较**旧 ADB 源码。因此改用 C51 命令行编译，避免再启动第三个 UV4 抢同一工程。

## 遵守

- 未读取旧 ADB、test2、GL-ST-ADB、Boot/ApTest、历史对话、agent-transcripts。
- 2026-09-07：用户要求按本仓库 `docs/led_segment_map.md` 纠正 GRID 扫描；只读该文件，未打开其提到的 GL-ST-ADB 工程。
- 未用父目录搜索或子代理扩大范围。
- 2026-09-08 阶段 4B：读取授权 `sc95-ota-dual-zone` skill + `reference.md`；未读旧 Boot 工程源码。
- 2026-09-07：读取赛元官网 TouchKey 应用指南（T1 调用顺序），未读旧工程触摸业务。
- 2026-09-07 阶段 1B：按授权读取并复制 `D:\嵌入式项目文件夹\ADB\Sense_Lib` 下 4 个文件。未读取旧工程其余源码。
- 2026-09-07 阶段 2：只提取授权 `内外机通信协议_260209-终.docx` 正文，未读旧工程协议源码。
- 2026-09-07 阶段 2 实现：读取官方 8763 Demo `USCIX_Init.c` 的 USCI2 UART 脚位/`TMCON`/`IE2`/interrupt 16；未复制 Demo 业务。规格书 `.doc` 故障码页此前未能提取正文；E1/E2 仅对应外机 Byte9=6（环境感温包）与 7（冷凝器中间管），其余故障只打日志。后改为轮询收数、不开 USCI2 中断。
- 2026-09-07 阶段 2 通过后冻结阶段 3：只提取授权 `红外遥控器协议.doc` 正文（载波 38 kHz、系统码 `0x56`、A–N 字节），未读旧工程红外源码。
- 2026-09-07 阶段 3：从授权 `ORBEK-LK-GLADB-D V1.0(1).PcbDoc` 的 OLE `Nets6`/`Pads6`/`Components6` 确认 U8=SC95F8763 SOP28 pin12=`P_REC`、REC1=WH1738；对照官网 SOP28 手册 pin12=P3.2。同文件确认 pin22 网名 `AD_NTC`（P2.3 / AIN7）。未读旧工程红外解码或 NTC 表。原理图 PDF 仍无文字层。引导码/数据 0/1/尾码在 Word 表中为空，采用实板冻结时序（不再改极性/窗口）。NTC 曲线先按典型 10 k B3950 + 10 k 上拉；2026-09-08 用户在对话中粘贴 `c_AD_TO_TEMPER_10K_TABLE`（0–72℃），按该表查 8 位 AD，未打开旧工程取表。0.1.2 对 12 位取反后室温约 61℃且手捂变小；0.1.3 改为 `adc>>4` 直接查表（本板 AD 随温度升高）。
- 2026-09-07 阶段 3 NTC：读取官方 8763 Demo `ADC_Init.c` 的 `ADCCON`/`ADCCFG0`/`ADCCFG2`/`ADCVH`/`ADCVL` 与启动位；**轮询 EOC，不使能 ADC 中断 6**。未复制 Demo 平均/中断业务。
- 2026-09-08 08:59：用户「阶段 3 通过」，接受 `STAGE-3-0.1.3`。
- 2026-09-08 阶段 4 冻结：只提取授权 `protocol_uforlynlgj5xx3zg_20260803.pdf` 与 zip 内 `mcu_sdk` 的 `readme.txt`/`protocol.h`/`wifi.h`/`mcu_api.h`；读取官方 8763 Demo `Uart_Init.c` 确认 P2.0/P2.1 为 UART0。未读旧工程 WiFi，未复制 SDK 到 `firmware/`，未读 OTA skill。
- 2026-09-08 阶段 4 实现：按 1B Sense_Lib 同类例外，把授权 zip 的 `mcu_sdk/` 复制到 `firmware/third_party/tuya_mcu/`（未改业务 DP 定义）。官方 `protocol.c`/`mcu_api.c`/`system.c` **不进 C51 编译**：`tuya_type.h` 的 `weak`/`inline`、`u32=unsigned int`（C51 为 16 位）以及体积不适配 8763。`tuya_link.c` 按同一套头文件实现心跳 / 产品信息 / 工作模式 / WiFi 状态 / 复位 / DP 下发上报（`55 AA`、PID `uforlynlgj5xx3zg`、DP 1/2/3/4/5/19/22/23/24/25/120/150）。UART0 波特率按官方 Demo 的 **Timer2** 段，不抢红外 Timer1。产测宏保持关闭。未读旧工程 WiFi，未读 OTA skill。
- 2026-09-08 阶段 4 供电：授权 PcbDoc 有网名 `P_Wifi_Power`（另有 `RST`，未当 WiFi 复位脚用）。用户确认 **P2.6 拉低模块才有供电**。未读旧工程初始化。触摸掩码不含 TK22，P2.6 可作 GPIO。
- 2026-09-08 10:16：用户「阶段4通过」，接受 `STAGE-4-0.1.3`。掉电记忆未读旧工程，未读 OTA skill。
- 2026-09-08 阶段 4C：独立 EEPROM 掉电记忆。IAP 读按官方 8763 Demo `IAP_Read`（`IAPADE=0x02`）；擦写按同系列 Demo `IAPKEY=0xF0`、`IAPCTL` 0x10/0x20 再 `| 0x02`。未复制缺失的 `IAP_Option_EW` 库，未写 APROM，未改 Option。
- 2026-09-08 10:28：用户「通过」，接受 `STAGE-4C-0.1.0`。未读 OTA skill。
- 2026-09-08 阶段 4B：用户确认后读取授权 skill `sc95-ota-dual-zone` 及其 `reference.md`。**未打开、未读**旧工程 `SC95F8763_BootLoader` / `SC95F8763_Boot_ApTest` 源码。IAP 时序沿用本仓库已验收的 `eeprom.c`（`IAPKEY=0xF0`、`IAPCTL` 0x10/0x20 再 `| 0x02`），扩到 APROM（`IAPADE=0x00`）。涂鸦 `0x0A`/`0x0B` 按已复制 SDK 的 `system.c` 帧格式在 `tuya_link.c` 实现，**不编译**官方 `protocol.c`。Option 由你在 SOC Programming Tool 设置，固件不写 Option。
- 2026-09-08 13:36：用户「阶段 4B 通过」，接受 `STAGE-4B-0.1.0`。SOC 工具改 Option 时 IRC 须保持 32 MHz。

## 提示词笔误与 ADB-Codex

提示词第二节指定本目录 `ADB-Cursor`；第十三节写“检查 ADB-Codex”。按第十三节做了**存在性与目录名列表**，未读其源码、文档、提交或验收记录。

`D:\嵌入式项目文件夹\ADB-Codex` 存在，且含 `.git`、`firmware`、`docs` 等。本目录 `uvproj` 字节数与其同名工程文件同为 13986，来源可能相关，**未打开对照**。

处理：不从 ADB-Codex 复制；不把它当作参考实现；不把本工程描述为“从零空白目录创建”。本目录开工时已有 Keil 骨架。独立性声明不覆盖该骨架的来源。

## 官方 Demo 接触

为核对 SC95F876x 工程字段和 USCI0 UART 寄存器用法，读取了官方 pack 中 Demo 的 `DEMO.uvproj` 器件段、`USCI0_Init.c`、`IO_Init.c`、`Function_Init.H` 和 `main.c` 中 WDT 一行。未复制 Demo 工程，未采用其错误器件名 SC95F8617，未启用其 128K Merge32K/banking。

## 原理图视觉识别

ORBEK PDF 无文字层。一次整图描述给出与提示词一致的 P0.5/P0.6、P4.4/P4.5、P2.1/P2.0；局部放大描述出现 STM32 风格脚名，与 8051 资料冲突，**作废**。阶段 -1 不以视觉结果冻结 LED 脚。

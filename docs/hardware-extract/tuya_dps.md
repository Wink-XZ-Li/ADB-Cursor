# 涂鸦产品功能（阶段 4 提取）

来源：授权 `protocol_uforlynlgj5xx3zg_20260803.pdf` + 官方 zip 内 `mcu_sdk/protocol.h`。  
未读旧工程 WiFi 源码。授权 zip 的 `mcu_sdk/` 已按 Sense_Lib 同类例外复制到 `firmware/third_party/tuya_mcu/`。

## 产品

| 项 | 值 |
| --- | --- |
| 名称 | FOGATTI房车空调-冷暖无灯-适配格力主机 |
| PID | `uforlynlgj5xx3zg` |
| 生成日期 | 2026-08-03 |
| 串口 | **9600 8N1**，帧头 `0x55 0xAA`，长度大端，校验为帧头起按字节和对 256 取余 |
| SDK | 涂鸦 MCU_SDK v2.6.2；`PRODUCT_KEY` 同上；`MCU_VER` 默认 `1.0.0` |
| 配网宏 | zip 内为 `CONFIG_MODE_SPECIAL`（防误触）；MCU 升级宏默认关闭 |
| 指示灯/复位 | zip 内 **未** 开 `WIFI_CONTROL_SELF_MODE`（MCU 处理灯和复位） |

## DP

| DP | 名称 | 方向 | 类型 | 属性 | 与本机 RAM |
| --- | --- | --- | --- | --- | --- |
| 1 | 开关 | 下发/上报 | bool | — | `hmi_power` |
| 2 | 温度设置 | 下发/上报 | value | 16–31 ℃ | 温标为 C 时的设定 |
| 3 | 当前温度 | 只上报 | value | −20–100 ℃ | 室内 NTC（`ntc_c`） |
| 4 | 模式 | 下发/上报 | enum | dry, airSupply, cool, heat | 本机 dry / fan / cool / heat（顺序与 1C 编号不同，实现时对照） |
| 5 | 风速 | 下发/上报 | enum | low, middle, high, turbo | 本机低/中/高/超快 |
| 19 | 温标 | 下发/上报 | enum | c, f | 本机 C/F |
| 22 | 故障 | 只上报 | fault | E1…E8 | 已有 E1/E2；其余只上报不发明新灯 |
| 23 | 当前温度_F | 只上报 | value | −40–200 ℉ | NTC 换算 |
| 24 | 目标温度_F | 下发/上报 | value | 61–88 ℉ | 温标为 F 时的设定 |
| 25 | 睡眠 | 下发/上报 | bool | — | 只做标志：App 下发/上报 + 红外 F3；不改风速/温度 |
| 120 | 屏保 | 下发/上报 | bool | — | 与红外 E2 同一标志（开 ⇔ 屏显关）；灭数字+图标；E1/E2/定时优先；任意键退出且执行原功能 |
| 150 | 使用时长 | 只上报 | value | 0–30 分钟，约每半小时 | 开机累计分钟，关机清零，每 30 分钟上报，钳位 0–30 |

## 联网状态与灯（协议正文）

MCU 配合处理时：SmartConfig 间隔 250 ms；AP 间隔 1500 ms；已配未连或低功耗长暗；已连路由或已连云长亮。

心跳：模块上电连发；MCU 回复后间隔约 15 s。

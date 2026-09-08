# 阶段 4B MCU OTA

版本标识：`STAGE-4B-0.1.0`  
HEX：`releases\Stage4B.hex`  
SHA256：`A51529A65405622E1C6159C6E1256924210F2172BD2EA257103D1EDAF9CFD44E`  
工厂镜像：`releases\factory.hex`  
SHA256：`D48F7AAFB2F65EA7213872D9D0C61177C1CDAC88E696339C39B02779E1CB1B0C`  
OTA 包：`releases\Stage4B_ota.bin`  
用户验收：2026-09-08 13:36「阶段 4B 通过」。接受版本 `STAGE-4B-0.1.0`。  
占用：Boot code=914（`[0x0000,0x1000)`）；App data=40.3，xdata=1298，const=106，code≈20628；向量 `0x101B`/`0x1023`/`0x105B`。  
基线：已验收 `STAGE-4C-0.1.0`（2026-09-08 10:28「通过」；A2-06 失联搁置）

授权 skill：`sc95-ota-dual-zone`（Boot 驻留 + DL→RUN）。**未读**旧工程 `SC95F8763_BootLoader` / `Boot_ApTest` 源码。

## 产品决策（已确认）

| 项 | 取值 |
| --- | --- |
| 地图 | skill **64K**：Boot `[0x0000,0x1000)`，RUN `[0x1000,0x8800)`，DL `[0x8800,0x10000)`（本片 128K 只用低 64K） |
| Option | Start=APROM，LDSIZE=0（无 LDROM），IAP=Code:ALL，**IRC=32 MHz**。你用 **SOC Programming Tool** 改并烧 `factory.hex` |
| 标志 | EEPROM `0x0200` = `55 AA 69 96`（扇区 1）。HMI 掉电记忆仍在 `0x0000`（扇区 0） |
| 清魔数 | **仅整段拷贝成功之后**；失败保留魔数并复位，**不进 App** |
| 涂鸦 | 命令 `0x0A/0x0B`，包 **256** 字节；文件 = `OADB` 16B 头 + RUN 镜像 |
| 升级中 | 忽略按键/红外；数码管顺时针转圈 |
| 升完 | 现有 EEPROM 设定恢复（规格 6.3.2） |

板级：第一次烧 `factory.hex` 后若日志乱码、开机 `--`、WiFi 灯灭，是 Option 时钟不是 32 MHz（2026-09-08 已复现；改回 32 MHz 后正常）。

## 本阶段不做

真 A/B、LDROM Boot、读旧 ADB Boot 源码、A2-06、品牌 PID、产测。

## 验收项

| 编号 | 行为 | 方法 | 预期 | 结果 |
| --- | --- | --- | --- | --- |
| A4B-01 | 编译 | `scripts/build.ps1 -Rebuild` | Boot≤4KB；App CODE∈`[0x1000,0x87FF]`；向量 `0x105B`/`0x101B` | **通过** |
| A4B-02 | 工厂烧录 | SOC 工具烧 `releases\factory.hex`，拔 LINK 上电 | 日志 `STAGE-4B-*`；HMI/WiFi 仍可用 | **通过**（IRC 须 32 MHz） |
| A4B-03 | OTA | 云端升一版 | 版本串变；升完设定仍在 | **通过** |
| A4B-04 | 下载中掉电 | OTA 写 DL 时断电 | 可重试，未置魔数则旧 App 仍跑 | **通过** |
| A4B-05 | 拷贝中掉电 | 复位进 Boot 后 1–3s 断电 | 再上电等 30–60s（Boot 无日志）后新版本可跑，**不需 ICP** | **通过** |

## Option（SOC Programming Tool）

1. 系统时钟 / IRC = **32 MHz**（固件按 32 MHz 算 115200/9600/4800；改成 16/8 MHz 会日志乱码、开机 `--`、WiFi 灯灭）
2. 若工具提示时钟 >12 MHz，则 LVR **>2.3 V**
3. Start from = **APROM**
4. LDSIZE = **0 / NO LDROM**
5. IAP = **Code:ALL**
6. 烧 `releases\factory.hex`
7. **拔掉 SC-Link** 再上电验收（skill：插着 LINK 不作为通过）

## 交付物

- `releases\factory.hex`：Boot ∪ App（App 只含 `≥0x1000`）
- `releases\Stage4B.hex`：仅 App，**不要**单独当工厂镜像烧（会盖掉 Boot）
- `releases\Stage4B_ota.bin`：`OADB` + RUN 镜像，涂鸦 MCU 固件包
- 不覆盖已验收的 `releases\Stage4.hex` / `Stage4C.hex`

工厂版本串 `STAGE-4B-0.1.0`。涂鸦产品 JSON `"v"` 以 `tuya_link.c` 为准（当前板上为 `1.2.13`）。云端再升一版时把 `"v"` 再加一档后重编 `.bin`。

本阶段首次烧录用 **SOC Programming Tool**，不要用 µVision Download 只下 App HEX。改过 `uvproj` 后请关闭并重开 µVision。

Boot 链接可能出现 L10/L57（无 C51 STARTUP，汇编 `LJMP` 不进 overlay 根）。函数均已链入，DATA 未错误重叠；以 `Boot.MAP` 中 `?BOOT_ENTRY` 与 `main` 地址为准。

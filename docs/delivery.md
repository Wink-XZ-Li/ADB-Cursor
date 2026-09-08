# ADB-Cursor 交付报告

文档版本：`STAGE-5-0.1.0`  
日期：2026-09-08  
用户验收：2026-09-08 14:16「阶段 5 通过」  
交付固件：已验收 **`STAGE-4B-0.1.0`**（本阶段不改业务、不改 Option）  
Git 基线：`c61768e`（2026-09-08 13:36「阶段 4B 通过」）  
工作目录：`D:\嵌入式项目文件夹\ADB-Cursor`

阶段 5 只做资源、回归汇总、本报告与运行时架构图。固件横幅仍为 `STAGE-4B-0.1.0`。

## 0. 运行时架构

交互图：[`docs/architecture/adb-runtime.html`](architecture/adb-runtime.html)  
静图：[`docs/architecture/adb-runtime.visual-check.2048x1320.light.png`](architecture/adb-runtime.visual-check.2048x1320.light.png)

10 个组件。加粗主路径：**本地入口 → HMI 设定权威 → 电源板链路 → 电源板/外机**。涂鸦云与 WiFi 模组在 MCU APROM 信任区外；日志、NTC、蜂鸣、A2-06 只在底部说明卡片，不再加连线。

## 1. 产物与哈希

2026-09-08 阶段 5 再编译（`scripts\build.ps1 -Rebuild`）复现同一组哈希，未改源码。

| 文件 | 用途 | SHA256 |
| --- | --- | --- |
| `releases\factory.hex` | 工厂整片：Boot ∪ App（仅 `≥0x1000`） | `D48F7AAFB2F65EA7213872D9D0C61177C1CDAC88E696339C39B02779E1CB1B0C` |
| `releases\Stage4B.hex` | 仅 App，**不要**当工厂镜像单独烧 | `A51529A65405622E1C6159C6E1256924210F2172BD2EA257103D1EDAF9CFD44E` |
| `releases\Stage4B_ota.bin` | 涂鸦 MCU 包：`OADB` 16B + RUN | `64B357B86F0D55FF59ADDA76B2045F7050ABBE41F218D99D9E40050587FD5729` |

OTA 包：payload=20853，CRC16=0x89FF，文件=20869 字节。  
涂鸦产品 JSON 当前 `"p":"uforlynlgj5xx3zg","v":"1.2.13","m":2`（云端 OTA 验收时加过 `"v"`）。日志横幅仍是 `STAGE-4B-0.1.0`。再升一版时把 `"v"` 再加一档后重编 `.bin`。

历史阶段 HEX（不覆盖、不作现网交付）：`Stage4.hex` / `Stage4C.hex` 等见各阶段验收页。

## 2. Flash 地图（64K 规划，本片 128K 只用低 64K）

| 区 | 范围 | 大小 | 谁写 |
| --- | --- | --- | --- |
| Boot | `[0x0000, 0x1000)` | 4 KB | 工厂 `factory.hex`；App **不写** |
| RUN | `[0x1000, 0x8800)` | `0x7800` | 工厂烧 App；Boot 在魔数有效时从 DL 整段拷入 |
| DL | `[0x8800, 0x10000)` | `0x7800` | 仅 App IAP（`addr ≥ 0x8800`） |
| EEPROM 扇区 0 | `0x0000` 起 12 字节 | HMI 掉电记忆 | App `nvm.c` |
| EEPROM `0x0200` | `55 AA 69 96` | OTA 魔数 | App 置位；Boot **仅整段拷贝成功后**清除 |

App 入口 `0x1800`（`STARTUP.A51` `CSEG AT 1800H`）。  
App `INTVECTOR(0x1000)`；可重定位 CODE+CONST：`C:0x1080–C:0x87EF`。  
Boot 可重定位 CODE：`C:0x0060–C:0x0FFF`（向量缝不塞业务码）。复位 `0x0000` 仅 3 字节 `LJMP boot_entry`。

合并结果（阶段 5 再编译）：Boot `0x0000–0x03CA`，App `0x101B–0x6174`，`dropped_lt_1000=0`。

## 3. 占用（LX51 MAP，2026-09-08 再编译）

### 3.1 总量

| 镜像 | Program Size | 窗口 | 占用 |
| --- | --- | --- | --- |
| Boot | data=45.0，xdata=0，const=0，**code=914** | CODE 类 `C:0x0060–C:0x0FFF` 已用 `0x392` | 914 / 4096 ≈ **22%**（相对 4 KB Boot 区） |
| App | data=40.3，**xdata=1298**，const=106，**code=20628** | CODE `0x5094` + CONST `0x006A` @ `C:0x1080–C:0x87EF` | 20734 / 30576 ≈ **68%** |
| XDATA | 已用 `0x512`=1298 | `XDATALEN=2000H`（8 KB） | 1298 / 8192 ≈ **16%** |
| OTA 载荷 | 20853 | 上限 `0x7800−16`=30704 | ≈ **68%**；余量约 9.8 KB |

Boot LX51 可能报 L10 / L57（无 C51 STARTUP，汇编 `LJMP` 不进 overlay 根）。函数均已链入，DATA 未错误重叠。App：0 error / 0 warning。

### 3.2 Boot 最大 CODE 段

| 字节 | 段 |
| --- | --- |
| 246 | `?PR?_COPY_RUN?MAIN` |
| 175 | `?PR?APPLY_OTA?MAIN` |
| 104 | `?PR?_CRC16?MAIN` |
| 62 | `?PR?MAGIC_OK?MAIN` |
| 57 | `?PR?_IAP_GO?BOOT_IAP` |

### 3.3 App 最大 CODE 段

| 字节 | 段 | 来源 |
| --- | --- | --- |
| 1052 | `?PR?_SENSORRENOVATE?SENSORMETHOD` | Sense_Lib |
| 1008 | `?PR?IR_LINK_POLL?IR_LINK` | 红外 |
| 864 | `?PR?HMI_POLL?HMI` | 本地 HMI |
| 786 | `?C?LIB_CODE` | C51 库 |
| 743 | `?PR?HANDLE_FRAME?TUYA_LINK` | 涂鸦 |
| 660 | `?PR?_HMI_APPLY_IR?HMI` | 红外→HMI |
| 444 | `?PR?_DISP_UI_DRAW?DISP_UI` | 显示 |
| 424 | `?PR?SENSORMETHOD` / `?PR?HMI_DRAW?HMI` | 触摸库 / 画屏 |

### 3.4 App 最大 XDATA 段

| 字节 | 段 |
| --- | --- |
| 512 | `?XD?WIFI_UART`（模块 RX 环） |
| 301 | `?XD?TUYA_LINK` |
| 98 | `?XD?TKDRIVER` |
| 73 | `?XD?PWR_LINK` |
| 69 | `?XD?PWR_UART` |
| 58 | `?XD?HMI` |

IRAM：DATA 用到 `0x27`，栈 `?STACK` 在 `I:0x28`。勿再把大缓冲放 DATA。

## 4. 脚位与口（已冻结）

| 资源 | 脚 / 模块 | 参数 |
| --- | --- | --- |
| 日志 | P0.5 TX / P0.6 RX，USCI0 | **115200**，主机 COM7 |
| 电源板 | P4.4 TX / P4.5 RX，USCI2 | **4800** 轮询，不开中断 16 |
| WiFi | P2.1 TX / P2.0 RX，UART0 Timer2 | **9600**，RX 中断 4 |
| WiFi 供电 | **P2.6 拉低** | `P_Wifi_Power` |
| TM1640 | P3.1 SCLK / P3.0 DIN | 显示 |
| 蜂鸣 | P2.4 | **仅上电鸣** |
| 触摸 | TK8/10/18/21/23/28 | Sense_Lib；ISR `interrupt 11` |
| 红外 | P3.2，Timer1 50 µs | 不改采样 |
| NTC | P2.3 AIN7 | `adc>>4` 查 `c_AD_TO_TEMPER_10K_TABLE` |
| 未引出脚 | P0.0–P0.3、P1.4–P1.7、P3.4–P3.7、P4.6–P4.7、P5 | 按宏推挽 |

完整表：`docs/hardware-resources.md`。

## 5. 中断向量（4B 之后）

| 源 | Boot 蹦床 | App（`INTVECTOR(0x1000)`） |
| --- | --- | --- |
| Timer1（红外） | `0x001B` → `LJMP 0x101B` | `0x101B` |
| UART0（WiFi RX） | `0x0023` → `LJMP 0x1023` | `0x1023` |
| 触摸 | `0x005B` → `LJMP 0x105B` | `0x105B` |

Timer0、USCI2、ADC：轮询，不占用对应向量。

## 6. Option（SOC Programming Tool，固件不写）

1. 系统时钟 / IRC = **32 MHz**（115200 / 9600 / 4800 均按 32 MHz）
2. 若工具提示时钟 >12 MHz，则 LVR **>2.3 V**
3. Start from = **APROM**
4. LDSIZE = **0 / NO LDROM**
5. IAP = **Code:ALL**
6. 烧 `releases\factory.hex`
7. **拔掉 SC-Link** 再上电验收

第一次烧工厂镜像后若日志乱码、开机 `--`、WiFi 灯灭：先查 IRC 是否被改离 32 MHz。GPIO（蜂鸣、TM1640）在错时钟下仍可能动。

## 7. 编译

```powershell
.\scripts\build.ps1 -Rebuild
```

C51 `D:\C51\BIN`，LARGE + OMF2 + LX51。App `INTVECTOR(0x1000)`。  
会覆盖 `releases\factory.hex`、`Stage4B.hex`、`Stage4B_ota.bin`。阶段 5 复编哈希与 `c61768e` 一致。  
改过 `uvproj` 后请关闭并重开 µVision。命令行编译避免再开第三个 UV4。

## 8. 工厂烧录

用 **SOC Programming Tool** 烧 `releases\factory.hex`。  
不要用 µVision Download 只下 `Stage4B.hex`（会盖掉 Boot）。  
烧录前断开电源板供电（提示词第十一节）。拔 LINK 后再上电看 COM7。

## 9. 涂鸦 OTA

1. 云端上传 `releases\Stage4B_ota.bin`（文件 = `OADB` + RUN，包长 256）
2. 命令 `0x0A` / `0x0B` 由 `tuya_link.c` 处理；**不编译**官方 `protocol.c`
3. App 只写 DL；置 EEPROM `0x0200` 魔数后复位
4. Boot 校验后整段拷入 RUN，成功才清魔数；失败保留魔数并复位，**不进 App**
5. 升级中忽略按键/红外，数码管顺时针转圈
6. 升完用现有 EEPROM 设定（规格 6.3.2）
7. 写 DL 时掉电：未置魔数则旧 App 仍跑，可重试
8. 拷贝中掉电：再上电等 **30–60 s**（Boot **无日志**），不要当砖、不要立刻 ICP

擦 DL 按扇区个数 `IAP_SEC_N_DL`，不要算 `DL_BASE+DL_SIZE`（C51 `unsigned int` 16 位会溢出）。

## 10. 回归汇总（纸面，来自已通过阶段）

未在本阶段重烧、未新测实板。A2-06 **仍搁置**。

| 阶段 | 版本 | Git | 结论 |
| --- | --- | --- | --- |
| −1 | `STAGE-M1-0.1.0` | `7e2ec43` | COM7 115200 版本串 |
| 0 | `STAGE-0-0.1.0` | `83ae519` | 时基 502–504 ms |
| 1A | `STAGE-1A-0.1.2` | `04913f1` | TM1640 段码 |
| 1B | `STAGE-1B-0.1.2` | `1ac23d2` | 触摸 + 上电蜂鸣 |
| 1C | `STAGE-1C-0.1.1` | `ad27e78` | 本地 HMI |
| 2 | `STAGE-2-0.1.2` | `8d94627` | 4800 电源板；**A2-06 搁置** |
| 3 | `STAGE-3-0.1.3` | `d8581b7` | 红外 + NTC |
| 4 | `STAGE-4-0.1.3` | `22bebca` | 涂鸦三入口 |
| 4C | `STAGE-4C-0.1.0` | `22bebca` | EEPROM 掉电记忆 |
| 4B | `STAGE-4B-0.1.0` | `c61768e` | 双区 OTA |

分项清单与方法见 `docs/acceptance/stage-*.md`。交付板上应仍能：面板/红外/App 改同一套设定、4800 跟设定、NTC 环境温、掉电 2 s 后记忆、工厂 `factory.hex` + 云端 OTA。

## 11. 已知限制

- **A2-06** 电源板失联（约 5 s `--`）搁置，未实现。
- 开机 `--` 且 5 s 无 `55 5A`：也可能是外机口未接或波特率错（时钟不是 32 MHz），与失联策略无关。
- 非真 A/B、无 LDROM Boot；本片 128K 只用低 64K。
- 官方 `protocol.c` / `mcu_api.c` / `system.c` 不进 C51（`u32` 在 C51 是 16 位；体积不适配）。
- 产测宏关闭；品牌 PID 未另开产品。
- 红外采样窗口已冻结，不重调。
- 蜂鸣仅上电；故障蜂鸣不做。
- 定时剩余、DP150 开机分钟、WiFi 状态、故障 **不**进掉电记忆。
- 空/坏 EEPROM：关机、制冷、中风、72°F。
- **勿擦 EEPROM 扇区 0**（HMI）；OTA 魔数在扇区 1。
- **勿只烧 App HEX**；勿在固件里写 Option。
- Boot 拷贝期无 DispUart；静默 30–60 s 是预期。
- C51 `unsigned int` 16 位；涂鸦文件长度用 `unsigned long`。
- Sense_Lib LARGE 下不用 `PSW.CY` 判断按键位（1B 已改）。
- 原理图 PDF 无文字层，脚位不以视觉识别为准。

## 12. 隔离

未读旧 ADB（除授权 `Sense_Lib` 四文件）、test2、GL-ST-ADB、旧 Boot 源码、历史对话。  
4B 只读授权 skill `sc95-ota-dual-zone`。4C/4 用官方 Demo 的 IAP/UART 时序，不复制业务。  
阶段 5 只解析本仓库 `build\*.MAP` 与已有验收文档。

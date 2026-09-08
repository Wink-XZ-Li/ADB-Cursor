# 阶段 5 资源、回归、交付

版本标识：文档 `STAGE-5-0.1.0`  
交付固件：已验收 `STAGE-4B-0.1.0`（不改业务、不改 Option、不重烧除非你要求）  
用户验收：2026-09-08 14:16「阶段 5 通过」。接受文档 `STAGE-5-0.1.0`。  
基线：2026-09-08 13:36「阶段 4B 通过」`c61768e`  
最终报告：`docs/delivery.md`

## 本阶段做

| 编号 | 交付 | 方法 | 结果 |
| --- | --- | --- | --- |
| A5-01 | 再编译 | `scripts/build.ps1 -Rebuild` 仍 0 error；Boot≤4KB | **已做**（2026-09-08）：Boot data=45.0 xdata=0 const=0 **code=914**；App data=40.3 xdata=1298 const=106 **code=20628**。App 0 warning。Boot LX51 L10/L57（无 C51 STARTUP，已知安全）。`Stage4B.hex` SHA256 与验收件一致 `A51529A6…D44E`。factory 合并 Boot `0x0000–0x03CA`、App `0x101B–0x6174`、`dropped_lt_1000=0`。OADB payload=20853 crc=0x89FF file=20869 |
| A5-02 | 占用 | 从 `build\Boot.MAP` / `Stage4B.MAP` 写资源表与最大符号 | **已写** `docs/delivery.md` §3。Boot CODE ≈22% / 4 KB；App CODE+CONST ≈68% / `0x1080–0x87EF`；XDATA ≈16% / 8 KB；OTA 载荷 ≈68% / 30704。最大 CODE：`SENSORRENOVATE` 1052、`IR_LINK_POLL` 1008、`HMI_POLL` 864。最大 XDATA：`WIFI_UART` 512、`TUYA_LINK` 301 |
| A5-03 | 回归表 | 汇总已通过阶段的验收项；**A2-06 仍搁置** | **已写** `docs/delivery.md` §10。纸面汇总 −1…4B，本阶段未新测实板 |
| A5-04 | 已知限制 | Option 32 MHz、勿只烧 App HEX、Boot 静默拷贝等 | **已写** `docs/delivery.md` §11 |
| A5-05 | 最终报告 | `docs/delivery.md`：产物、哈希、地图、口、操作 | **已写** |
| A5-06 | 运行时架构图 | archify `architecture`，8–12 组件，一条主路径，外部依赖与信任边界，辅助进卡片 | **已写** `docs/architecture/adb-runtime.html` 与 `docs/architecture/adb-runtime.visual-check.2048x1320.light.png` |

## 本阶段不做

新功能、A2-06、改 PID、改红外采样、读旧工程、阶段 6。不把未测经验写成新结论。不改 `version.h`。

## 产物哈希（与 `c61768e` 一致）

| 文件 | SHA256 |
| --- | --- |
| `releases\Stage4B.hex` | `A51529A65405622E1C6159C6E1256924210F2172BD2EA257103D1EDAF9CFD44E` |
| `releases\factory.hex` | `D48F7AAFB2F65EA7213872D9D0C61177C1CDAC88E696339C39B02779E1CB1B0C` |
| `releases\Stage4B_ota.bin` | `64B357B86F0D55FF59ADDA76B2045F7050ABBE41F218D99D9E40050587FD5729` |

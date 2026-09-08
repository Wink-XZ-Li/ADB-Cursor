# 红外输入脚确认（阶段 3）

未读旧工程红外源码。脚位来自授权 `ORBEK-LK-GLADB-D V1.0(1).PcbDoc` + 赛元 SOP28 手册。

## 板级

| 项 | 值 |
| --- | --- |
| MCU | U8 `SC95F8763`，封装 `LC-SOIC-28_300mil`（SOP28） |
| 接收头 | REC1，封装 `WH1738-MF`（红外） |
| 网络名 | `P_REC` |
| U8 焊盘 | **pin 12** 接 `P_REC`（经 R50/R52） |

同芯片已验收脚与网络一致：pin 5/6 = `RX_Power`/`TX_Power`（P4.5/P4.4），pin 13/14 = `P_SCL`/`P_SDA`（P3.1/P3.0），pin 19/20 = `RX_WIFI`/`TX_WIFI`（P2.0/P2.1），pin 23 = `PWM_BUZZ`（P2.4），pin 28/1 = `TX_Disp`/`RXD_Disp`（P0.5/P0.6）。

PCB 上另有未贴的 `IC1` LQFP48 `HC32L180` 占位，与本板 8763 无关。

## 手册 SOP28 pin 12

赛元 `SC95F8763` SOP28/TSSOP28：**pin 12 = P3.2 / S1 / C2 / TK5**。  
P3.2 **没有** INT0/INT1/INT2（INT1 在 P4.0–P4.3 等）。触摸掩码 `0x10A40500` 不含 TK5，P3.2 可作 GPIO。

## 实现

输入：P3.2 高阻上拉。Timer1 每 50 µs 采样（Timer0 仍为 1 ms 时基）。

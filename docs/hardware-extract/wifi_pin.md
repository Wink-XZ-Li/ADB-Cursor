# WiFi 串口脚（阶段 4）

未读旧工程 WiFi 初始化。脚位来自已验收 PcbDoc 网名 + 官方 8763 Demo `Uart_Init.c`。

## 板级

| 项 | 值 |
| --- | --- |
| 网名 | `TX_WIFI` / `RX_WIFI` / `P_Wifi_Power` |
| U8 | SOP28 **pin 20 = P2.1**、**pin 19 = P2.0**（阶段 3 `ir_pin.md` 已对照） |
| 供电 | **P2.6 拉低** 模块才有电（用户确认；PcbDoc 网名 `P_Wifi_Power`） |
| 提示词 | P2.1 TX / P2.0 RX |

## 手册 / Demo

官方 `Uart_Init.c`：`P2CON &= 0xFC`、`P2PH |= 0x03`，使用 **UART0**（`SCON`/`SBUF`，中断 4）。  
官方 `USCI1_Init.c` 的 UART1 脚是 **P1**，不是本板 WiFi 口，阶段 4 不用 USCI1。

USCI0 已给日志 P0.5/P0.6；USCI2 已给电源板 P4.4/P4.5。

## 与 Timer1 冲突

Demo 默认用 **Timer1** 作 UART0 波特率。阶段 3 红外已占用 Timer1（50 µs 采样，向量 `0x001B`）。  
同一 Demo 提供 **Timer2** 作 UART0 波特率的写法。阶段 4 实现时用 Timer2，不抢 Timer1。

涂鸦波特率 **9600**（产品协议 PDF，不是 115200）。

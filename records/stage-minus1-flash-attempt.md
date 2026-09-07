# 阶段 -1 烧录尝试

时间：2026-09-07 13:35–13:37

## 事实

- 电源板供电已断开；DispUart 已接到 COM7（用户确认）。
- `UV4 -f` 第一次返回码 0，约 12 s；未生成 `uv4_flash.log`；`StageMinus1.build_log.htm` 无 Compile/Erase/Program/Verify 正文；HEX SHA256 仍为 `CB70822E…8D5E80`（11:56 编译产物，未被改写）。
- COM7 @ 115200 监听超过 30 s，**0 字节**。日志 `records/stage-minus1-flash.log` 为空。
- 第二次 `UV4 -f`（日志改到 `%TEMP%`）弹出 μVision 窗口后停住，未写出日志。
- 设备管理器可见 CH340 COM7；未见名称含 SC-Link/SOC 的编程器设备。
- 未改芯片 Option，未改工程调试器 DLL。

## 未证实的假设（下一步要区分）

- μVision 5.11 的 `-f` 在本机不能无人值守完成下载。
- SC-Link 当前未枚举，或工程 Flash 配置 `InvalidFlash=1` 导致并未下载。
- 即使已下载，115200 无输出也可能是主频不是 32 MHz 或 TX 未接到 CH340 RX。先要有“已编程”证据，再测波特率。

## 处理

请在当前已打开的 ADB-Cursor μVision 窗口执行 Flash Download。完成后回复，再抓 COM7。

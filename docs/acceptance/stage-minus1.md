# 阶段 -1 验收清单

版本标识：`STAGE-M1-0.1.0`（2026-09-07）  
工程：`D:\嵌入式项目文件夹\ADB-Cursor\ADB-Cursor.uvproj`  
编译脚本：`scripts\build.ps1`（C51/A51/BL51/OH51，不经 UV4 GUI）  
应烧录文件：`D:\嵌入式项目文件夹\ADB-Cursor\build\StageMinus1.hex`  
副本：`D:\嵌入式项目文件夹\ADB-Cursor\releases\StageMinus1.hex`  
SHA256：`CB70822E9BE6F5BC1D07DD839764B784C3EF9D37BE7DE6A795485018188D5E80`  
大小：1570 bytes  
占用：data=23.0  xdata=0  code=499  
编译：0 Error / 0 Warning（C51 V9.53.0.0 + BL51 V6.22）  
烧录配置：保持工程已有 `SOC_8051_Driver.dll`，不改芯片 Option。  
Git：阶段通过后提交，作为可恢复基线。  
用户验收：2026-09-07 13:47「阶段 -1 通过」。

## 本阶段输入

- 空/骨架 Keil C51 工程、官方头文件与手册、已知 DispUart 脚位。

## 本阶段不做

触摸、显示业务、HMI、电源板协议、红外、涂鸦、OTA。

## 验收项

| 项 | 方法 | 预期 | 结果 |
| --- | --- | --- | --- |
| 编译 | `scripts/build.ps1 -Rebuild` | 0 error；有 HEX | **已编译**：0/0，HEX SHA256 如上 |
| 烧录 | 确认电源板供电断开后 `scripts/flash.ps1 -ConfirmPowerDisconnected` | 下载成功，MCU 运行 | 命令行 Download 曾因空 Flash 工具失败；板上已跑本版本（外部串口见到 `STAGE-M1-0.1.0`），推断随后由 μVision GUI 下载成功。未改 Option。 |
| Hello | DispUart 115200 8N1，`scripts/serial_capture.ps1` | 周期性出现 `ADB-Cursor STAGE-M1-0.1.0 2026-09-07 tick=N`，N 递增 | **外部串口已抓到**（13:41:59 COM7@115200，tick=2533–2569 递增）。你在 SerialDebug 上的乱码与本机 9600 抓取同类，判定为工具波特率不是 115200。周期约 55 ms/行，快于代码里标称的 500 ms，阶段 0 再校准时基。 |
| 非唯一日志口 | 查源码 / 抓线 | 仅初始化 USCI0；未初始化 P4.4/P4.5 与 P2.1/P2.0 | 源码已满足 |
| Blink | 观察 P0.4 或任意可见指示 | 约 0.5 s 翻转。**不作为通过的必要项** | 未验证 |
| 异常 | 拔掉日志线仍应保持周期运行 | 复位后仍能再抓到 banner | 未验证 |

不能仅凭固件自打印“成功”判定通过。以外部串口文本为准。

## 操作步骤

1. 断开电源板供电（必须口头/文字确认）。
2. SC-Link 连接目标板，板子供电（ADB 侧）。
3. 编译并烧录上述 HEX。
4. DispUart：MCU P0.5→USB 串口 RX，P0.6 可悬空或接 USB TX，共地。主机 115200 8N1。
5. 复位后抓 8 s 日志。预期约每 0.5 s 一行 banner，`tick` 递增。
6. 回复“阶段 -1 通过”或失败现象（原始日志、接线、是否复位）。

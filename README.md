# ADB-Cursor

房车空调 ADB 固件（SC95F8763 / Keil C51 / SC-Link）。本目录为独立实验工程，不继承旧工程 Git 历史。

当前阶段：**-1**（最小 Blink/Hello）。未完成实板验收前不得进入后续阶段。

- 阶段计划：`docs/stage-plan.md`
- 资料与复用：`docs/sources.md`
- 阶段 -1 验收：`docs/acceptance/stage-minus1.md`

```powershell
.\scripts\build.ps1 -Rebuild
# 确认电源板供电已断开后：
.\scripts\flash.ps1 -ConfirmPowerDisconnected
.\scripts\serial_capture.ps1 -Port COM7 -Baud 115200
```

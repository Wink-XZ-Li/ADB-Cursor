# ADB-Cursor

房车空调 ADB 固件（SC95F8763 / Keil C51 / SC-Link）。本目录为独立实验工程，不继承旧工程 Git 历史。

当前阶段：**5 已通过**（资源、回归、交付）。交付固件为已验收 **`STAGE-4B-0.1.0`**。文档 `STAGE-5-0.1.0`。

- 交付报告：`docs/delivery.md`
- 运行时架构：`docs/architecture/adb-runtime.html`
- 阶段计划：`docs/stage-plan.md`
- 资料与复用：`docs/sources.md`
- 阶段 5 验收：`docs/acceptance/stage-5.md`

```powershell
.\scripts\build.ps1 -Rebuild
# 工厂烧录用 SOC Programming Tool 烧 releases\factory.hex（不要只烧 App HEX）
# 确认电源板供电已断开后，若用 µVision/SC-Link 调试：
.\scripts\flash.ps1 -ConfirmPowerDisconnected
.\scripts\serial_capture.ps1 -Port COM7 -Baud 115200
```

# 隔离与接触记录

## 进程列表附带接触

列举 UV4 进程时看到命令行：

- `D:\嵌入式项目文件夹\ADB\ADB.uvproj`（旧 ADB 工程，10:16 起已打开）
- `D:\嵌入式项目文件夹\ADB-Cursor\ADB-Cursor.uvproj`（本工程 GUI）

只见到路径，**未打开、未读、未比较**旧 ADB 源码。因此改用 C51 命令行编译，避免再启动第三个 UV4 抢同一工程。

## 遵守

- 未读取旧 ADB、test2、GL-ST-ADB、Boot/ApTest、历史对话、agent-transcripts。
- 2026-09-07：用户要求按本仓库 `docs/led_segment_map.md` 纠正 GRID 扫描；只读该文件，未打开其提到的 GL-ST-ADB 工程。
- 未用父目录搜索或子代理扩大范围。
- OTA skill 未读取。
- 2026-09-07：读取赛元官网 TouchKey 应用指南（T1 调用顺序），未读旧工程触摸业务。
- 2026-09-07 阶段 1B：按授权读取并复制 `D:\嵌入式项目文件夹\ADB\Sense_Lib` 下 4 个文件。未读取旧工程其余源码。
- 通用 skill 仅用显式本工程路径。

## 提示词笔误与 ADB-Codex

提示词第二节指定本目录 `ADB-Cursor`；第十三节写“检查 ADB-Codex”。按第十三节做了**存在性与目录名列表**，未读其源码、文档、提交或验收记录。

`D:\嵌入式项目文件夹\ADB-Codex` 存在，且含 `.git`、`firmware`、`docs` 等。本目录 `uvproj` 字节数与其同名工程文件同为 13986，来源可能相关，**未打开对照**。

处理：不从 ADB-Codex 复制；不把它当作参考实现；不把本工程描述为“从零空白目录创建”。本目录开工时已有 Keil 骨架。独立性声明不覆盖该骨架的来源。

## 官方 Demo 接触

为核对 SC95F876x 工程字段和 USCI0 UART 寄存器用法，读取了官方 pack 中 Demo 的 `DEMO.uvproj` 器件段、`USCI0_Init.c`、`IO_Init.c`、`Function_Init.H` 和 `main.c` 中 WDT 一行。未复制 Demo 工程，未采用其错误器件名 SC95F8617，未启用其 128K Merge32K/banking。

## 原理图视觉识别

ORBEK PDF 无文字层。一次整图描述给出与提示词一致的 P0.5/P0.6、P4.4/P4.5、P2.1/P2.0；局部放大描述出现 STM32 风格脚名，与 8051 资料冲突，**作废**。阶段 -1 不以视觉结果冻结 LED 脚。

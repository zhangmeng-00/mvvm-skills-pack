﻿---
name: project-implementation
description: "Use when implementing planned Qt/MVVM code, feature work, bug fixes, refactors, Model/ViewModel/View integration, or startup changes after specification and staged planning are complete."
---

# Project Implementation

## 职责

约束代码阶段小步实现、最小影响面、遵守 MVVM 分层和项目注释规则。

## 实施前检查

- `docs/todo.md` 已有当前任务计划。
- 架构相关内容已有规格或明确设计。
- 已确认本次修改边界。
- 已决定需要调用哪些专项 skills。

## 必须调用

- 启动结构：`write-app-bootstrapper`。
- Model：`write-model`。
- ViewModel：`write-viewmodel`。
- View：`write-view`。
- QSS：`write-view-style`。

## 编码规则

- 只修改当前目标必需文件。
- 匹配现有项目风格。
- 不顺手重构无关代码。
- 新增或主动重写的类、函数、接口使用中文块注释。
- 修 bug 必须追根因。

## 禁止事项

- 不得跳过计划直接编码。
- 不得扩大修改范围。
- 不得新增未要求的抽象层或配置项。
- 不得用硬编码掩盖架构问题。

## 自查清单

- [ ] 是否有可验证目标。
- [ ] 是否只实现当前切片。
- [ ] 是否调用了正确的专项 skill。
- [ ] 是否更新 `docs/todo.md` 进度。

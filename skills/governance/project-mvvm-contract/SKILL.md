﻿---
name: project-mvvm-contract
description: "Use when checking Qt MVVM layering, Model/ViewModel/View responsibilities, UI/business separation, cross-layer calls, existing-project MVVM migration, or new feature boundary design."
---

# Project MVVM Contract

## 职责

约束 MVVM 分层和调用方向，避免越级调用、逆向调用和 UI/业务混杂。

## 必须调用

- 总体分层：`design-mvvm-framework`。
- 现有项目重构：`refactor-to-mvvm-project`。
- 写 Model：`write-model`。
- 写 ViewModel：`write-viewmodel`。
- 写 View：`write-view`。
- 写样式：`write-view-style`。

## 分层红线

- View 只负责控件、布局、绑定、用户事件转发和样式接口。
- ViewModel 只负责 UI 状态、属性、命令转换和订阅 Model 状态。
- Model 只负责业务、设备、数据源、算法、后台任务和外部接口。
- Bootstrapper/main 只负责启动、注册和顶层 View 创建。

## 禁止事项

- View 不得直接调用 Model。
- Model 不得知道 View 或 ViewModel。
- ViewModel 不得持有 View 指针。
- Bootstrapper 不得创建 ViewModel。
- 不得用公共工具类偷偷承载业务规则。

## 自查清单

- [ ] 是否列出每个组件职责。
- [ ] 是否检查越级调用和逆向调用。
- [ ] 是否把用户事件经 ViewModel 转成命令。
- [ ] 是否把业务逻辑从 View 移出。
- [ ] 是否保持 Model 与 UI 无关。

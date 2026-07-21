﻿---
name: design-mvvm-framework
description: "Use when planning a MediatorCore/MediatorRuntime/MediatorBinding MVVM feature, new program, complex requirement, or cross-layer change; 用于拆分 Model/ViewModel/View/Bootstrapper 职责、规划 tag 流、启动关系和专项 skill 调用顺序。"
---

# Design MVVM Framework

## 职责

作为 MVVM 总纲/编排器，判断任务范围并决定后续使用哪些专项 skill。

## 何时使用

- 用户说“增加一个功能”，且尚未确认是单层小改。
- 从零创建程序、示例或完整模块。
- 功能涉及 Model、ViewModel、View、Bootstrapper 中至少两层。
- 需求存在多种架构解释。
- 需要决定后续调用哪些专项 skill。

## 输出契约

必须输出：
- 功能目标和验收标准。
- 涉及层级：Model、ViewModel、View、Bootstrapper、Message、Policy。
- 组件职责表。
- 专项 skill 调用顺序。
- 是否需要 Superpowers 规格与计划。
- 跨层自查清单。

## 分流规则

单层小改：
- 只改 View：使用 `$write-view`。
- 只改 ViewModel：使用 `$write-viewmodel`。
- 只改 Model：使用 `$write-model`。
- 只改启动顺序：使用 `$write-app-bootstrapper`。
- 只改 Policy：使用 `$design-mediator-policy`。
- 只改 tag 或自定义类型：使用 `$design-mediator-message`。

跨层功能推荐顺序：
1. `$design-mvvm-framework`
2. `$design-mediator-message`
3. `$design-mediator-policy`
4. `$write-app-bootstrapper`（如涉及启动或全局注册）
5. `$write-model`
6. `$write-viewmodel`
7. `$write-view`
8. 验证与总结自查

## 不负责

- 不写完整 Model/ViewModel/View 模板。
- 不写完整 Policy 实现细节。
- 不写完整 tag/type 注册细节。

## 自查清单

- [ ] 是否判断单层小改还是跨层功能。
- [ ] 是否定义可验证目标。
- [ ] 是否列出组件职责。
- [ ] 是否决定 Message/Policy 是否需要专项设计。
- [ ] 是否明确专项 skill 调用顺序。
- [ ] 是否避免越级调用和逆向调用。

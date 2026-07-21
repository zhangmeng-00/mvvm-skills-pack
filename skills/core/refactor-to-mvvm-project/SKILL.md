﻿---
name: refactor-to-mvvm-project
description: "Use when auditing and refactoring an existing Qt/C++ project into MediatorCore/MediatorRuntime/MediatorBinding MVVM; requires staged migration, UI/business separation, tag/policy planning, and no big-bang rewrite."
---

# Refactor To MVVM Project

## 职责

阅读现有 Qt/C++ 项目，将旧项目按阶段重构到 MediatorCore MVVM 架构。此 skill 是“已有项目改造入口”，与 `create-mvvm-project` 的“从零创建入口”并列。

## 适用场景

- 用户要求把现有项目改成 MVVM 架构。
- 现有 View 中混有业务逻辑、设备访问、数据处理或跨模块调用。
- 现有 Model/服务类直接操作 UI。
- 旧项目依赖复杂信号槽链路，需要迁移到 Mediator tag 流。
- 需要把已有功能逐步拆成 Model、ViewModel、View 和 Bootstrapper。

## 强制流程

### 1. 现状审计

先阅读项目，不直接改代码。至少输出：
- 项目入口和启动流程。
- UI/View 类清单。
- 业务逻辑、设备访问、数据源、定时器和外部接口清单。
- 当前信号槽、回调、全局变量、单例和跨对象调用关系。
- 违反 MVVM 的位置。
- 可保留的旧代码和必须迁移的旧代码。

### 2. 目标架构设计

根据审计结果设计目标结构：
- 哪些类成为 Model。
- 哪些 UI 状态进入 ViewModel。
- 哪些控件和布局保留在 View。
- 哪些启动逻辑进入 `main.cpp` 或 `ModelBootstrapper`。
- 哪些消息需要通过 Mediator tag 传递。
- 哪些自定义类型需要 Qt 元对象注册。
- 哪些订阅需要 Policy 或新增 Policy。

跨层或复杂迁移必须先进入 Superpowers 规格与计划流程。

### 3. 迁移切片

禁止一次性大爆炸式重写。按功能闭环拆分迁移：
- 每次只迁移一个页面、一个业务流程或一条数据链路。
- 每个切片都要定义旧行为、目标行为和验证方式。
- 优先迁移“View 直接执行业务”的位置。
- 能保留旧接口的阶段先保留，等新链路验证后再删除旧路径。

推荐迁移顺序：
1. 建立启动结构：调用 `write-app-bootstrapper`。
2. 设计 tag/type：调用 `design-mediator-message`。
3. 设计 Policy：调用 `design-mediator-policy`。
4. 抽出业务 Model：调用 `write-model`。
5. 抽出 UI 状态 ViewModel：调用 `write-viewmodel`。
6. 瘦身 View 并建立绑定：调用 `write-view`。
7. 如需 UI 样式迁移或优化：调用 `write-view-style`。

## 输出契约

首次审计后必须输出：
- 现状架构摘要。
- MVVM 违规点清单。
- 目标组件拆分表。
- tag/type 初稿。
- Policy 初稿。
- 分阶段迁移计划。
- 第一阶段可验证目标。

每次实现后必须输出：
- 当前阶段迁移范围。
- 调用过的专项 skill。
- 构建、测试、日志或行为验证证据。
- 剩余迁移清单。

## 不负责

- 不从零复制框架模板；从零创建用 `create-mvvm-project`。
- 不跳过审计直接重写项目。
- 不一次性重写所有页面和业务。
- 不删除未验证的旧逻辑。
- 不让 View 直接调用 Model。
- 不让 Model 或启动器创建 ViewModel。

## 自查清单

- [ ] 是否先审计现有项目而不是直接改代码。
- [ ] 是否识别 View、ViewModel、Model、Bootstrapper 的目标职责。
- [ ] 是否列出旧信号槽/回调到 Mediator tag 的迁移映射。
- [ ] 是否调用或准备调用 `design-mediator-message` 和 `design-mediator-policy`。
- [ ] 是否按功能闭环拆分迁移切片。
- [ ] 是否避免大爆炸式重写。
- [ ] 是否每个阶段都有验证方式。
- [ ] 是否保留旧行为对照并记录剩余风险。

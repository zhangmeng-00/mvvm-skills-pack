﻿---
name: project-design
description: "Use when designing a large Qt/MVVM program from scratch, refactoring an existing project, changing architecture, splitting modules, defining system boundaries, or planning Model/ViewModel/View/Bootstrapper responsibilities."
---

# Project Design

## 职责

约束 AI 在大型程序设计阶段先定义目标、边界、架构和验证方式，再进入实现。此 skill 是治理层，应编排现有 MVVM/Superpowers skills，不替代它们。

## 必须调用

- 从零创建项目：先用 `design-mvvm-framework`，必要时用 `create-mvvm-project`。
- 现有项目重构：使用 `refactor-to-mvvm-project`。
- 涉及 tag/type：使用 `design-mediator-message`。
- 涉及订阅策略：使用 `design-mediator-policy`。
- 复杂需求：进入 Superpowers 规格与计划流程。

## 必须产出

- 项目目标与非目标。
- 用户角色与核心场景。
- 系统边界。
- 架构候选方案与取舍。
- 推荐架构。
- 模块职责表。
- 数据流、控制流、状态流。
- 错误处理、日志和验证策略。
- 写入 `docs/todo.md` 的分阶段实施计划。

## 禁止事项

- 不得直接写代码。
- 不得边写代码边决定架构。
- 不得一次性设计成无法验证的巨大阶段。
- 不得引入用户未要求的功能。
- 不得省略验证策略。

## 自查清单

- [ ] 是否先定义目标、非目标和系统边界。
- [ ] 是否比较 2-3 个架构方案。
- [ ] 是否明确 Model/ViewModel/View/Bootstrapper 职责。
- [ ] 是否决定需要调用哪些 MVVM 核心 skills。
- [ ] 是否把计划写入 `docs/todo.md`。

﻿---
name: project-planning
description: "Use when a large-program task has more than three substantive steps, architecture decisions, cross-layer features, refactors, staged verification, or docs/todo.md planning requirements."
---

# Project Planning

## 职责

把需求拆成可执行、可验证、可追踪的阶段。此 skill 管计划纪律，不替代 `design-mvvm-framework` 或具体代码 skills。

## 必须调用

- 架构或跨层功能：先用 `design-mvvm-framework`。
- 从零创建：配合 `create-mvvm-project`。
- 现有项目重构：配合 `refactor-to-mvvm-project`。
- 复杂任务：使用 Superpowers 计划流程。

## 计划格式

写入 `docs/todo.md`：

```text
- [ ] 步骤：做什么。验证：如何证明完成。
```

每个阶段必须有：
- 可验证目标。
- 修改边界。
- 依赖的 skills。
- 验证方式。
- 剩余风险记录位置。

## 禁止事项

- 不得写“完成即可”这类不可检查计划。
- 不得把多个功能闭环塞进一个步骤。
- 不得跳过验证计划。
- 不得在计划未确认时进入代码实现。

## 自查清单

- [ ] 是否每一步都有验证方式。
- [ ] 是否把跨层功能拆成多个闭环。
- [ ] 是否记录需要调用的现有 skills。
- [ ] 是否在执行中更新勾选状态。

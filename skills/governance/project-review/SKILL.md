﻿---
name: project-review
description: "Use when the user asks for review, a stage is complete, merge preparation is needed, or Codex must inspect architecture risks, missing tests, behavior regressions, or MVVM layering violations."
---

# Project Review

## 职责

以代码审查姿态检查问题，优先找 bug、风险、行为回归、缺测试和分层违规。

## 必须关注

- 是否违反 MVVM 分层。
- 是否存在越级调用、逆向调用、循环依赖。
- 是否存在未验证的行为变化。
- 是否缺失测试或缺少行为证据。
- 是否超出本次任务边界。

## 可配合调用

- `superpowers:requesting-code-review`
- `design-mvvm-framework`
- `project-verification`

## 输出格式

先列问题，按严重程度排序。每个问题包含：
- 文件和位置。
- 风险说明。
- 建议修复方向。

## 禁止事项

- 不得先写总结再埋问题。
- 不得用泛泛表扬代替审查。
- 不得忽略缺测试风险。

## 自查清单

- [ ] 是否问题优先。
- [ ] 是否按严重程度排序。
- [ ] 是否覆盖分层和验证风险。

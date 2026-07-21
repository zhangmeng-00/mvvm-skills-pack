﻿---
name: project-retrospective
description: "Use when the user corrects AI behavior, implementation needs rework, verification fails, architecture judgment was wrong, workflow was skipped, or lessons must be recorded in docs/常见问题.md."
---

# Project Retrospective

## 职责

把用户纠正和失败经验沉淀成可执行、可检查的项目经验，避免重复犯错。

## 必须做

- 记录触发原因。
- 写清错误行为。
- 写清以后必须怎么做。
- 将经验追加到 `docs/常见问题.md`。
- 如果经验影响所有项目，再提示是否同步到全局规则。

## 可配合调用

- `superpowers:receiving-code-review`
- `project-planning`
- `project-verification`

## 经验格式

```text
## YYYY-MM-DD 主题
- 触发：发生了什么。
- 问题：AI 哪里做错。
- 规则：以后必须如何执行。
- 检查：如何确认没有再犯。
```

## 禁止事项

- 不得写空泛口号。
- 不得只道歉不改规则。
- 不得把用户纠正当成一次性上下文。

## 自查清单

- [ ] 是否写入 `docs/常见问题.md`。
- [ ] 是否规则可执行、可检查。
- [ ] 是否判断是否需要同步全局规则。

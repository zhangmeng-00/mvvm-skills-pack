﻿---
name: project-message-design
description: "Use when a feature involves Mediator tags, Publish/Subscribe, QVariant payloads, Qt custom metatype registration, Policy/replay semantics, cross-module state, or event-flow design."
---

# Project Message Design

## 职责

约束项目内消息契约先行，避免边写代码边发明 tag、类型和订阅策略。

## 必须调用

- tag/type 契约：`design-mediator-message`。
- Policy/replay：`design-mediator-policy`。
- Model 订阅/发布：`write-model`。
- ViewModel 订阅/命令：`write-viewmodel`。

## 必须产出

- tag 名称。
- 发布方。
- 订阅方。
- QVariant 实际类型。
- Subscribe 默认值。
- 是否自定义类型。
- `Q_DECLARE_METATYPE` 位置。
- `qRegisterMetaType<T>()` 位置。
- Policy 和 replay 语义。

## 禁止事项

- 不得用临时字符串 tag 随写随发。
- 不得让 Subscribe 默认值类型和 Publish 实际类型不一致。
- 不得发布未注册的跨线程自定义类型。
- 不得混淆 Policy 选择和 tag 命名职责。

## 自查清单

- [ ] 是否所有 tag 都有发布方和订阅方。
- [ ] 是否所有类型都能进入 QVariant。
- [ ] 是否自定义类型已注册到 Qt 元对象系统。
- [ ] 是否每个订阅都有 Policy 决策。

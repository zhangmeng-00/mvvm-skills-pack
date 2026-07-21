﻿---
name: design-mediator-message
description: "Use when designing MediatorCore tags, Publish/Subscribe contracts, QVariant payload types, custom Qt metatype registration, Q_DECLARE_METATYPE, qRegisterMetaType<T>(), and Subscribe default value consistency."
---

# Design Mediator Message

## 职责

设计 Mediator 消息契约：tag、发布方、订阅方、数据类型、自定义类型注册。

## 输出契约

必须输出：
- tag 名称。
- 发布方。
- 订阅方。
- QVariant 类型。
- 自定义类型定义位置。
- `Q_DECLARE_METATYPE` 位置。
- `qRegisterMetaType<T>()` 注册位置。
- `Subscribe()` 默认值示例。
- `Publish()` 示例。

## 规则

- tag 命名优先使用 `{domain}/{resource}[/{action}]`。
- 命令类 tag 推荐包含 `cmd`。
- 状态类 tag 推荐包含 `state` 或清晰的业务资源名。
- 自定义类型通过 `QVariant::fromValue()` 发布前必须 `Q_DECLARE_METATYPE(TypeName)`。
- 跨线程传递前必须调用 `qRegisterMetaType<TypeName>("TypeName")`。
- `Subscribe()` 默认值类型必须与 `Publish()` 实际类型一致。

## 不负责

- 不选择 Policy。
- 不编写 Model/ViewModel/View。
- 不设计业务 handler。

## 自查清单

- [ ] tag 命名是否清晰。
- [ ] 发布方和订阅方是否明确。
- [ ] `Subscribe()` 默认值类型是否与 `Publish()` 实际类型一致。
- [ ] 自定义类型是否 `Q_DECLARE_METATYPE`。
- [ ] 跨线程前是否 `qRegisterMetaType<T>()`。
- [ ] 是否避免用错误 QVariant 类型占位。

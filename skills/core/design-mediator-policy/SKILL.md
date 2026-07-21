﻿---
name: design-mediator-policy
description: "Use when choosing or designing MediatorCore subscription policies such as AlwaysPolicy, AlwaysPolicy(true), ValueChangedPolicy, replay, median filter, average filter, throttle, debounce, or range filtering."
---

# Design Mediator Policy

## 职责

为订阅选择现有 Policy，或根据需求设计新的 `ISubscriptionPolicy` 实现。

## 决策规则

- 命令：优先 `AlwaysPolicy`。
- 实时状态：`AlwaysPolicy` 或 `AlwaysPolicy(true)`。
- 晚订阅需要最近值：`AlwaysPolicy(true)`。
- 仅变化通知：`ValueChangedPolicy`。
- 需要中值滤波、均值滤波、节流、防抖、范围过滤等语义：新增 `ISubscriptionPolicy`。

## 新增 Policy 要求

新增前先确认现有 Policy 是否可复用。确需新增时必须明确：
- 类名。
- 参数，例如窗口大小、阈值、时间间隔。
- 输入类型，例如 `double`、`int`、自定义类型。
- 内部状态，例如滑动窗口、累计和、上次时间戳。
- 状态更新时机。
- 边界行为：窗口未满、无效值、类型转换失败、NaN、重复值。
- 测试清单。

## 输出契约

必须输出：
- tag 名称。
- 使用已有 Policy 或新增 Policy。
- 默认值。
- 是否 replay last value。
- 选择理由。
- 需要测试的通知行为。
- 若新增 Policy：类名、参数、内部状态、类型约束、边界行为、测试清单。

## 不负责

- 不定义 tag 命名。
- 不注册自定义数据类型。
- 不编写 Model/ViewModel 业务 handler。

## 自查清单

- [ ] 是否先判断能复用现有 Policy。
- [ ] 命令是否使用 `AlwaysPolicy`。
- [ ] 需要晚订阅回放的状态是否使用 replay。
- [ ] 仅变化通知是否使用 `ValueChangedPolicy`。
- [ ] 新增 Policy 是否定义输入类型、状态和边界。
- [ ] 新增 Policy 是否有测试覆盖窗口未满、无效值、重复值等边界。

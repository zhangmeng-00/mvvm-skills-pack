﻿---
name: write-model
description: "Use when implementing a MediatorCore BaseModel subclass for backend business logic, devices, data sources, timers, external APIs, tag-to-private-handler mapping, Publish output, child Model startup, lifecycle, and logging."
---

# Write Model

## 职责

编写 `BaseModel` 子类，负责后台业务、设备、数据源、定时器、外部接口和发布状态。

## 规则

- 继承 `BaseModel`，不要直接继承 `ActorObserve`。
- 构造函数只做成员初始化。
- 订阅和 handler 注册放在 `SetupSubscriptions()`。
- `SetupSubscriptions()` 只表达 `Subscribe(...)` 和 `RegisterHandlers({...})`。
- `RegisterHandlers({...})` 推荐只保留 `tag -> 私有成员函数` 映射。
- lambda 只做转发，不写业务分支。
- `ObserveData()` 尽量只有一行 `Enqueue(tag, value);` 或等价 dispatch。
- 私有 handler 第一行必须 `LOG_DEBUG`。
- handler 不直接调用 View/ViewModel/其他 Model。
- Model 可以启动子 Model，但必须通过 `AppContext` 或等价容器托管，并传递显式启动参数。
- 遵循 `$design-mediator-message` 输出的 tag/type 契约。
- 遵循 `$design-mediator-policy` 输出的 Policy 决策。

## 推荐输出

- Model 头文件和实现文件。
- `SetupSubscriptions()`。
- handler 映射。
- 每个 tag 对应的私有 handler 函数。
- `initialize()` / `shutdown()`。
- 必要测试建议。

## 推荐模式

`SetupSubscriptions()` 保持短小，只表达订阅和映射：

```cpp
void SineModel::SetupSubscriptions()
{
    Subscribe("sine/cmd/start", QVariant(false),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/stop", QVariant(false),
              std::make_shared<AlwaysPolicy>());

    RegisterHandlers({
        {"sine/cmd/start", [this](const QVariant& v) { handleStartCommand(v); }},
        {"sine/cmd/stop",  [this](const QVariant& v) { handleStopCommand(v); }},
    });
}
```

业务逻辑放到私有函数：

```cpp
void SineModel::handleStartCommand(const QVariant& value)
{
    LOG_DEBUG("SineModel",
              QString("sine/cmd/start = %1").arg(value.toBool() ? "true" : "false"));
    if (!value.toBool()) {
        return;
    }

    m_running = true;
    m_paused = false;
    startTimer();
    Publish("sine/status", QVariant("running"));
    LOG_INFO("SineModel", "Signal generation started");
}
```

## 不负责

- 不创建 View/ViewModel。
- 不设计 UI。
- 不选择或设计 Policy。
- 不定义 tag/type 契约。

## 自查清单

- [ ] 是否继承 `BaseModel`。
- [ ] 构造函数是否只做成员初始化。
- [ ] 订阅和 handler 是否在 `SetupSubscriptions()`。
- [ ] `RegisterHandlers()` 是否只做 tag 到私有函数的映射。
- [ ] `ObserveData()` 是否只有一行入队/dispatch。
- [ ] 私有 handler 第一行是否 `LOG_DEBUG`。
- [ ] 是否没有直接调用 View/ViewModel/其他 Model。
- [ ] 子 Model 是否由 `AppContext` 托管并接收显式参数。

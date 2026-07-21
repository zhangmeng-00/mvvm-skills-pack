﻿---
name: write-app-bootstrapper
description: "Use when writing or adjusting MediatorRuntime startup code: main.cpp, ModelBootstrapper, AppContext AddComponent order, Logger tags, qRegisterMetaType<T>(), top-level View creation, and keeping ViewModels owned by Views."
---

# Write App Bootstrapper

## 职责

编写 `main.cpp` 和 `ModelBootstrapper`，集中启动顶层 Model 和后台服务。

## 规则

- `main.cpp` 只做应用壳：`QApplication`、`AppContext`、启动器、顶层 View、`app.exec()`。
- 顶层 Model 通过 `AppContext::AddComponent<T>()` 托管。
- ViewModel 不由启动器创建，必须由对应 View 创建。
- 全局自定义类型注册必须早于第一次 `Subscribe()` / `Publish()`。
- Logger tag 在启动阶段设置清楚。

## 推荐输出

- `main.cpp`。
- `ModelBootstrapper.h/.cpp`。
- `RegisterCustomMetaTypes()` 或等价入口。
- 启动顺序说明。

## 不负责

- 不创建 ViewModel。
- 不创建子 View。
- 不编写 Model 业务逻辑。
- 不选择 Policy。

## 自查清单

- [ ] `main.cpp` 是否只做应用壳。
- [ ] Model 是否通过 `AppContext::AddComponent<T>()` 托管。
- [ ] ViewModel 是否没有由启动器创建。
- [ ] 自定义类型是否早于 Subscribe/Publish 注册。
- [ ] Logger tag 是否设置清楚。
- [ ] 顶层 View 是否由 main 创建。

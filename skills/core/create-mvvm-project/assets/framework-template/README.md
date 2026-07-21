# MediatorCore — 基于 Mediator 模式的 C++/Qt 消息框架

## 1. 概述

MediatorCore 是一个基于 Qt5 的 **MVVM 消息通信框架库**，核心采用 **Mediator（中介者）** + **Observer（观察者）** + **Actor 模式** 实现组件间的解耦通信。

框架以**库**形式发布，不包含可执行应用程序。上层项目可通过 CMake `find_package` 或 `add_subdirectory` 引入。

### 1.1 设计目标

- **低耦合**：组件间通过 Mediator 通信，避免直接依赖
- **线程安全**：Mediator 运行在独立线程，Topic 以互斥锁 + 订阅者快照保护分发；ActorObserve 通过 Qt 事件循环把消息投递回对象所属线程
- **类型感知路由**：Topic 以 `typeName + tag` 双层索引，订阅时绑定期望的数据类型
- **可复用**：核心消息链路只依赖 `Qt5::Core`，可跨项目独立复用

---

## 2. 库结构

```
MediatorCore (Qt5::Core)
├── actor/          ActorObserve — Actor 模式线程隔离
├── mediator/       Mediator, Observe, Topic — 消息总线核心
├── policy/         ISubscriptionPolicy, AlwaysPolicy, ValueChangedPolicy
├── model/          IModel, BaseModel, LogEntry
└── viewmodel/      BaseViewModel

MediatorRuntime (Qt5::Core + MediatorCore)
└── app/
    ├── AppContext  — Mediator 线程管理、组件注册
    └── log/Logger  — 结构化日志（单例，通过 Mediator 发布）

MediatorBinding (Qt5::Widgets + MediatorCore)
└── ext/binding/    Binding, BindingHelper — ViewModel Q_PROPERTY → View 单向绑定

MediatorLogging (Qt5::Widgets + Qt5::Sql + MediatorCore + MediatorBinding)
└── ext/logging/    LogModel, LogListViewModel, LogListView — SQLite 日志持久化与 UI

MediatorModbus (Qt5::Network + Qt5::SerialBus + MediatorCore)
└── model/modbus/   ModbusMasterModel, ModbusModuleBase, PidPressureModule, PointDef
```

---

## 3. 核心架构

```
┌──────────────────────────────────────────────────────┐
│                   Core Framework                      │
│                                                      │
│  ┌─────────┐   ┌──────────┐   ┌────────┐            │
│  │ Observe │──►│ Mediator │──►│ Topic  │──► Policy  │
│  │(观察者)  │   │ (中介者)  │   │ (主题)  │   (策略)  │
│  └─────────┘   └──────────┘   └────────┘            │
│       │                                          │
│       ▼                                          │
│  ┌──────────────┐                                 │
│  │ ActorObserve │ — 线程隔离 + Mailbox 串行处理    │
│  └──────────────┘                                 │
└──────────────────────────────────────────────────────┘
```

### 3.1 Mediator（中介者）

- 管理 Topic（按 `value.typeName() + tag` 双层索引）
- 处理订阅：将 (observer + policy) 加入 Topic
- 处理发布：转发到 Topic→Notify()
- 维护 sticky state cache，支持晚创建订阅者 replay 最近值
- 线程安全（QMutex 保护）

### 3.2 Topic（主题）

- 保存订阅者列表（QPointer 自动清理已析构 observer）
- 应用 Policy 过滤（ValueChangedPolicy 依赖 Topic 级缓存的上次值做比较）
- 快照后回调，避免回调内修改订阅者列表导致迭代器失效

### 3.3 Observe / ActorObserve

**Observe** 是所有消息参与者的基类：
- `Subscribe(tag, value, policy)` — 向 Mediator 请求订阅
- `Unsubscribe(tag)` / `Unsubscribe(tag, value)` — 退订（带 value 参数可精确匹配类型）
- `Publish(tag, value)` — 发布消息
- `SetupSubscriptions()` — 接入 Mediator 后的初始化钩子（在此订阅，而非构造函数）
- `ObserveData(tag, value)` — 统一的业务处理入口（子类重写）

**ActorObserve** 继承 Observe，增加线程隔离：
- 通过 `handleData()` 重写拦截消息入口
- 当投递线程 ≠ 对象所属线程时，使用 `Qt::QueuedConnection` 投递
- 确保 `ObserveData` 始终在对象所属线程串行执行

### 3.4 Policy（订阅策略）

| Policy | 行为 |
|--------|------|
| `AlwaysPolicy` | 每次发布都通知，支持 `replayLastValue`（sticky） |
| `ValueChangedPolicy` | 仅在值变化时通知 |

### 3.5 BaseModel / BaseViewModel

- **BaseModel**：继承 `ActorObserve + IModel`，提供 `initialize()` / `shutdown()` / `modelName()` 生命周期接口
- **BaseViewModel**：继承 `ActorObserve`，提供 UI 事件处理方法（`onClicked` / `onTextChanged` 等），通过 `Q_PROPERTY` + signal 驱动 View 绑定

---

## 4. 目录结构

```
MediatorMVAQt/
├── core/                    # MediatorCore 库
│   ├── actor/               ActorObserve
│   ├── mediator/            Mediator, Observe, Topic
│   ├── policy/              ISubscriptionPolicy, AlwaysPolicy, ValueChangedPolicy
│   ├── model/               IModel, BaseModel, LogEntry
│   └── viewmodel/           BaseViewModel
├── app/                     # MediatorRuntime 库
│   ├── AppContext           应用上下文（Mediator 线程管理、组件注册）
│   └── log/Logger           结构化日志服务
├── ext/                     # 可选扩展库
│   ├── binding/             MediatorBinding（属性绑定）
│   └── logging/             MediatorLogging（日志持久化 + UI）
├── model/modbus/            MediatorModbus 库（Modbus TCP 通信）
├── tests/                   测试（需 MEDIATOR_BUILD_TESTS=ON）
├── docs/                    设计文档与变更记录
├── CMakeLists.txt
└── CMakePresets.json
```

---

## 5. 使用指南

### 5.1 创建 Model

```cpp
#include "BaseModel.h"
#include "AlwaysPolicy.h"

class SensorModel : public BaseModel {
    Q_OBJECT
public:
    explicit SensorModel(QObject* parent = nullptr)
        : BaseModel(parent) {}

    QString modelName() const override { return "SensorModel"; }

    void SetupSubscriptions() override {
        Subscribe("cmd/reset", QVariant(false),
                  std::make_shared<AlwaysPolicy>());
    }

    void publishTemperature(double temp) {
        Publish("sensor/temperature", temp);
    }

protected:
    void ObserveData(const QString& tag, const QVariant& value) override {
        if (tag == "cmd/reset" && value.toBool()) {
            // 处理重置
        }
    }
};
```

### 5.2 创建 ViewModel

```cpp
#include "BaseViewModel.h"

class TemperatureVM : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)

public:
    explicit TemperatureVM(QObject* parent = nullptr)
        : BaseViewModel(parent) {}

    void SetupSubscriptions() override {
        Subscribe("sensor/temperature", QVariant(0.0),
                  std::make_shared<AlwaysPolicy>(true)); // sticky
    }

    double temperature() const { return m_temp; }

signals:
    void temperatureChanged();

protected:
    void ObserveData(const QString& tag, const QVariant& value) override {
        if (tag == "sensor/temperature") {
            m_temp = value.toDouble();
            emit temperatureChanged();
        }
    }

private:
    double m_temp = 0.0;
};
```

### 5.3 属性绑定

```cpp
#include "Binding.h"

// ViewModel Q_PROPERTY → View Q_PROPERTY（单向）
Binding::BindProperty(ui->tempLabel, "text", temperatureVM, "temperature");
Binding::BindProperty(ui->listView,  "model", logListVM,      "tableModel");
```

### 5.4 使用 Logger

```cpp
#include "Logger.h"

LOG_INFO("Module", "operation completed");
LOG_WARN("Module", "something unexpected");
LOG_ERROR("Module", "error: " + errorString);
LOG_FATAL("Module", "unrecoverable failure");
```

### 5.5 应用初始化

```cpp
#include "AppContext.h"

auto& ctx = AppContext::instance();

// 创建并注册组件（自动 ConnectObserve + 调用 SetupSubscriptions）
auto* sensorModel = ctx.AddComponent<SensorModel>();
auto* tempVM      = ctx.AddComponent<TemperatureVM>();
auto* logModel    = ctx.AddComponent<LogModel>();

// Logger 需要显式注册
ctx.ConnectObserve(Logger::instance());
```

### 5.6 带类型的精确退订

```cpp
// 订阅时绑定类型
Subscribe("device/value", QVariant(0), policy);

// 退订时传入相同类型的样板值，精确匹配 typeName + tag
Unsubscribe("device/value", QVariant(0));
```

---

## 6. 线程模型

```
Main Thread                  Mediator Thread          Worker Thread (可选)
    │                            │                        │
    ├─ UI / View                 ├─ Mediator              ├─ Model
    ├─ ViewModel                 ├─ Topic                 │  (ActorObserve,
    │  (ActorObserve,            │  (消息分发)             │   useSeparateThread=true)
    │   useSeparateThread=false) │                        │
    │                            │                        │
    │  Qt::QueuedConnection ←————┼—— Qt::QueuedConnection │
    │                            │                        │
```

- Mediator 运行在独立 QThread
- ActorObserve 默认在主线程，可选独立线程
- 所有跨线程消息通过 `Qt::QueuedConnection` 投递

---

## 7. Tag 命名约定

```
# 状态数据 (state/*)
sensor/temperature
pid_pressure/0/state/pv

# 控制命令 (cmd/*)
cmd/reset
pid_pressure/0/cmd/set_sp

# 日志系统
user/logging        # 日志发布
user/logList        # 日志列表

# Modbus 模块 (模块名/实例/...)
{moduleName}/{instanceId}/state/{key}
{moduleName}/{instanceId}/cmd/{command}
```

---

## 8. Policy 选择指南

| 场景 | 推荐 Policy |
|------|------------|
| 实时状态显示 | `AlwaysPolicy` |
| 节省资源、只在值变化时更新 | `ValueChangedPolicy` |
| 新界面需要显示当前状态 | `AlwaysPolicy(true)` |

---

## 9. 构建

```bash
# 配置（仅库）
cmake --preset vs2019-x64

# 构建
cmake --build --preset vs2019-x64-release

# 构建并运行测试
cmake --preset vs2019-x64 -DMEDIATOR_BUILD_TESTS=ON
cmake --build --preset vs2019-x64-debug
ctest --test-dir build/vs2019-x64 -C Debug --output-on-failure
```

---

*文档版本: 2.0*
*最后更新: 2026-06-05*

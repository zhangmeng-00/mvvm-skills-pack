﻿---
name: write-viewmodel
description: "Use when implementing a MediatorCore BaseViewModel subclass for UI state, Q_PROPERTY, getters, changed signals, Model-state subscriptions, and converting user events into Publish command tags without calling Model or View."
---

# Write ViewModel

## 职责

编写 `BaseViewModel` 子类，负责 UI 状态、`Q_PROPERTY`、通知信号，以及用户事件到命令 tag 的转换。

## ViewModel 类型

先判断 ViewModel 类型：
- 页面 ViewModel：服务顶层或页面级 View。
- 子组件 ViewModel：服务局部控件或子 View。
- 列表/表格 ViewModel：暴露 model 或列表数据。
- 图表 ViewModel：维护图表点、范围、颜色、显示状态。
- 表单 ViewModel：维护输入值、校验状态、提交命令。

## 输入契约

编写前应明确：
- ViewModel 名称和对应 View。
- UI 状态属性清单。
- 用户事件清单。
- 订阅 tag 清单。
- 发布命令 tag 清单。
- `$design-mediator-message` 输出的 tag/type 契约。
- `$design-mediator-policy` 输出的 Policy 决策。
- 初始值。

## 规则

- 继承 `BaseViewModel`。
- UI 状态使用 `Q_PROPERTY(... READ ... NOTIFY ...)`。
- 更新属性时避免重复 emit。
- `SetupSubscriptions()` 只做两件事：`Subscribe(...)` 和 `RegisterHandlers({...})`。
- `RegisterHandlers({...})` 只保留 `tag -> 私有成员函数` 映射。
- lambda 只做转发，不写业务逻辑、状态转换或分支判断。
- 订阅 Model 状态，在 handler 中更新 ViewModel 状态。
- 用户事件入口只转换为 `Publish()` 命令 tag。
- 不直接调用 Model。
- 不创建或操作 View 控件。
- 不执行设备、数据库或长耗时业务。
- 遵循 `$design-mediator-message` 输出的 tag/type 契约。
- 遵循 `$design-mediator-policy` 输出的 Policy 决策。

## Q_PROPERTY 规则

每个 UI 状态属性：
- 必须有 READ。
- 可变属性必须有 NOTIFY。
- getter 必须是 const。
- update/set 方法必须比较旧值，变化后才 emit。
- 属性类型应适合 UI 消费，例如 `QString`、`bool`、`QColor`、`QVariantList`。
- 不把复杂业务对象直接暴露给 View。

示例：

```cpp
Q_PROPERTY(QString status READ status NOTIFY statusChanged)
```

## 状态更新规则

允许：
- 将 Model 状态转为 UI 可显示状态。
- 保存颜色、文本、启用状态、列表数据等 UI 状态。
- 做轻量格式转换。

禁止：
- 设备操作。
- 数据库操作。
- 长耗时计算。
- 直接调用 Model。
- 直接操作 View 控件。

## 用户事件规则

用户事件入口：
- 将按钮、滑块、输入事件转换为命令 tag。
- 可更新必要的本地 UI 状态。
- 使用 `Publish()` 发命令。
- 不直接调用 Model。

## 订阅 handler 规则

订阅 handler：
- 必须实现为语义清晰的私有成员函数，例如 `handleBusinessStatus(const QVariant& value)`。
- 私有 handler 第一行必须 `LOG_DEBUG`。
- 具体业务、状态转换和属性更新都放在私有 handler 函数内。
- 读取 QVariant 并转换为 UI 状态。
- 调用私有 update/set 方法。
- 不直接操作控件。
- 不把业务逻辑写进 `RegisterHandlers({...})` 的 lambda。

## 推荐输出

- ViewModel 头文件和实现文件。
- `Q_PROPERTY`、getter、signal。
- `SetupSubscriptions()`。
- handler 映射。
- 每个订阅 tag 对应的私有 handler 函数。
- 状态更新方法。
- 用户事件入口实现。

## 推荐模式

```cpp
class MainViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit MainViewModel(QObject* parent = nullptr);
    QString status() const { return m_status; }

signals:
    void statusChanged();

protected:
    void SetupSubscriptions() override;
    void ObserveData(const QString& tag, const QVariant& value) override;
    void onClicked(const QString& senderId) override;

private:
    void setStatus(const QString& status);
    void handleBusinessStatus(const QVariant& value);

private:
    QString m_status = QStringLiteral("idle");
};
```

`SetupSubscriptions()` 只表达订阅和映射：

```cpp
void MainViewModel::SetupSubscriptions()
{
    Subscribe("business/status", QVariant(QStringLiteral("idle")),
              std::make_shared<AlwaysPolicy>(true));

    RegisterHandlers({
        {"business/status", [this](const QVariant& v) { handleBusinessStatus(v); }},
    });
}
```

`ObserveData()` 只入队：

```cpp
void MainViewModel::ObserveData(const QString& tag, const QVariant& value)
{
    Enqueue(tag, value);
}
```

业务逻辑放到私有 handler：

```cpp
void MainViewModel::handleBusinessStatus(const QVariant& value)
{
    LOG_DEBUG("MainViewModel",
              QString("business/status = %1").arg(value.toString()));
    setStatus(value.toString());
}

void MainViewModel::onClicked(const QString& senderId)
{
    if (senderId == "startButton") {
        Publish("business/cmd/start", QVariant(true));
    }
}

void MainViewModel::setStatus(const QString& status)
{
    if (m_status == status) {
        return;
    }
    m_status = status;
    emit statusChanged();
}
```

## 不负责

- 不创建控件。
- 不直接操作 View。
- 不调用 Model。
- 不执行业务或设备操作。

## 自查清单

- [ ] 是否继承 `BaseViewModel`。
- [ ] 属性是否有 READ 和 NOTIFY。
- [ ] setter/update 方法是否避免重复 emit。
- [ ] `SetupSubscriptions()` 是否只包含 `Subscribe(...)` 和 `RegisterHandlers({...})`。
- [ ] `RegisterHandlers()` 是否只做 tag 到私有函数的映射。
- [ ] lambda 是否只转发、不写业务逻辑。
- [ ] 用户事件是否只 Publish 命令 tag。
- [ ] 订阅 handler 是否更新 ViewModel 状态而非 View。
- [ ] 私有 handler 第一行是否 `LOG_DEBUG`。
- [ ] 是否没有直接调用 Model/View。
- [ ] 是否遵循 tag/type/Policy 契约。

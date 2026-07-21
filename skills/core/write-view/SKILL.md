﻿---
name: write-view
description: "Use when implementing a Qt QWidget/View layer that creates controls, layouts, nested child Views, its own ViewModel, Binding::BindProperty bindings, event forwarding, and stylesheet extension points without business logic."
---

# Write View

## 职责

编写 QWidget/View 层，负责控件、布局、子 View、Binding、事件转发和样式表接口。

## View 类型

先判断 View 类型：
- 顶层 View：由 `main.cpp` 创建。
- 页面 View：由父 View 嵌套。
- 子组件 View：服务局部 UI。
- 图表 View：允许 UI-only 的绘图刷新。
- 表单 View：控件输入和校验提示展示。

## 输入契约

编写前应明确：
- View 名称。
- 对应 ViewModel 类型。
- 控件清单。
- 布局结构。
- Binding 清单。
- 用户事件转发清单。
- 子 View 清单。
- 样式表配置需求。

## 规则

- View 在构造流程中创建自己的 ViewModel。
- ViewModel 通过 `AppContext::instance().ConnectObserve(m_viewModel)` 注册。
- View 可创建和嵌套子 View。
- 使用 `Binding::BindProperty()` 做 ViewModel 到 View 的属性绑定。
- 控件事件转发给 ViewModel 公共事件入口。
- 预留 Qt 样式表接口，例如 `setStyleSheetPath()`、`setCustomStyleSheet()`。
- 不写业务逻辑。
- 不直接调用 Model。
- 默认不直接 `Publish()`。
- 不让 Model/ViewModel 加载样式表或设置控件样式。

## 构造流程

推荐顺序：
1. 创建 ViewModel。
2. `setupUI()`。
3. `setupDefaultStyle()`。
4. `setupBindings()`。
5. `setupConnections()`。
6. `AppContext::ConnectObserve(m_viewModel)`。

## Binding 规则

- 使用 `Binding::BindProperty()`。
- 绑定方向是 ViewModel 属性到 View 控件属性。
- 不做 Model 到 View 绑定。
- 复杂 UI 更新可通过 ViewModel signal 连接到 View 的 UI-only 方法。

## 样式表接口规则

View 必须预留：
- `setStyleSheetPath(const QString& path)` 或等价接口。
- `setCustomStyleSheet(const QString& styleSheet)` 或等价接口。
- `setupDefaultStyle()`。

要求：
- 默认样式可作为 fallback。
- 外部或父 View 可覆盖样式。
- 空路径、读取失败、空样式字符串不崩溃。
- Model/ViewModel 不参与样式表加载。

## 推荐模式

```cpp
class MainView : public QWidget {
    Q_OBJECT
public:
    explicit MainView(QWidget* parent = nullptr);
    ~MainView() override;

    void setStyleSheetPath(const QString& path);
    void setCustomStyleSheet(const QString& styleSheet);

private:
    void setupUI();
    void setupDefaultStyle();
    void setupBindings();
    void setupConnections();

private:
    MainViewModel* m_viewModel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QPushButton* m_startButton = nullptr;
};
```

```cpp
MainView::MainView(QWidget* parent)
    : QWidget(parent)
    , m_viewModel(new MainViewModel(this))
{
    setupUI();
    setupDefaultStyle();
    setupBindings();
    setupConnections();

    AppContext::instance().ConnectObserve(m_viewModel);
}
```

## 不负责

- 不写业务逻辑。
- 不直接调用 Model。
- 不创建后台 Model。
- 不设计 Policy。

## 自查清单

- [ ] View 是否创建自己的 ViewModel。
- [ ] ViewModel 是否注册到 AppContext。
- [ ] 构造流程是否清晰。
- [ ] Binding 是否从 ViewModel 到 View。
- [ ] 控件事件是否转发给 ViewModel 公共入口。
- [ ] 是否预留样式表接口。
- [ ] UI-only 逻辑是否没有业务含义。
- [ ] 是否没有业务逻辑和 Model 调用。

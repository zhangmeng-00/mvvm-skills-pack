﻿---
name: write-view-style
description: "Use when writing or improving Qt QWidget/View QSS from a reference image, screenshot, UI style brief, ui-ux-pro-max, or design-taste-frontend output; creates scoped .qss for objectName and widget interaction states."
---

# Write View Style

## 职责

为 Qt QWidget/View 编写或优化 QSS 样式表。只负责视觉样式，不负责 View 结构、ViewModel、Model 或业务逻辑。

## 触发场景

- 用户提供参考图片，希望提取视觉风格并生成 QSS。
- 用户要求优化现有 Qt Widgets UI。
- 用户已有 `.qss`，需要重构、统一控件状态或补充交互态。
- `write-view` 已经预留 `setStyleSheetPath()` / `setCustomStyleSheet()`，需要生成实际样式表。
- 其他 UI/UX skill 给出视觉方向，需要将方向落成 Qt QSS。

## 输入契约

编写前应明确：
- View 类名和主要 `objectName`。
- 控件清单，例如 `QPushButton`、`QLabel`、`QLineEdit`、`QSlider`、`QGroupBox`、`QTableView`。
- 参考图片、截图或风格描述。
- 明暗主题、品牌色、字号、密度和圆角偏好。
- 样式表保存路径，例如 `app/styles/main.qss` 或 `resources/styles/main.qss`。
- 是否只影响某个 View，还是作为全局应用样式。

## QSS 规则

- 优先使用 View 根 `objectName` 限定范围，避免污染全局。
- 对关键控件覆盖 normal、hover、pressed、disabled；可选 checked、focus。
- 不用 QSS 表达业务状态；业务状态应由 ViewModel 暴露属性或 View 的 UI-only 逻辑处理。
- 不通过 Model/ViewModel 加载样式表。
- 不照搬 Web CSS；只使用 Qt QSS 支持的属性和选择器。
- 控件高度、内边距、字体、边框、颜色要统一。
- 样式命名应服务可维护性，建议 View 中为关键控件设置稳定 `objectName`。

## 输出契约

必须输出：
- `.qss` 内容。
- 推荐保存路径。
- 需要设置的控件 `objectName` 清单。
- View 中调用样式接口的示例。
- 覆盖的控件状态说明。
- 如果参考图片中有无法用 QSS 直接实现的效果，说明替代方案。

## 推荐 QSS 结构

```css
/* MainView scope */
#MainView {
    background-color: #f5f7fb;
    color: #20242a;
    font-family: "Microsoft YaHei";
    font-size: 13px;
}

#MainView QPushButton {
    min-height: 30px;
    padding: 4px 14px;
    border: 1px solid #2f6fed;
    border-radius: 4px;
    background-color: #3578f6;
    color: #ffffff;
}

#MainView QPushButton:hover {
    background-color: #4a88f7;
}
```

## View 调用示例

```cpp
auto* view = new MainView();
view->setObjectName("MainView");
view->setStyleSheetPath(":/styles/main.qss");
```

## 不负责

- 不创建 View 控件结构。
- 不创建 ViewModel 或 Model。
- 不写业务逻辑。
- 不用样式表替代状态机或业务状态。
- 不强行复刻 Qt QSS 无法实现的图片效果。

## 自查清单

- [ ] 是否限定作用域，避免全局污染。
- [ ] 是否列出需要设置的 `objectName`。
- [ ] 是否覆盖 hover/pressed/disabled 等状态。
- [ ] 是否避免业务逻辑进入 QSS。
- [ ] 是否说明保存路径和 View 调用方式。
- [ ] 是否区分 QSS 可实现效果和需要代码支持的效果。

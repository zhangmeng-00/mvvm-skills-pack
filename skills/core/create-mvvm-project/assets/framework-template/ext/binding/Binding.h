#pragma once

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QDebug>
#include <QAbstractItemView>
#include <QAbstractItemModel>

#include "BindingHelper.h"

namespace Binding {

/*
 * BindProperty
 * ------------------------------------------------------------
 * View.Q_PROPERTY ← ViewModel.Q_PROPERTY (单向：VM → View)
 *
 * 使用方式：
 *   Binding::BindProperty(ui->label, "text", viewModel, "title");
 */
inline void BindProperty(QObject* view,
                         const char* viewProperty,
                         QObject* viewModel,
                         const char* vmProperty)
{
    if (!view || !viewModel) {
        qWarning() << "[Binding] view or viewModel is null";
        return;
    }

    // 1️⃣ 初始同步
    const QVariant initialValue = viewModel->property(vmProperty);
    if (qobject_cast<QAbstractItemView*>(view)
        && QByteArray(viewProperty) == QByteArray("model")) {
        QObject* obj = initialValue.value<QObject*>();
        auto* model = qobject_cast<QAbstractItemModel*>(obj);
        static_cast<QAbstractItemView*>(view)->setModel(model);
    } else {
        view->setProperty(viewProperty, initialValue);
    }

    // 2️⃣ 查找 ViewModel 的属性
    const QMetaObject* mo = viewModel->metaObject();
    int propIndex = mo->indexOfProperty(vmProperty);
    if (propIndex < 0) {
        qWarning() << "[Binding] VM property not found:" << vmProperty;
        return;
    }

    QMetaProperty prop = mo->property(propIndex);
    if (!prop.hasNotifySignal()) {
        qWarning() << "[Binding] VM property has no NOTIFY:" << vmProperty;
        return;
    }

    // 3️⃣ 获取 NOTIFY signal
    QMetaMethod signalMethod = prop.notifySignal();
    QByteArray signalStr = QByteArray("2") + signalMethod.methodSignature();

    // 4️⃣ 创建 helper（生命周期跟随 view）
    auto* helper = new BindingHelper(
        view,
        viewProperty,
        viewModel,
        vmProperty,
        view
        );

    // 5️⃣ 连接信号（Qt5 反射方式）
    bool ok = QObject::connect(
        viewModel,
        signalStr.constData(),
        helper,
        SLOT(onVmPropertyChanged())
        );

    if (!ok) {
        qWarning() << "[Binding] connect failed:"
                   << vmProperty << "->" << viewProperty;
    }
}

} // namespace Binding

/*
| 控件           | 可绑定的 viewProperty                   |
| ------------ | ----------------------------------- |
| QLabel       | `"text"`, `"enabled"`, `"visible"`  |
| QLineEdit    | `"text"`, `"enabled"`, `"readOnly"` |
| QPushButton  | `"text"`, `"enabled"`, `"visible"` |
| QWidget      | `"enabled"`, `"visible"`            |
| QProgressBar | `"value"`, `"minimum"`, `"maximum"` |
| QSlider      | `"value"`, `"minimum"`, `"maximum"` |
| QSpinBox     | `"value"`                           |
| QCheckBox    | `"checked"`, `"enabled"`            |
| QRadioButton | `"checked"`                         |
*/

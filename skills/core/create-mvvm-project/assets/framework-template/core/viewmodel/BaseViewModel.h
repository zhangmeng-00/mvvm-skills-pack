#pragma once
#include <QObject>
#include <QDateTime>

#include "ActorObserve.h"

/*
 * BaseViewModel
 * ============================================================
 * - ViewModel 也是 Observe（订阅总线）
 * - 但它对 View 的输出是：Qt Property + signals
 */
class BaseViewModel : public ActorObserve {
    Q_OBJECT
public:
    explicit BaseViewModel(QObject* parent = nullptr)
        : ActorObserve(parent)
    {}

    // 公开的 Publish 槽，供 Binding 层调用
    // 通过元对象系统调用时需要是 slot 或 Q_INVOKABLE
    Q_INVOKABLE void Publish(const QString& tag, const QVariant& value) {
        Observe::Publish(tag, value);
    }

    // ========== 按钮事件 ==========
    Q_INVOKABLE virtual void onClicked(const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onPressed(const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onReleased(const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onToggled(bool checked, const QString& senderId = QString()) {}

    // ========== 复选框/单选按钮 ==========
    Q_INVOKABLE virtual void onStateChanged(int state, const QString& senderId = QString()) {}

    // ========== 下拉框 ==========
    Q_INVOKABLE virtual void onCurrentIndexChanged(int index, const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onCurrentTextChanged(const QString& text, const QString& senderId = QString()) {}

    // ========== 数值输入 ==========
    Q_INVOKABLE virtual void onValueChanged(int value, const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onValueChangedDouble(double value, const QString& senderId = QString()) {}

    // ========== 文本输入 ==========
    Q_INVOKABLE virtual void onTextChanged(const QString& text, const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onTextEdited(const QString& text, const QString& senderId = QString()) {}
    Q_INVOKABLE virtual void onReturnPressed(const QString& senderId = QString()) {}

protected:
           // 子类实现 ObserveData(tag, value)
};

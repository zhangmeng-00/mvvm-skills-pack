#pragma once

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QThread>
#include <QDebug>
#include <QAbstractItemView>
#include <QAbstractItemModel>

/*
 * BindingHelper
 * ============================================================
 * ViewModel.Q_PROPERTY  →  View.Q_PROPERTY
 *
 * 特点：
 * - 不关心控件类型
 * - 线程安全（强制 UI 线程）
 * - 生命周期绑定到 view
 */
class BindingHelper : public QObject {
    Q_OBJECT
public:
    BindingHelper(QObject* view,
                  const char* viewProperty,
                  QObject* viewModel,
                  const char* vmProperty,
                  QObject* parent = nullptr)
        : QObject(parent)
        , m_view(view)
        , m_viewProperty(viewProperty)
        , m_viewModel(viewModel)
        , m_vmProperty(vmProperty)
    {}

public slots:
    void onVmPropertyChanged()
    {
        if (!m_view || !m_viewModel)
            return;

        // 🔒 保证 UI 线程执行
        if (QThread::currentThread() != m_view->thread()) {
            QMetaObject::invokeMethod(
                this,
                "onVmPropertyChanged",
                Qt::QueuedConnection
                );
            return;
        }

        QVariant value = m_viewModel->property(m_vmProperty);
        if (qobject_cast<QAbstractItemView*>(m_view)
            && QByteArray(m_viewProperty) == QByteArray("model")) {
            QObject* obj = value.value<QObject*>();
            auto* model = qobject_cast<QAbstractItemModel*>(obj);
            static_cast<QAbstractItemView*>(m_view)->setModel(model);
        } else {
            m_view->setProperty(m_viewProperty, value);
        }
    }

private:
    QObject*   m_view;
    const char* m_viewProperty;
    QObject*   m_viewModel;
    const char* m_vmProperty;
};

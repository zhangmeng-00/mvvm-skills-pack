#pragma once

#include <QObject>
#include <QThread>
#include <QVector>
#include <QSet>
#include <QDebug>
#include <QMetaObject>
#include <QMutex>

#include <functional>
#include <memory>
#include <type_traits>

#include "Mediator.h"
#include "Observe.h"


class AppContext : public QObject
{
    Q_OBJECT
public:
    static AppContext& instance();

    explicit AppContext(QObject* parent = nullptr);
    ~AppContext();

    Mediator* mediator() const;

    // --------- 线程安全：所有 mediator 调用都走这里 ----------
    void ConnectObserve(Observe* obs);

    // 线程安全 wrapper：所有 mediator 调用都走这里
    template<typename Fn>
    void InvokeOnMediator(Fn&& fn)
    {
        if (!m_mediator) return;

        // Qt5.15 支持 functor invoke
        QMetaObject::invokeMethod(
            m_mediator,
            [f = std::forward<Fn>(fn)]() mutable { f(); },
            Qt::QueuedConnection
            );
    }

    // --------- 组件容器：统一创建/托管 QObject（Model/Actor/Service/Driver…） ----------
    template<typename T, typename... Args>
    typename std::enable_if<std::is_base_of<QObject, T>::value, T*>::type
    AddComponent(Args&&... args)
    {
        // 统一把 parent 设为 AppContext，这样生命周期由 AppContext 托管
        T* obj = new T(std::forward<Args>(args)...);
        if (obj->thread() == thread()) {
            obj->setParent(this);
        } else {
            qWarning() << "[AppContext] Skip setParent due to cross-thread component:" << obj;
        }
        m_components.push_back(obj);

        // 如果它也是 Observe，就自动 ConnectObserve（线程安全）
        if (auto obs = dynamic_cast<Observe*>(obj)) {
            ConnectObserve(obs);
        }
        return obj;
    }


private:
    void shutdownMediatorThread();

private:
    Mediator*  m_mediator = nullptr;
    QThread*   m_mediatorThread = nullptr;

    QVector<QObject*> m_components;
    QSet<Observe*> m_connectedObservers;
    QMutex m_observerMutex;

};

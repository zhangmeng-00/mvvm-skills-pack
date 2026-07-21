#include "Topic.h"
#include "Observe.h"

#include <QDebug>
#include <QMutexLocker>
#include <algorithm>

Topic::Topic(const QString& tag, QObject* parent)
    : QObject(parent)
    , m_tag(tag)
{
}

bool Topic::containsLocked(Observe* observer) const
{
    for (const auto& s : m_subscribers) {
        if (s.observer == observer) return true;
    }
    return false;
}

bool Topic::HasSubscriber() const
{
    QMutexLocker locker(&m_mutex);
    return !m_subscribers.empty();
}

void Topic::cleanupDeadSubscribersLocked()
{
    m_subscribers.erase(
        std::remove_if(m_subscribers.begin(), m_subscribers.end(),
                       [](const SubscriberItem& s) { return s.observer.isNull(); }),
        m_subscribers.end()
        );
}

void Topic::onObserverDestroyed(QObject* /*obj*/)
{
    // 只清理已失效的 QPointer 项（最稳，不依赖析构对象地址）
    QMutexLocker lk(&m_mutex);
    cleanupDeadSubscribersLocked();

    qDebug() << "[Topic] Cleanup dead subscribers tag =" << m_tag
             << "count =" << (int)m_subscribers.size();
}

void Topic::AddSubscriber(Observe* observer, PolicyPtr policy)
{
    if (!observer) return;

    {
        QMutexLocker lk(&m_mutex);
        cleanupDeadSubscribersLocked();

        // 去重：防止重复订阅导致 count 虚高
        if (!containsLocked(observer)) {
            SubscriberItem item;
            item.observer = observer;
            item.policy   = policy;
            m_subscribers.push_back(item);

            qDebug() << "[Topic] AddSubscriber tag =" << m_tag
                     << "observer =" << observer
                     << "count =" << (int)m_subscribers.size();
        } else {
            // 已订阅：更新 policy（可选）
            for (auto& s : m_subscribers) {
                if (s.observer == observer) {
                    s.policy = policy;
                    break;
                }
            }
        }
    }

    // ✅ 用 slot + UniqueConnection，避免重复 connect（lambda 无法 Unique）
    QObject::connect(observer, &QObject::destroyed,
                     this, &Topic::onObserverDestroyed,
                     Qt::UniqueConnection);

    // ✅ Replay lastValue（如果你需要新订阅者立刻拿到当前状态，可以打开）
    // 注意：Replay 不应被 ValueChangedPolicy 过滤
    /*
    Observe* obs = observer;
    QVariant last;
    bool hasLast = false;
    {
        QMutexLocker lk(&m_mutex);
        hasLast = m_hasLastValue;
        last = m_lastValue;
    }
    if (hasLast && obs) {
        qDebug() << "[Topic] Replay last value tag =" << m_tag << "value =" << last;
        obs->OnDataReceived(m_tag, last);
    }
    */
}

void Topic::RemoveSubscriber(Observe* observer)
{
    if (!observer) return;

    QMutexLocker lk(&m_mutex);
    cleanupDeadSubscribersLocked();

    auto before = m_subscribers.size();
    m_subscribers.erase(
        std::remove_if(m_subscribers.begin(), m_subscribers.end(),
                       [&](const SubscriberItem& s) { return s.observer == observer; }),
        m_subscribers.end()
        );

    if (m_subscribers.size() != before) {
        qDebug() << "[Topic] RemoveSubscriber tag =" << m_tag
                 << "observer =" << observer
                 << "count =" << (int)m_subscribers.size();
    }
}

PolicyPtr Topic::GetSubscriberPolicy(Observe* observer) const
{
    QMutexLocker lk(&m_mutex);
    for (const auto& s : m_subscribers) {
        if (s.observer == observer) {
            return s.policy;
        }
    }
    return nullptr;
}

bool Topic::HasStickySubscriber() const
{
    QMutexLocker lk(&m_mutex);
    for (const auto& s : m_subscribers) {
        if (s.policy && s.policy->ShouldReplayLastValue()) {
            return true;
        }
    }
    return false;
}

void Topic::Notify(const QVariant& value)
{
    // 快照：避免回调里再 subscribe/unsubscribe 导致迭代器问题
    std::vector<SubscriberItem> snapshot;
    QVariant oldValue;
    QVariant newValue = value;
    bool hadLast = false;

    {
        QMutexLocker lk(&m_mutex);
        cleanupDeadSubscribersLocked();

        qDebug() << "[Topic] Notify tag =" << m_tag
                 << "subscriber count =" << (int)m_subscribers.size();

        snapshot = m_subscribers;
        oldValue = m_lastValue;
        hadLast  = m_hasLastValue;

        // 更新状态
        m_lastValue = newValue;
        m_hasLastValue = true;
    }

    // 解锁后回调订阅者（避免死锁&并发修改）
    for (const auto& s : snapshot) {
        Observe* obs = s.observer.data();
        if (!obs) continue;

        bool should = true;

        // policy 只影响未来通知；首次发布时 hadLast=false，这里默认不过滤
        if (s.policy && hadLast) {
            should = s.policy->ShouldExecute(oldValue, newValue);
        }
        // 用invokeMethod投递到Observe自己的线程
        if (should) {
            const QString tag = m_tag;
            const QVariant v = newValue;

            QMetaObject::invokeMethod(
                obs,
                [obs, tag, v](){ obs->OnDataReceived(tag, v); },
                Qt::QueuedConnection
                );
        }

    }
}

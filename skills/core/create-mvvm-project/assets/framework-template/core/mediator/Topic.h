#pragma once

#include <QObject>
#include <QVariant>
#include <QString>
#include <QPointer>
#include <QMutex>
#include <vector>
#include <memory>

#include "ISubscriptionPolicy.h"

class Observe;

/*
 * Topic
 * ============================================================
 * - 一个 tag 对应一个 Topic
 * - 保存最近一次发布的数据（State）
 * - 支持退订 + observer 析构自动清理（避免悬空指针）
 */
class Topic : public QObject {
    Q_OBJECT
public:
    explicit Topic(const QString& tag, QObject* parent = nullptr);

    void AddSubscriber(Observe* observer, PolicyPtr policy);
    void RemoveSubscriber(Observe* observer);
    PolicyPtr GetSubscriberPolicy(Observe* observer) const;
    bool HasStickySubscriber() const;

    // 你原先是 Notify(tag,value)，tag 实际没用到，这里保持简化版
    void Notify(const QVariant& value);
    bool HasSubscriber() const;

private:
    struct SubscriberItem {
        QPointer<Observe> observer;   // QObject 销毁自动变 nullptr
        PolicyPtr policy;
    };

    void cleanupDeadSubscribersLocked();
    bool containsLocked(Observe* observer) const;

private slots:
    // observer 析构后：只清理 QPointer 变空的订阅项（不依赖已析构对象的裸指针）
    void onObserverDestroyed(QObject* obj);

private:
    QString m_tag;

    mutable QMutex m_mutex;

    QVariant m_lastValue;
    bool     m_hasLastValue{false};

    std::vector<SubscriberItem> m_subscribers;
};

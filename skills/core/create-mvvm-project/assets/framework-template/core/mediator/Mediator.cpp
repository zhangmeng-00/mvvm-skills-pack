#include "Mediator.h"
#include <QDebug>
#include <QMetaType>
#include <QMutexLocker>

/*
 * 构造函数
 */
Mediator::Mediator(QObject* parent)
    : QObject(parent)
{
    /*
     * Qt5 queued connection 传递 PolicyPtr（std::shared_ptr）必须注册元类型
     * 放在这里注册，确保 Mediator 初始化后一定可用
     */
    qRegisterMetaType<PolicyPtr>("PolicyPtr");
}

/*
 * ConnectObserve
 */
void Mediator::ConnectObserve(Observe* obs)
{
    if (!obs) return;

    qDebug() << "[Mediator] ConnectObserve:" << obs;

    /*
     * Use QueuedConnection universally:
     * - Observe may be on UI thread
     * - Mediator may be on separate thread
     */

    const auto queuedUnique = static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection);

    connect(obs, SIGNAL(RequestSubscribe(Observe*,QString,QVariant,PolicyPtr)),
            this, SLOT(OnSubscribe(Observe*,QString,QVariant,PolicyPtr)),
            queuedUnique);

    connect(obs, SIGNAL(RequestPublish(QString,QVariant)),
            this, SLOT(OnPublish(QString,QVariant)),
            queuedUnique);

    // OnUnsubscribe has two overloads (2-arg and 3-arg).
    // Use SIGNAL/SLOT macros to avoid overload resolution ambiguity.
    connect(obs, SIGNAL(RequestUnsubscribe(Observe*,QString)),
            this, SLOT(OnUnsubscribe(Observe*,QString)),
            queuedUnique);

    connect(obs, SIGNAL(RequestUnsubscribe(Observe*,QString,QVariant)),
            this, SLOT(OnUnsubscribe(Observe*,QString,QVariant)),
            queuedUnique);
}

/*
 * OnSubscribe - unified handler (with value)
 */
void Mediator::OnSubscribe(Observe* observer,
                           const QString& tag,
                           const QVariant& value,
                           PolicyPtr policy)
{
    if (!observer || !policy) {
        qWarning() << "[Mediator] OnSubscribe: null observer or policy";
        return;
    }

    const QString typeName = value.typeName();
    auto topic = getOrCreateTopic(typeName, tag);
    topic->AddSubscriber(observer, policy);

    qDebug() << "[Mediator] Subscribed:" << observer
             << "tag:" << tag
             << "type:" << typeName
             << "policy:" << (policy->ShouldReplayLastValue() ? "Always(sticky)" : "Always");
}

/*
 * OnUnsubscribe - without type info (2-arg)
 */
void Mediator::OnUnsubscribe(Observe* obs, const QString& tag)
{
    if (!obs) return;

    QMutexLocker locker(&m_mutex);
    for (auto& typeMap : m_topics) {
        auto it = typeMap.find(tag);
        if (it != typeMap.end()) {
            it.value()->RemoveSubscriber(obs);
        }
    }
}

/*
 * OnUnsubscribe - with type info (3-arg)
 * Precise unsubscription by typeName + tag matching
 */
void Mediator::OnUnsubscribe(Observe* obs, const QString& tag, const QVariant& value)
{
    if (!obs) return;

    const QString typeName = value.typeName();
    auto topic = findTopic(typeName, tag);
    if (topic) {
        topic->RemoveSubscriber(obs);
    }
}

/*
 * OnPublish
 */
void Mediator::OnPublish(const QString& tag,
                         const QVariant& value)
{
    const QString typeName = value.typeName();

    // Check for sticky subscriber first
    auto topic = findTopic(typeName, tag);
    if (!topic || !topic->HasSubscriber()) {
        // No subscribers, cache the value as state
        QMutexLocker locker(&m_mutex);
        m_stateCache[typeName][tag] = value;
        return;
    }

    topic->Notify(value);
}

std::shared_ptr<Topic> Mediator::findTopic(const QString& typeName,
                                            const QString& tag)
{
    QMutexLocker locker(&m_mutex);
    auto typeIt = m_topics.find(typeName);
    if (typeIt == m_topics.end()) {
        return nullptr;
    }
    auto tagIt = typeIt->find(tag);
    if (tagIt == typeIt->end()) {
        return nullptr;
    }
    return tagIt.value();
}

std::shared_ptr<Topic> Mediator::getOrCreateTopic(const QString& typeName,
                                                   const QString& tag)
{
    QMutexLocker locker(&m_mutex);
    auto& entry = m_topics[typeName][tag];
    if (!entry) {
        entry = std::make_shared<Topic>(tag);
    }
    return entry;
}

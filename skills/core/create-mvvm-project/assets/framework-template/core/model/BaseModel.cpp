#include "BaseModel.h"

#include <QMetaObject>
#include <QDebug>

BaseModel::BaseModel(QObject* parent, bool useSeparateThread)
    : ActorObserve(parent, useSeparateThread)
{
}

void BaseModel::RegisterHandler(const QString& tag, HandlerFunc handler)
{
    m_handlers.insert(tag, handler);
}

void BaseModel::RegisterHandlers(
    std::initializer_list<QPair<QString, HandlerFunc>> handlers)
{
    for (const auto& pair : handlers) {
        m_handlers.insert(pair.first, pair.second);
    }
}

void BaseModel::Enqueue(const QString& tag, const QVariant& value)
{
    {
        QMutexLocker lock(&m_queueMutex);
        m_queue.append({tag, value});
    }

    // 如果 drain 还未排期，通过 QueuedConnection 调度一次
    // 使用 atomic flag 避免竞态
    if (!m_drainScheduled) {
        m_drainScheduled = true;
        QMetaObject::invokeMethod(
            this,
            [this]() {
                m_drainScheduled = false;
                Drain();
            },
            Qt::QueuedConnection);
    }
}

void BaseModel::Drain()
{
    QList<QPair<QString, QVariant>> batch;

    {
        QMutexLocker lock(&m_queueMutex);
        batch.swap(m_queue);
    }

    for (const auto& item : batch) {
        auto it = m_handlers.find(item.first);
        if (it != m_handlers.end()) {
            it.value()(item.second);
        }
    }
}

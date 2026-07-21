#include "ActorObserve.h"

#include <QDebug>
#include <QMetaObject>
#include <QThread>

ActorObserve::ActorObserve(QObject* parent, bool useSeparateThread)
    : Observe(nullptr)
{
    qDebug() << "ActorObserve ctor thread:" << QThread::currentThreadId()
             << "useSeparateThread:" << useSeparateThread;

    if (useSeparateThread) {
        m_thread = new QThread(nullptr);
        moveToThread(m_thread);

        connect(m_thread, &QThread::started, [this]() {
            qDebug() << "ActorObserve worker thread started:" << QThread::currentThreadId();
            qDebug() << "ActorObserve object thread:" << thread()->currentThreadId();
        });

        m_thread->start();
    } else {
        setParent(parent);
    }
}

ActorObserve::~ActorObserve()
{
    if (!m_thread) {
        return;
    }

    if (m_thread->isRunning()) {
        m_thread->quit();
        m_thread->wait();
    }

    if (m_thread->thread() == QThread::currentThread()) {
        delete m_thread;
    } else {
        m_thread->deleteLater();
    }
    m_thread = nullptr;
}

void ActorObserve::handleData(const QString& tag, const QVariant& value)
{
    if (QThread::currentThread() == thread()) {
        ObserveData(tag, value);
        return;
    }

    QMetaObject::invokeMethod(
        this,
        [this, tag, value]() {
            ObserveData(tag, value);
        },
        Qt::QueuedConnection);
}

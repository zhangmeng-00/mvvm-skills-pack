#include "Observe.h"
#include <QDebug>

Observe::Observe(QObject* parent)
    : QObject(parent)
{
}

Observe::~Observe() = default;

void Observe::Subscribe(const QString& tag, const QVariant& value, PolicyPtr policy)
{
    qDebug() << "[Observe] Subscribe request with value:" << this << tag
             << "type:" << value.typeName();

    // 带value的订阅，用于类型索引
    emit RequestSubscribe(this, tag, value, policy);
}

void Observe::Unsubscribe(const QString& tag)
{
    qDebug() << "[Observe] Unsubscribe request:" << this << tag;
    emit RequestUnsubscribe(this, tag);
}

void Observe::Unsubscribe(const QString& tag, const QVariant& value)
{
    qDebug() << "[Observe] Unsubscribe request with value:" << this << tag
             << "type:" << value.typeName();
    emit RequestUnsubscribe(this, tag, value);
}

void Observe::Publish(const QString& tag, const QVariant& value)
{
    emit RequestPublish(tag, value);
}

void Observe::OnDataReceived(const QString& tag,
                             const QVariant& value)
{
    // 默认实现：直接转给子类业务入口
    handleData(tag, value);
}

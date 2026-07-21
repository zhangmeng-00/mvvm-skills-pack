#pragma once
#include <QVariant>
#include <memory>

/*
 * ISubscriptionPolicy
 * ------------------------------------------------------------
 * 订阅策略接口：决定是否触发订阅者
 */
class ISubscriptionPolicy {
public:
    explicit ISubscriptionPolicy(bool replayLastValue = false)
        : m_replayLastValue(replayLastValue) {}

    virtual ~ISubscriptionPolicy() = default;

    virtual bool ShouldExecute(const QVariant& oldValue,
                               const QVariant& newValue) = 0;

    // 是否需要重发上一次值给新订阅者（Sticky）
    virtual bool ShouldReplayLastValue() const { return m_replayLastValue; }

protected:
    bool m_replayLastValue{false};
};

/*
 * PolicyPtr
 * ------------------------------------------------------------
 * 统一策略指针类型，减少模板噪声
 */
using PolicyPtr = std::shared_ptr<ISubscriptionPolicy>;

/*
 * Qt 元对象系统声明
 * ------------------------------------------------------------
 * 让 PolicyPtr 可以通过 Qt::QueuedConnection 作为参数传递
 */
Q_DECLARE_METATYPE(PolicyPtr)

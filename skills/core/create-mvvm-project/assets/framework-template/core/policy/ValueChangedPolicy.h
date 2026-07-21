#pragma once
#include "ISubscriptionPolicy.h"

/*
 * ValueChangedPolicy
 * ============================================================
 * 只有值变化时才通知
 */
class ValueChangedPolicy : public ISubscriptionPolicy {
public:
    explicit ValueChangedPolicy(bool replayLastValue = false)
        : ISubscriptionPolicy(replayLastValue) {}

    bool ShouldExecute(const QVariant& oldValue,
                       const QVariant& newValue) override {
        return oldValue != newValue;
    }
};

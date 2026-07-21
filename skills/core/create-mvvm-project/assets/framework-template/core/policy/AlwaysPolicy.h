#pragma once
#include "ISubscriptionPolicy.h"

/*
 * AlwaysPolicy
 * ============================================================
 * 每次发布都通知
 */
class AlwaysPolicy : public ISubscriptionPolicy {
public:
    explicit AlwaysPolicy(bool replayLastValue = false)
        : ISubscriptionPolicy(replayLastValue) {}

    bool ShouldExecute(const QVariant&, const QVariant&) override {
        return true;
    }
};

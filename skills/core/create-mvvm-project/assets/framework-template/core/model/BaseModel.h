#pragma once

#include "ActorObserve.h"
#include "IModel.h"

#include <QHash>
#include <QMutex>
#include <QList>
#include <QPair>
#include <functional>
#include <initializer_list>

/*
 * BaseModel
 * ============================================================
 * 所有业务 Model 的基类
 *
 * 特点：
 * ------------------------------------------------------------
 * - 已经具备 Actor（串行、线程安全）
 * - Enqueue + Drain：入队后批量分发到 handler
 * - RegisterHandler(s)：tag → handler 映射
 * - 实现 IModel 接口，支持依赖注入
 */
class BaseModel : public ActorObserve, public IModel {
    Q_OBJECT
public:
    using HandlerFunc = std::function<void(const QVariant&)>;

    explicit BaseModel(QObject* parent = nullptr,
                      bool useSeparateThread = false);

    // ===== IModel 接口实现 =====

    QString modelName() const override {
        return metaObject()->className();
    }

    void initialize() override {
        SetupSubscriptions();
    }

    void shutdown() override {
    }

    QString debugInfo() const override {
        return QString("BaseModel: %1").arg(modelName());
    }

protected:
    /*
     * ObserveData — write-model 规则 2
     * --------------------------------------------------------
     * 只有一行：Enqueue(tag, value)
     */
    void ObserveData(const QString& tag,
                     const QVariant& value) override
    {
        Enqueue(tag, value);
    }

    /*
     * RegisterHandler — 单个 handler 注册
     * --------------------------------------------------------
     * 用于运行时动态 tag
     */
    void RegisterHandler(const QString& tag, HandlerFunc handler);

    /*
     * RegisterHandlers — 批量 handler 注册（推荐）
     * --------------------------------------------------------
     * 接收初始化列表，如：
     *   RegisterHandlers({
     *       {"cmd/start", [this](const QVariant& v) { ... }},
     *       {"cmd/stop",  [this](const QVariant& v) { ... }},
     *   });
     */
    void RegisterHandlers(std::initializer_list<
        QPair<QString, HandlerFunc>> handlers);

private:
    /*
     * Enqueue — 入队 + 调度 drain
     * --------------------------------------------------------
     * 将 (tag, value) 存入队列，通过 QueuedConnection 调度 Drain
     */
    void Enqueue(const QString& tag, const QVariant& value);

    /*
     * Drain — 清空队列 + 分发到 handler
     * --------------------------------------------------------
     * 取走全部队内元素，按 tag 查找 handler 并调用
     */
    void Drain();

private:
    QMutex m_queueMutex;
    QList<QPair<QString, QVariant>> m_queue;
    bool m_drainScheduled = false;

    QHash<QString, HandlerFunc> m_handlers;
};

#pragma once

#include <QObject>
#include <QVariant>
#include <memory>
#include <QString>

#include "ISubscriptionPolicy.h"

/*
 * Observe
 * ============================================================
 * 所有 Model / ViewModel 的基类
 *
 * 职责：
 * - 向 Mediator 订阅 Topic（tag + policy）
 * - 向 Mediator 发布数据（tag + value）
 * - 接收 Mediator 分发的数据（tag + value）
 */
class Observe : public QObject {
    Q_OBJECT
public:
    explicit Observe(QObject* parent = nullptr);
    virtual ~Observe();

    /*
     * Subscribe
     * --------------------------------------------------------
     * 向 Mediator 请求订阅：
     * - tag：例如 "user/score"
     * - policy：订阅策略（Always / ValueChanged 等）
     *
     * ✅ 关键点：policy 直接作为信号参数发送，避免异步串台
     */

    // 带value参数的订阅（用于类型索引）
    void Subscribe(const QString& tag, const QVariant& value, PolicyPtr policy);

    void Unsubscribe(const QString& tag);

    /*
     * Unsubscribe (带类型)
     * --------------------------------------------------------
     * 精确退订指定 typeName + tag 的订阅。
     * value 用于传递 QVariant 类型信息，与 Subscribe 时的类型对应。
     */
    void Unsubscribe(const QString& tag, const QVariant& value);
    //增加SetupSubscriptions,所有的Model统一在模块内部订阅，只需要重写SetupSubscriptions就可以了---zm
    virtual void SetupSubscriptions() {}

    /*
     * Publish
     * --------------------------------------------------------
     * 向 Mediator 发布数据（tag + value）
     */
    void Publish(const QString& tag, const QVariant& value);

signals:
    /*
     * RequestSubscribe
     * --------------------------------------------------------
     * 统一订阅信号（带 policy）
     * 使用 QueuedConnection 时，policy 仍然正确对应本次订阅
     */

    // 带value的订阅信号
    void RequestSubscribe(Observe* observer,
                          const QString& tag,
                          const QVariant& value,
                          PolicyPtr policy);

     void RequestUnsubscribe(Observe* self, QString tag);

    /*
     * RequestUnsubscribe (带类型)
     * --------------------------------------------------------
     * 携带 QVariant 类型信息，Mediator 可精确匹配 typeName + tag
     */
    void RequestUnsubscribe(Observe* self, QString tag, QVariant value);

    /*
     * RequestPublish
     * --------------------------------------------------------
     * 统一发布信号
     */
    void RequestPublish(const QString& tag,
                        const QVariant& value);


public slots:
    /*
     * OnDataReceived
     * --------------------------------------------------------
     * Mediator/Topic -> Observe 的回调入口
     * 必须是 virtual，供 ActorObserve override
     */
    virtual void OnDataReceived(const QString& tag,
                                const QVariant& value);

protected:
    /*
     * ObserveData
     * --------------------------------------------------------
     * 统一的业务处理入口（子类实现）。
     *
     * ActorObserve 会在线程检查后调用此方法，
     * 直接继承 Observe 的子类则由 handleData 默认转发至此。
     */
    virtual void ObserveData(const QString& tag,
                             const QVariant& value) {}

    /*
     * handleData
     * --------------------------------------------------------
     * OnDataReceived 的内部转发入口。
     * 默认实现调用 ObserveData；ActorObserve 重写此方法
     * 以实现跨线程投递。
     */
    virtual void handleData(const QString& tag,
                             const QVariant& value)
    {
        ObserveData(tag, value);
    }
};

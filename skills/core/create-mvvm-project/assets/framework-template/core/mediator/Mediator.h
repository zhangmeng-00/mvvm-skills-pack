#pragma once

#include <QObject>
#include <QMap>
#include <QMutex>
#include <memory>
#include <QVariant>

#include "Topic.h"
#include "Observe.h"

/*
 * Mediator
 * ============================================================
 * 负责：
 * - 管理 Topic（按 tag 创建/获取）
 * - 处理订阅：把 (observer + policy) 加入 Topic
 * - 处理发布：转发到 Topic->Notify()
 */
class Mediator : public QObject {
    Q_OBJECT
public:
    explicit Mediator(QObject* parent = nullptr);

    /*
     * ConnectObserve
     * --------------------------------------------------------
     * 让某个 Observe 接入 Mediator：
     * - 订阅请求信号
     * - 发布请求信号
     */
    void ConnectObserve(Observe* obs);

private slots:
    /*
     * OnSubscribe
     * --------------------------------------------------------
     * 处理订阅请求（来自任意线程）
     * ✅ policy 直接由信号传入，杜绝异步串台
     */

    // 带value参数的订阅处理（用于类型索引）
    void OnSubscribe(Observe* observer,
                     const QString& tag,
                     const QVariant& value,
                     PolicyPtr policy);

    void OnUnsubscribe(Observe* obs, const QString& tag);

    /*
     * OnUnsubscribe (带类型)
     * --------------------------------------------------------
     * 带 QVariant 类型信息的精确退订，按 typeName + tag 匹配
     */
    void OnUnsubscribe(Observe* obs, const QString& tag, const QVariant& value);

    /*
     * OnPublish
     * --------------------------------------------------------
     * 处理发布请求
     */
    void OnPublish(const QString& tag,
                   const QVariant& value);

private:
    /*
     * findTopic
     * --------------------------------------------------------
     * 查找Topic（不创建）
     */
    std::shared_ptr<Topic> findTopic(const QString& typeName,
                                     const QString& tag);

    /*
     * getOrCreateTopic
     * --------------------------------------------------------
     * 获取或创建 Topic（按typeName + tag索引）
     */
    std::shared_ptr<Topic> getOrCreateTopic(const QString& typeName,
                                             const QString& tag);

private:
    QMutex m_mutex; // 保护 topics
    // 两层Map：第一层typeName -> 第二层tag -> Topic
    QMap<QString, QMap<QString, std::shared_ptr<Topic>>> m_topics;
    // 两层Map：第一层typeName -> 第二层tag -> cached value
    QMap<QString, QMap<QString, QVariant>> m_stateCache;
};

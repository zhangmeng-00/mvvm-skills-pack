#pragma once

#include "Observe.h"
#include <QString>
#include <QVariant>

class QThread;

/*
 * ActorObserve
 * ============================================================
 * Qt5 Actor / Mailbox 风格的 Observe
 *
 * 职责：
 * - 拦截 OnDataReceived（Topic → Observe 的入口）
 * - 把消息投递到对象所属线程
 * - 确保 ObserveData 串行执行
 */
class ActorObserve : public Observe {
    Q_OBJECT
public:
    explicit ActorObserve(QObject* parent = nullptr,
                          bool useSeparateThread = false);
    ~ActorObserve() override;

protected:
    /*
     * Topic → ActorObserve 的入口
     * 可能运行在 Mediator 线程
     * 重写 handleData 以实现跨线程投递到 ObserveData
     */
    void handleData(const QString& tag,
                    const QVariant& value) override;

private:
    // 单独线程（如果使用）
    QThread* m_thread = nullptr;
};

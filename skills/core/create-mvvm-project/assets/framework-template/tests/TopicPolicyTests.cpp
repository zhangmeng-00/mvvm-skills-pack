#include <QtTest/QtTest>

#include "AlwaysPolicy.h"
#include "Mediator.h"
#include "Observe.h"
#include "ValueChangedPolicy.h"

#include <memory>

class RecordingObserver : public Observe {
    Q_OBJECT

public:
    explicit RecordingObserver(QObject* parent = nullptr)
        : Observe(parent)
    {
    }

    QList<QPair<QString, QVariant>> received;

signals:
    void dataRecorded();

protected:
    /*
     * 记录通过 Mediator 分发到观察者的数据。
     *
     * 参数说明：
     *   - tag：主题标签
     *   - value：主题值
     *
     * 副作用：追加到 received，并发出 dataRecorded 便于测试等待异步投递。
     */
    void handleData(const QString& tag, const QVariant& value) override
    {
        received.append(qMakePair(tag, value));
        emit dataRecorded();
    }
};

class TopicPolicyTests : public QObject {
    Q_OBJECT

private slots:
    /*
     * sticky 订阅者应能收到订阅前同类型同 tag 的最后一次发布值。
     *
     * 约束：该测试锁定状态型 Topic 的 replay 语义，避免 UI 晚订阅时丢失当前状态。
     */
    void stickySubscriberReplaysValuePublishedBeforeSubscription()
    {
        Mediator mediator;
        RecordingObserver publisher;
        mediator.ConnectObserve(&publisher);

        const QString tag = QStringLiteral("sensor/temperature");
        publisher.Publish(tag, QVariant(42));
        QCoreApplication::processEvents();

        RecordingObserver subscriber;
        mediator.ConnectObserve(&subscriber);
        subscriber.Subscribe(tag, QVariant(0), std::make_shared<AlwaysPolicy>(true));

        QTRY_COMPARE(subscriber.received.size(), 1);
        QCOMPARE(subscriber.received.first().first, tag);
        QCOMPARE(subscriber.received.first().second.toInt(), 42);
    }

    /*
     * 退订按 tag 表达，必须清理该 tag 下所有 QVariant 类型索引的订阅项。
     *
     * 约束：当前 RequestUnsubscribe 不携带类型信息，因此不能只删除第一个 typeName。
     */
    void unsubscribeRemovesAllTypeIndexedSubscriptionsForTag()
    {
        Mediator mediator;
        RecordingObserver subscriber;
        RecordingObserver publisher;
        mediator.ConnectObserve(&subscriber);
        mediator.ConnectObserve(&publisher);

        const QString tag = QStringLiteral("device/state");
        subscriber.Subscribe(tag, QVariant(0), std::make_shared<AlwaysPolicy>());
        subscriber.Subscribe(tag, QVariant(0.0), std::make_shared<AlwaysPolicy>());
        QCoreApplication::processEvents();

        subscriber.Unsubscribe(tag);
        QCoreApplication::processEvents();
        subscriber.received.clear();

        publisher.Publish(tag, QVariant(1));
        publisher.Publish(tag, QVariant(1.5));
        QTest::qWait(50);

        QCOMPARE(subscriber.received.size(), 0);
    }

    /*
     * ValueChangedPolicy 首次发布应通知，重复值不通知，变化值再通知。
     *
     * 约束：该策略用于降低重复状态刷新，首次状态仍必须能初始化订阅者。
     */
    void valueChangedPolicySkipsDuplicateValuesAfterInitialNotification()
    {
        Mediator mediator;
        RecordingObserver subscriber;
        RecordingObserver publisher;
        mediator.ConnectObserve(&subscriber);
        mediator.ConnectObserve(&publisher);

        const QString tag = QStringLiteral("sensor/pressure");
        subscriber.Subscribe(tag, QVariant(0), std::make_shared<ValueChangedPolicy>());
        QCoreApplication::processEvents();

        publisher.Publish(tag, QVariant(10));
        QTRY_COMPARE(subscriber.received.size(), 1);
        QCOMPARE(subscriber.received.last().second.toInt(), 10);

        publisher.Publish(tag, QVariant(10));
        QTest::qWait(50);
        QCOMPARE(subscriber.received.size(), 1);

        publisher.Publish(tag, QVariant(11));
        QTRY_COMPARE(subscriber.received.size(), 2);
        QCOMPARE(subscriber.received.last().second.toInt(), 11);
    }
};

QTEST_MAIN(TopicPolicyTests)

#include "TopicPolicyTests.moc"

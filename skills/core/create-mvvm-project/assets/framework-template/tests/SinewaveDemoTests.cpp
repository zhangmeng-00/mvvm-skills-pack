#include <QtTest/QtTest>

#include "SineModel.h"

class TestableSineModel : public SineModel {
    Q_OBJECT

public:
    explicit TestableSineModel(QObject* parent = nullptr, bool useSeparateThread = false)
        : SineModel(parent, useSeparateThread)
    {
    }

    /*
     * 模拟 Mediator 已经完成订阅握手后的模型初始化。
     *
     * 副作用：触发 SineModel 注册命令 handler 与内部定时器。
     */
    void setupForTest()
    {
        SetupSubscriptions();
    }

    /*
     * 直接投递订阅数据，避免测试依赖完整 AppContext/Mediator 线程。
     *
     * 参数说明：
     *   - tag：命令主题
     *   - value：命令值
     */
    void deliverCommand(const QString& tag, const QVariant& value)
    {
        ObserveData(tag, value);
    }
};

class SinewaveDemoTests : public QObject {
    Q_OBJECT

private slots:
    /*
     * SineModel 收到启动命令后应发布采样点。
     *
     * 约束：Demo 的 ViewModel 只消费 sine/point，不在 View 中计算波形。
     */
    void sineModelPublishesPointsAfterStartCommand()
    {
        TestableSineModel model(nullptr, true);
        QSignalSpy publishSpy(&model, SIGNAL(RequestPublish(QString,QVariant)));
        QVERIFY(publishSpy.isValid());

        QVERIFY(QMetaObject::invokeMethod(
            &model,
            [&model]() {
                model.setupForTest();
            },
            Qt::BlockingQueuedConnection));

        model.OnDataReceived(QStringLiteral("sine/cmd/start"), QVariant(true));

        QTRY_VERIFY_WITH_TIMEOUT(std::any_of(
            publishSpy.cbegin(),
            publishSpy.cend(),
            [](const QList<QVariant>& arguments) {
                return arguments.at(0).toString() == QStringLiteral("sine/point");
            }), 250);
    }
};

QTEST_MAIN(SinewaveDemoTests)

#include "SinewaveDemoTests.moc"

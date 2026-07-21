#include <QtTest/QtTest>

#include "AlwaysPolicy.h"
#include "ModbusMasterModel.h"
#include "PidPressureModule.h"

class ModbusModuleTests : public QObject {
    Q_OBJECT

private slots:
    /*
     * Modbus 模块订阅必须由 SetupSubscriptions 发出。
     *
     * 约束：构造函数阶段对象通常尚未接入 Mediator，直接 Subscribe 会丢失信号。
     */
    void pidPressureModuleRegistersCommandSubscriptionsInSetupSubscriptions()
    {
        qRegisterMetaType<PolicyPtr>("PolicyPtr");

        const QList<PointDef> points;
        PidPressureModule module(0, 1, 100, 10, points);
        QSignalSpy spy(&module, SIGNAL(RequestSubscribe(Observe*,QString,QVariant,PolicyPtr)));
        QVERIFY(spy.isValid());

        module.SetupSubscriptions();

        QCOMPARE(spy.size(), 2);
        QCOMPARE(spy.at(0).at(1).toString(), QStringLiteral("pid_pressure/0/cmd/set_sp"));
        QCOMPARE(spy.at(1).at(1).toString(), QStringLiteral("pid_pressure/0/cmd/set_enable"));
    }

    /*
     * 未启动 TCP 时写 float 不应崩溃。
     *
     * 约束：上层命令可能早于通讯连接建立到达，Master 必须安全拒绝。
     */
    void writeFloatBeforeStartTcpDoesNotCrash()
    {
        ModbusMasterModel master;

        master.writeFloat(1, 100, 12.5);

        QVERIFY(true);
    }

    /*
     * 未启动 TCP 时写 bit 不应崩溃。
     *
     * 约束：缓存未命中会尝试读寄存器，必须先确认客户端存在。
     */
    void writeBitBeforeStartTcpDoesNotCrash()
    {
        ModbusMasterModel master;

        master.writeBit(1, 100, 0, true);

        QVERIFY(true);
    }

    /*
     * 注册空模块不应崩溃。
     *
     * 约束：模块列表只接受有效模块指针。
     */
    void addNullModuleDoesNotCrash()
    {
        ModbusMasterModel master;

        master.addModule(nullptr);

        QVERIFY(true);
    }
};

QTEST_MAIN(ModbusModuleTests)

#include "ModbusModuleTests.moc"

#include "ModbusMasterModel.h"
#include "ModbusModuleBase.h"

#include <QDebug>

/*
 * 构造函数
 */
ModbusMasterModel::ModbusMasterModel(QObject* parent)
    : QObject(parent)
{
    /*
     * 将定时器 timeout 信号
     * 连接到 poll() 槽函数
     */
    connect(&m_pollTimer,
            &QTimer::timeout,
            this,
            &ModbusMasterModel::poll);
}

void ModbusMasterModel::startTcp(
    const QString& host,
    int port)
{
    if (m_client) {
        m_pollTimer.stop();
        m_client->disconnectDevice();
        m_client->deleteLater();
        m_client = nullptr;
    }

    /*
     * 创建 Modbus TCP Client
     *
     * 注意：
     * - 只创建一次
     * - 所有模块共用这个连接
     */
    m_client = new QModbusTcpClient(this);

    /*
     * 设置 PLC 网络参数
     */
    m_client->setConnectionParameter(
        QModbusDevice::NetworkAddressParameter,
        host);

    m_client->setConnectionParameter(
        QModbusDevice::NetworkPortParameter,
        port);

    /*
     * 发起 TCP 连接
     */
    m_client->connectDevice();

    /*
     * 启动轮询定时器
     *
     * 示例：200ms 轮询一次
     */
    m_pollTimer.start(200);
}

void ModbusMasterModel::addModule(
    ModbusModuleBase* module)
{
    if (!module) {
        return;
    }

    /*
     * 将模块加入轮询列表
     *
     * 同时把 Master 指针注入给模块，
     * 使模块具备写寄存器的能力
     */
    module->setMaster(this);
    m_modules << module;
}

void ModbusMasterModel::poll()
{
    if (!m_client) {
        return;
    }

    /*
     * 核心轮询逻辑
     *
     * 每一轮：
     * - 顺序遍历所有模块
     * - 向 PLC 发送读取请求
     *
     * 注意：
     * - 这里不关心模块内容
     * - 只负责调度
     */
    for (auto* module : m_modules) {

        // 由模块提供“我要读哪一段寄存器”
        QModbusDataUnit unit = module->buildReadUnit();

        /*
         * 发送 Modbus 读请求
         *
         * unitId 在你的系统中是固定的
         * （同一个 PLC）
         */
        if (auto* reply =
            m_client->sendReadRequest(unit, /*unitId=*/1)) {

            /*
             * 异步处理读完成
             */
            connect(reply,
                    &QModbusReply::finished,
                    this,
                    [=]() {
                        /*
                         * 将 Modbus 响应交给模块解析
                         */
                        module->handleReadResult(
                            reply->result());

                        /*
                         * 释放 reply 对象
                         */
                        reply->deleteLater();
                    });
        }
    }
}

void ModbusMasterModel::writeFloat(
    int unitId,
    int reg,
    double value)
{
    if (!m_client) {
        qWarning() << "[ModbusMasterModel] writeFloat ignored before startTcp";
        return;
    }

    /*
     * 将 double 转换为 float
     * 再拆成两个 16 位寄存器
     */
    float f = static_cast<float>(value);
    quint32 raw;
    std::memcpy(&raw, &f, sizeof(float));

    QModbusDataUnit unit(
        QModbusDataUnit::HoldingRegisters,
        reg - 1,   // QtModbus 使用 0-based 地址
        2);

    unit.setValue(0, raw >> 16);
    unit.setValue(1, raw & 0xFFFF);

    /*
     * 发送写请求
     */
    m_client->sendWriteRequest(unit, unitId);
}

// 假设你已经有：QHash<int, quint16> m_hrCache;  // key = reg(1-based or 0-based你统一)
// 并且在 read reply 的时候更新它

void ModbusMasterModel::writeBit(int unitId, int reg1Based, int bit, bool on)
{
    if (!m_client) {
        qWarning() << "[ModbusMasterModel] writeBit ignored before startTcp";
        return;
    }

    const int addr0 = reg1Based - 1;   // QtModbus 0-based 地址
    const int cacheKey = reg1Based;    // 你也可以用 addr0，关键是统一

    auto doWrite = [&](quint16 currentWord){
        quint16 value = currentWord;
        if (on) value |=  (quint16(1) << bit);
        else    value &= ~(quint16(1) << bit);

        QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, addr0, 1);
        unit.setValue(0, value);
        m_client->sendWriteRequest(unit, unitId);
    };

    if (m_hrCache.contains(cacheKey)) {
        doWrite(m_hrCache.value(cacheKey));
        return;
    }

    // 缓存没有：先读一次，再在回调里改写
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, addr0, 1);
    if (auto* reply = m_client->sendReadRequest(readUnit, unitId)) {
        connect(reply, &QModbusReply::finished, this, [=]() mutable {
            reply->deleteLater();
            if (reply->error() != QModbusDevice::NoError) return;

            const auto result = reply->result();
            quint16 currentWord = result.value(0);
            // 更新缓存
            m_hrCache[cacheKey] = currentWord;
            // 再写
            doWrite(currentWord);
        });
    }
}

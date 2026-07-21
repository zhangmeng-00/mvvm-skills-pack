#include "PidPressureModule.h"
#include "ModbusMasterModel.h"
#include <cstring>
#include "AlwaysPolicy.h"

PidPressureModule::PidPressureModule(int instanceId,
                                     int unitId,
                                     int baseReg,
                                     int stride,
                                     const QList<PointDef>& points,
                                     QObject* parent)
    : ModbusModuleBase("pid_pressure", instanceId, parent),
    m_unitId(unitId),
    m_baseReg(baseReg),
    m_stride(stride),
    m_points(points)
{
}

void PidPressureModule::SetupSubscriptions()
{
    /*
     * 订阅控制命令
     *
     * 约定：
     * - UI / ViewModel 只能 Publish cmd
     * - Model 才能 Subscribe 并执行
     * - 订阅必须在接入 Mediator 后执行，避免构造阶段信号丢失
     */
    Subscribe(topicBase() + "/cmd/set_sp", QVariant(0.0), std::make_shared<AlwaysPolicy>());
    Subscribe(topicBase() + "/cmd/set_enable", QVariant(false), std::make_shared<AlwaysPolicy>());
}

QModbusDataUnit PidPressureModule::buildReadUnit()
{
    /*
     * 构造整段寄存器读取请求
     *
     * 注意：
     * - QtModbus 使用 0-based 地址
     * - Excel 中 10000 → QtModbus 里是 9999
     */
    return QModbusDataUnit(
        QModbusDataUnit::HoldingRegisters,
        m_baseReg - 1,
        m_stride
        );
}

void PidPressureModule::handleReadResult(
    const QModbusDataUnit& unit)
{
    /*
     * 将一整段寄存器数据
     * 映射为业务语义并发布 state
     */
    for (const auto& p : m_points) {
        QVariant value;

        switch (p.type) {

        case DataType::BoolBit: {
            // 读取某个寄存器的 bit
            quint16 word = unit.value(p.offset);
            value = bool(word & (1 << p.bit));
            break;
        }

        case DataType::Float32: {
            // 32 位浮点，占两个寄存器
            quint32 raw =
                (unit.value(p.offset) << 16) |
                unit.value(p.offset + 1);

            float f;
            std::memcpy(&f, &raw, sizeof(float));
            value = f;
            break;
        }
        }

        // 发布 state Topic
        Publish(
            topicBase() + "/state/" + p.key,
            value
            );
    }
}

void PidPressureModule::ObserveData(
    const QString& tag, const QVariant& value)
{
    /*
     * 处理控制命令
     * 这里只做“寄存器写入”
     */
    if (!m_master)
        return;

    if (tag.endsWith("/cmd/set_sp")) {
        // SP：offset = 2
        m_master->writeFloat(
            m_unitId,
            m_baseReg + 2,
            value.toDouble()
            );
    }
    else if (tag.endsWith("/cmd/set_enable")) {
        // Enable：offset = 48, bit = 0
        m_master->writeBit(
            m_unitId,
            m_baseReg + 48,
            0,
            value.toBool()
            );
    }
}

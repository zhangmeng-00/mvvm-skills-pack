#pragma once
#include "ModbusModuleBase.h"
#include "PointDef.h"
#include <QList>

/*
 * =====================================================
 * PidPressureModule
 * =====================================================
 *
 * 说明：
 * - 一个实例 = 一个压力点位（如 BPR70）
 * - 所有实例：
 *   ✔ 共用一个 Modbus TCP 连接
 *   ✔ 共用一个 UnitId
 *   ✔ 通过 base + stride 区分地址
 *
 * 核心原则：
 * - 一次性读取 base ~ base+stride-1
 * - 内部再按 PointDef 解析
 */
class PidPressureModule : public ModbusModuleBase
{
public:
    /*
     * instanceId : 实例编号（0/1/2）
     * unitId     : PLC 的 UnitId（同一 PLC 通常固定）
     * baseReg    : 起始寄存器（10000 / 10050 / 10100）
     * stride     : 每个点位占用寄存器数量（50）
     * points     : 点位定义表（来自 Excel）
     */
    PidPressureModule(int instanceId,
                      int unitId,
                      int baseReg,
                      int stride,
                      const QList<PointDef>& points,
                      QObject* parent = nullptr);

    // 构造“整段读取”的 Modbus 请求
    QModbusDataUnit buildReadUnit() override;

    // 将整段寄存器解析为 state/*
    void handleReadResult(const QModbusDataUnit& unit) override;

    /*
     * 注册控制命令订阅
     *
     * 副作用：通过 Observe::Subscribe 发出订阅请求。该函数由 AppContext
     * 在对象接入 Mediator 后调用，避免构造阶段订阅信号丢失。
     */
    void SetupSubscriptions() override;

protected:
    // 处理来自 ViewModel 的 cmd/*
    void ObserveData(const QString& tag,
                     const QVariant& value) override;

private:
    int m_unitId;     // PLC UnitId（固定）
    int m_baseReg;    // 起始寄存器
    int m_stride;     // 寄存器段长度
    QList<PointDef> m_points;
};

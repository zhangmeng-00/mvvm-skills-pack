#pragma once
#include "ActorObserve.h"
#include <QModbusDataUnit>

/*
 * =====================================================
 * ModbusModuleBase
 * =====================================================
 *
 * 设计意图（请认真读）：
 *
 * - 一个 Module = 一个“功能模块实例”
 *   例如：
 *     - 一个压力点位
 *     - 一个温度点位
 *
 * - Module：
 *   ✔ 不创建 Modbus 连接
 *   ✔ 不关心 TCP / RTU
 *   ✔ 不做轮询调度
 *
 * - Module 只负责：
 *   1) 描述自己要读哪些寄存器
 *   2) 解析寄存器为业务语义
 *   3) Publish state/*
 *   4) 订阅并处理 cmd/*
 */
class ModbusMasterModel;

class ModbusModuleBase : public ActorObserve
{
public:
    /*
     * moduleName : 模块类型名，如 "pid_pressure"
     * instanceId : 实例编号（0 / 1 / 2 ...）
     */
    ModbusModuleBase(QString moduleName,
                     int instanceId,
                     QObject* parent = nullptr);

    /*
     * 由 ModbusMasterModel 注入
     * Module 自身不拥有 Master
     */
    void setMaster(ModbusMasterModel* master);

    /*
     * 构造 Modbus 读取请求
     * - 只描述“读什么”
     * - 不发送、不等待
     */
    virtual QModbusDataUnit buildReadUnit() = 0;

    /*
     * Master 收到 Modbus 响应后
     * 回调该函数进行解析
     */
    virtual void handleReadResult(const QModbusDataUnit& unit) = 0;

protected:
    /*
     * Topic 前缀统一生成：
     * 例如：pid_pressure/0
     */
    QString topicBase() const;

    ModbusMasterModel* m_master{};  // 通讯主模型（非拥有）
    QString m_moduleName;           // 模块名
    int m_instanceId{};             // 实例编号
};

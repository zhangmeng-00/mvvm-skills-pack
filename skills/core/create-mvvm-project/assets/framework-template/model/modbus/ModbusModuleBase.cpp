#include "ModbusModuleBase.h"

/*
 * 构造函数
 */
ModbusModuleBase::ModbusModuleBase(QString name,
                                   int instanceId,
                                   QObject* parent)
    : ActorObserve(parent),
    m_moduleName(name),
    m_instanceId(instanceId)
{
}

void ModbusModuleBase::setMaster(ModbusMasterModel* master)
{
    m_master = master;
}

QString ModbusModuleBase::topicBase() const
{
    // 统一 Topic 命名规范
    return QString("%1/%2").arg(m_moduleName).arg(m_instanceId);
}

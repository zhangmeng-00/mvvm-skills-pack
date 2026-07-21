#pragma once

#include <QObject>
#include <QTimer>
#include <QList>
#include <QModbusTcpClient>
#include <QHash>

class ModbusModuleBase;

/*
 * =========================================================
 * ModbusMasterModel
 * =========================================================
 *
 * 【设计定位】
 * ---------------------------------------------------------
 * ModbusMasterModel 是系统中唯一的 Modbus TCP 通讯管理者。
 *
 * 在整个架构中：
 * - 它是“Master”
 * - 所有 ModbusModuleBase 都是“从属模块”
 *
 * 【核心职责】
 * ---------------------------------------------------------
 * 1. 管理 Modbus TCP 连接（建立 / 断开）
 * 2. 按固定周期轮询所有模块
 * 3. 发送 Modbus 读写请求
 * 4. 将 Modbus 响应分发给对应模块解析
 *
 * 【明确不做的事】
 * ---------------------------------------------------------
 * ❌ 不解析寄存器含义
 * ❌ 不做业务判断
 * ❌ 不发布任何 Topic
 * ❌ 不知道模块是“压力 / 温度 / 阀门”
 *
 * 【为什么必须只有一个 Master？】
 * ---------------------------------------------------------
 * - 一个 PLC = 一个 TCP 连接
 * - 多个 TCP 连接会增加 PLC 负担
 * - Modbus 协议本身是“主从轮询模型”
 */
class ModbusMasterModel : public QObject
{
public:
    /*
     * 构造函数
     *
     * 只做最基础的初始化：
     * - 定时器
     * - 信号槽连接
     *
     * 不在构造函数中建立 TCP 连接
     */
    explicit ModbusMasterModel(QObject* parent = nullptr);

    /*
     * 启动 Modbus TCP 连接
     *
     * @param host
     *   PLC 的 IP 地址
     *
     * @param port
     *   Modbus TCP 端口（通常是 502）
     *
     * 调用时机：
     * - AppContext 初始化阶段
     * - 程序启动时调用一次
     */
    void startTcp(const QString& host, int port);

    /*
     * 注册一个 Modbus 模块
     *
     * 说明：
     * - Module 生命周期由外部管理
     * - Master 只保存指针，不负责释放
     */
    void addModule(ModbusModuleBase* module);

    /*
     * 写 32 位浮点数到保持寄存器
     *
     * 说明：
     * - 用于处理模块的 cmd/set_xxx
     * - float 会拆成两个 16 位寄存器
     */
    void writeFloat(int unitId, int reg, double value);

    /*
     * 写保持寄存器中的某一 bit
     *
     * 说明：
     * - 典型用于 enable / mode / flag
     * - 会先读当前 word，再修改 bit 后写回
     */
    void writeBit(int unitId, int reg, int bit, bool on);

private slots:
    /*
     * 轮询槽函数
     *
     * 说明：
     * - 由定时器周期性触发
     * - 每次轮询：
     *     1) 遍历所有 Module
     *     2) 请求 Module 的读单元
     *     3) 发送 Modbus 读请求
     */
    void poll();

private:
    /*
     * Modbus TCP 客户端
     *
     * 整个系统中只有一个实例
     */
    QModbusTcpClient* m_client { nullptr };

    /*
     * 所有已注册的 Modbus 模块
     */
    QList<ModbusModuleBase*> m_modules;

    /*
     * 轮询定时器
     *
     * 轮询周期可配置（如 200ms）
     */
    QTimer m_pollTimer;
    QHash<int, quint16> m_hrCache;
};

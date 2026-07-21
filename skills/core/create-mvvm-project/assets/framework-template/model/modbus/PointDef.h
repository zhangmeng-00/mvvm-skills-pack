#pragma once
#include <QString>

/*
 * =========================
 * 点位数据类型定义
 * =========================
 *
 * 说明：
 * - 一个 PointDef 对应 Excel 中的一行
 * - DataType 用于指导 Modbus 寄存器如何解析
 */
enum class DataType {
    BoolBit,    // 保持寄存器中的某一 bit（如 10048_0）
    Float32     // 32 位浮点数，占用 2 个保持寄存器
};

/*
 * =========================
 * 点位访问权限
 * =========================
 *
 * 说明：
 * - R  : 只读（只能从 PLC 读）
 * - RW : 读写（既可读，也可通过 cmd 写）
 *
 * 主要用于约束：哪些点位可以生成 cmd
 */
enum class Access {
    R,
    RW
};

/*
 * =========================
 * 点位定义结构
 * =========================
 *
 * 重要设计点（非常关键）：
 * - offset 是【相对 base 寄存器】的偏移
 * - 不再保存绝对寄存器地址
 *
 * 举例：
 * - base = 10000
 * - offset = 2
 * → 实际寄存器 = 10002
 */
struct PointDef {
    QString key;     // 语义名称，如 "pv" / "sp" / "enable"
    int offset;      // 相对 base 的寄存器偏移（0 ~ stride-1）
    int bit;         // bit 位（0~15），-1 表示不是 bit
    DataType type;   // 数据类型
    Access access;   // 访问权限
};

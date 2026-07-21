#pragma once

#include <QString>
#include <QObject>

/*
 * IModel - 通用模型接口
 * ============================================================
 * 所有业务 Model 实现此接口
 * 核心能力：生命周期管理、调试信息
 *
 * 注意：订阅/发布功能由基类 Observe 提供，无需重复定义
 *
 * 使用方式：
 *   class UserModel : public ActorObserve, public IModel { ... };
 */
class IModel {
public:
    virtual ~IModel() = default;

    // ===== 模型标识 =====
    virtual QString modelName() const = 0;

    // ===== 生命周期 =====
    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    // ===== 调试 =====
    virtual QString debugInfo() const = 0;
};

// 智能指针类型别名
using IModelPtr = std::shared_ptr<IModel>;

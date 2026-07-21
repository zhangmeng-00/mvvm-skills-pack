#pragma once

#include "ActorObserve.h"
#include "LogEntry.h"
#include <QString>

/*
 * Logger - 全局日志服务
 * ============================================================
 * 继承 ActorObserve，通过 Publish 发布日志
 *
 * 使用方式：
 *   Logger::instance().info("ModuleName", "message");
 *   Logger::error("Modbus", "Connection failed");
 *
 * 便捷宏（推荐）：
 *   LOG_DEBUG("Module", "message")
 *   LOG_INFO("Module", "message")
 *   LOG_WARN("Module", "message")
 *   LOG_ERROR("Module", "message")
 */
class Logger : public ActorObserve {
    Q_OBJECT
public:
    // 单例访问
    static Logger* instance() {
        static Logger inst;
        return &inst;
    }

    // 设置发布 tag（默认 "user/logging"）
    void setPublishTag(const QString& tag) { m_publishTag = tag; }
    QString publishTag() const { return m_publishTag; }

    // 便捷方法
    Q_INVOKABLE void debug(const QString& module, const QString& message);
    Q_INVOKABLE void info(const QString& module, const QString& message);
    Q_INVOKABLE void warn(const QString& module, const QString& message);
    Q_INVOKABLE void error(const QString& module, const QString& message);
    Q_INVOKABLE void fatal(const QString& module, const QString& message);

    // 条件日志
    Q_INVOKABLE void logIf(bool condition, LogLevel level,
                           const QString& module, const QString& message);

    // 设置最小日志级别
    void setMinLevel(LogLevel level);
    LogLevel minLevel() const { return m_minLevel; }

    // 检查是否应该记录
    bool shouldLog(LogLevel level) const;

signals:
    // 直接发送日志信号（用于 UI 实时显示）
    void logEmitted(const LogEntry& entry);

protected:
    // 实现 ObserveData（Logger 不需要订阅任何内容）
    void ObserveData(const QString& tag, const QVariant& value) override {
        Q_UNUSED(tag);
        Q_UNUSED(value);
    }

private:
    void emitLog(LogLevel level, const QString& module, const QString& message);

private:
    LogLevel m_minLevel = LogLevel::INFO;
    QString m_publishTag = QStringLiteral("user/logging");
};

// 便捷宏
#define LOG_DEBUG(module, message) do { Logger::instance()->debug(module, message); } while(0)
#define LOG_INFO(module, message)  do { Logger::instance()->info(module, message);  } while(0)
#define LOG_WARN(module, message)  do { Logger::instance()->warn(module, message);  } while(0)
#define LOG_ERROR(module, message) do { Logger::instance()->error(module, message); } while(0)
#define LOG_FATAL(module, message) do { Logger::instance()->fatal(module, message); } while(0)
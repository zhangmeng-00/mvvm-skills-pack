#pragma once

#include <QString>
#include <QMetaType>
#include <QList>

// 日志级别枚举
enum class LogLevel {
    Debug,
    INFO,
    WARN,
    ERROR,
    FATAL
};

// 日志条目结构体
struct LogEntry {
    QString timestamp;
    QString modelName;
    LogLevel logLevel;
    QString logMessage;
};

// 为LogEntry结构体提供Q_DECLARE_METATYPE宏，使其可以在QVariant中使用
Q_DECLARE_METATYPE(LogEntry)
Q_DECLARE_METATYPE(QList<LogEntry>)

#pragma once

#include "ActorObserve.h"
#include "LogEntry.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>

/*
 * LogModel
 * ============================================================
 * 日志模块，负责处理日志记录和存储
 *
 * 功能：
 * -----------------------------------------------------------
 * - 订阅 subscribeTag 标签，接收日志数据
 * - 将日志数据存储到 SQLite 数据库
 * - 查询最新100条日志数据并发布为 publishTag
 *
 * 用法：
 *   LogModel* lm = new LogModel("user/logging", "user/logList", nullptr, true);
 *   mediator->ConnectObserve(lm);
 */
class LogModel : public ActorObserve {
    Q_OBJECT
public:
    explicit LogModel(const QString& subscribeTag = "user/logging",
                      const QString& publishTag = "user/logList",
                      QObject* parent = nullptr,
                      bool useSeparateThread = false);
    ~LogModel() override;

protected:
    // 设置订阅
    void SetupSubscriptions() override;
    // 处理订阅数据
    void ObserveData(const QString& tag, const QVariant& value) override;

private:
    // 初始化数据库
    bool initDatabase();
    // 插入日志记录
    bool insertLog(const LogEntry& logEntry);
    // 查询最新100条日志
    QList<LogEntry> queryLatestLogs();
    // 发布日志列表
    void publishLogList();

private:
    QSqlDatabase m_db;
    QString m_dbPath;

    // tag 名称（通过构造函数注入，不依赖 Tags.h）
    QString m_subscribeTag;
    QString m_publishTag;
};

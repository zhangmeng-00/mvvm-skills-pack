#include "LogModel.h"

#include "AlwaysPolicy.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QThread>

#include <memory>

using namespace std;

LogModel::LogModel(const QString& subscribeTag,
                   const QString& publishTag,
                   QObject* parent,
                   bool useSeparateThread)
    : ActorObserve(parent, useSeparateThread),
      m_dbPath("./logs.db"),
      m_subscribeTag(subscribeTag),
      m_publishTag(publishTag)
{
    qDebug() << "LogModel ctor thread:" << QThread::currentThreadId();
}

LogModel::~LogModel()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void LogModel::SetupSubscriptions()
{
    Subscribe(m_subscribeTag, QVariant::fromValue(LogEntry()), std::make_shared<AlwaysPolicy>());

    // SetupSubscriptions is invoked on the actor's own thread.
    // Initialize DB here to keep QSqlDatabase thread affinity correct.
    if (!initDatabase()) {
        qCritical() << "Failed to initialize database";
        return;
    }

    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM log_entries");
    if (query.next() && query.value(0).toInt() == 0) {
        LogEntry initEntry;
        initEntry.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        initEntry.modelName = "System";
        initEntry.logLevel = LogLevel::INFO;
        initEntry.logMessage = "Application started";
        insertLog(initEntry);
    }

    publishLogList();
}

void LogModel::ObserveData(const QString& tag, const QVariant& value)
{
    qDebug() << "LogModel::ObserveData thread:" << QThread::currentThreadId();

    if (tag == m_subscribeTag) {
        LogEntry logEntry = value.value<LogEntry>();
        if (insertLog(logEntry)) {
            publishLogList();
        }
    }
}

bool LogModel::initDatabase()
{
    if (QSqlDatabase::contains("logConnection")) {
        m_db = QSqlDatabase::database("logConnection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "logConnection");
        m_db.setDatabaseName(m_dbPath);
    }

    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);

    const QString createApplicationsTable = R"(
        CREATE TABLE IF NOT EXISTS applications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            app_name TEXT NOT NULL UNIQUE
        );
    )";
    if (!query.exec(createApplicationsTable)) {
        qCritical() << "Failed to create applications table:" << query.lastError().text();
        return false;
    }

    const QString createLogLevelsTable = R"(
        CREATE TABLE IF NOT EXISTS log_levels (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            level_name TEXT NOT NULL UNIQUE
        );
    )";
    if (!query.exec(createLogLevelsTable)) {
        qCritical() << "Failed to create log_levels table:" << query.lastError().text();
        return false;
    }

    const QString createLogSourcesTable = R"(
        CREATE TABLE IF NOT EXISTS log_sources (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            logger_name TEXT NOT NULL UNIQUE
        );
    )";
    if (!query.exec(createLogSourcesTable)) {
        qCritical() << "Failed to create log_sources table:" << query.lastError().text();
        return false;
    }

    const QString createLogEntriesTable = R"(
        CREATE TABLE IF NOT EXISTS log_entries (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT (datetime('now','localtime')),
            level_id INTEGER NOT NULL,
            message TEXT NOT NULL,
            source_id INTEGER NOT NULL,
            app_id INTEGER NOT NULL,
            FOREIGN KEY (level_id) REFERENCES log_levels(id),
            FOREIGN KEY (source_id) REFERENCES log_sources(id),
            FOREIGN KEY (app_id) REFERENCES applications(id)
        );
    )";
    if (!query.exec(createLogEntriesTable)) {
        qCritical() << "Failed to create log_entries table:" << query.lastError().text();
        return false;
    }

    const QString insertLogLevels = R"(
        INSERT OR IGNORE INTO log_levels (id, level_name) VALUES
        (1, 'Debug'),
        (2, 'INFO'),
        (3, 'WARN'),
        (4, 'ERROR'),
        (5, 'FATAL');
    )";
    if (!query.exec(insertLogLevels)) {
        qCritical() << "Failed to insert log levels:" << query.lastError().text();
        return false;
    }

    const QString insertDefaultApp = R"(
        INSERT OR IGNORE INTO applications (app_name) VALUES ('DefaultApp');
    )";
    if (!query.exec(insertDefaultApp)) {
        qCritical() << "Failed to insert default app:" << query.lastError().text();
        return false;
    }

    return true;
}

bool LogModel::insertLog(const LogEntry& logEntry)
{
    if (!m_db.isOpen()) {
        if (!initDatabase()) {
            return false;
        }
    }

    QSqlQuery query(m_db);

    if (!m_db.transaction()) {
        qCritical() << "Failed to start transaction:" << m_db.lastError().text();
        return false;
    }

    int appId = 1;

    int sourceId = 0;
    query.prepare("SELECT id FROM log_sources WHERE logger_name = ?");
    query.addBindValue(logEntry.modelName);
    if (query.exec() && query.next()) {
        sourceId = query.value(0).toInt();
    } else {
        query.prepare("INSERT INTO log_sources (logger_name) VALUES (?)");
        query.addBindValue(logEntry.modelName);
        if (!query.exec()) {
            qCritical() << "Failed to insert log_source:" << query.lastError().text();
            m_db.rollback();
            return false;
        }
        sourceId = query.lastInsertId().toInt();
    }

    int levelId = static_cast<int>(logEntry.logLevel) + 1;

    query.prepare(R"(
        INSERT INTO log_entries (level_id, message, source_id, app_id)
        VALUES (?, ?, ?, ?)
    )");
    query.addBindValue(levelId);
    query.addBindValue(logEntry.logMessage);
    query.addBindValue(sourceId);
    query.addBindValue(appId);

    if (!query.exec()) {
        qCritical() << "Failed to insert log_entry:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    if (!m_db.commit()) {
        qCritical() << "Failed to commit transaction:" << m_db.lastError().text();
        return false;
    }

    return true;
}

QList<LogEntry> LogModel::queryLatestLogs()
{
    QList<LogEntry> logs;

    if (!m_db.isOpen()) {
        if (!initDatabase()) {
            return logs;
        }
    }

    QSqlQuery query(m_db);
    const QString querySql = R"(
        SELECT le.timestamp, ls.logger_name, ll.id - 1, le.message
        FROM log_entries le
        JOIN log_sources ls ON le.source_id = ls.id
        JOIN log_levels ll ON le.level_id = ll.id
        ORDER BY le.timestamp DESC
        LIMIT 100
    )";

    if (!query.exec(querySql)) {
        qCritical() << "Failed to query logs:" << query.lastError().text();
        return logs;
    }

    while (query.next()) {
        LogEntry logEntry;
        logEntry.timestamp = query.value(0).toString();
        logEntry.modelName = query.value(1).toString();
        logEntry.logLevel = static_cast<LogLevel>(query.value(2).toInt());
        logEntry.logMessage = query.value(3).toString();
        logs.append(logEntry);
    }

    return logs;
}

void LogModel::publishLogList()
{
    const QList<LogEntry> logs = queryLatestLogs();
    const QVariant logListVariant = QVariant::fromValue(logs);
    Publish(m_publishTag, logListVariant);
}

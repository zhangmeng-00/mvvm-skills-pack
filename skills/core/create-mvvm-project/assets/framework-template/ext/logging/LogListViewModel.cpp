#include "LogListViewModel.h"

#include "AlwaysPolicy.h"

#include <QBrush>
#include <QFont>

LogListTableModel::LogListTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int LogListTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_entries.size();
}

int LogListTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 4;
}

QVariant LogListTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const LogEntry& entry = m_entries.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return entry.timestamp;
        case 1: return entry.modelName;
        case 2: return levelToText(entry.logLevel);
        case 3: return entry.logMessage;
        default: break;
        }
    }

    if (role == Qt::ForegroundRole) {
        return QBrush(levelToColor(entry.logLevel));
    }

    if (role == Qt::FontRole) {
        if (entry.logLevel == LogLevel::ERROR || entry.logLevel == LogLevel::FATAL) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    }

    return {};
}

QVariant LogListTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return {};
    }

    switch (section) {
    case 0: return QStringLiteral("Time");
    case 1: return QStringLiteral("Source");
    case 2: return QStringLiteral("Level");
    case 3: return QStringLiteral("Message");
    default: return {};
    }
}

void LogListTableModel::setLogEntries(const QList<LogEntry>& entries)
{
    beginResetModel();
    m_entries = entries;
    endResetModel();
}

QString LogListTableModel::levelToText(LogLevel level) const
{
    switch (level) {
    case LogLevel::Debug: return QStringLiteral("Debug");
    case LogLevel::INFO:  return QStringLiteral("INFO");
    case LogLevel::WARN:  return QStringLiteral("WARN");
    case LogLevel::ERROR: return QStringLiteral("ERROR");
    case LogLevel::FATAL: return QStringLiteral("FATAL");
    default: return QStringLiteral("UNKNOWN");
    }
}

QColor LogListTableModel::levelToColor(LogLevel level) const
{
    switch (level) {
    case LogLevel::Debug: return QColor("#9E9E9E");
    case LogLevel::INFO:  return QColor("#2E7D32");
    case LogLevel::WARN:  return QColor("#F57C00");
    case LogLevel::ERROR: return QColor("#D32F2F");
    case LogLevel::FATAL: return QColor("#B71C1C");
    default: return QColor("#212121");
    }
}

LogListViewModel::LogListViewModel(QObject* parent,
                                   const QString& subscribeTag)
    : BaseViewModel(parent)
    , m_tableModel(new LogListTableModel(this))
    , m_subscribeTag(subscribeTag)
{
    emit tableModelChanged();
}

void LogListViewModel::SetupSubscriptions()
{
    // 使用 AlwaysPolicy(true) 实现 Sticky 功能（新订阅者能收到之前的值）
    Subscribe(m_subscribeTag, QVariant::fromValue(QList<LogEntry>()), std::make_shared<AlwaysPolicy>(true));
}

void LogListViewModel::ObserveData(const QString& tag, const QVariant& value)
{
    if (tag != m_subscribeTag) {
        return;
    }

    const QList<LogEntry> logs = value.value<QList<LogEntry>>();
    m_tableModel->setLogEntries(logs);
}

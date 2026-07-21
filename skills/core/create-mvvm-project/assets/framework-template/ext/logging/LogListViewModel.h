#pragma once

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QColor>
#include <QList>

#include "BaseViewModel.h"
#include "LogEntry.h"

class LogListTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit LogListTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void setLogEntries(const QList<LogEntry>& entries);

private:
    QString levelToText(LogLevel level) const;
    QColor levelToColor(LogLevel level) const;

private:
    QList<LogEntry> m_entries;
};

class LogListViewModel : public BaseViewModel {
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* tableModel READ tableModel NOTIFY tableModelChanged)

public:
    explicit LogListViewModel(QObject* parent = nullptr,
                              const QString& subscribeTag = "user/logList");

    QAbstractItemModel* tableModel() const { return m_tableModel; }

signals:
    void tableModelChanged();

protected:
    void SetupSubscriptions() override;
    void ObserveData(const QString& tag, const QVariant& value) override;

private:
    LogListTableModel* m_tableModel = nullptr;
    QString m_subscribeTag;
};

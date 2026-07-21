#pragma once

#include "BaseViewModel.h"

#include <QColor>
#include <QVariantList>
#include <QList>
#include <QPair>

/*
 * SineViewModel
 * ============================================================
 * 正弦波 Demo 的 ViewModel
 *
 * 订阅 sine/point、sine/status
 * 暴露 Qt Property 给 View Binding
 * 接收 View 控件事件，Publish sine/cmd/* 指令
 */
class SineViewModel : public BaseViewModel {
    Q_OBJECT

    Q_PROPERTY(QVariantList points READ points NOTIFY pointsChanged)
    Q_PROPERTY(double amplitude READ amplitude WRITE setAmplitude NOTIFY amplitudeChanged)
    Q_PROPERTY(double frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(double phase READ phase WRITE setPhase NOTIFY phaseChanged)
    Q_PROPERTY(int sampleCount READ sampleCount WRITE setSampleCount NOTIFY sampleCountChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QColor statusColor READ statusColor NOTIFY statusColorChanged)

public:
    explicit SineViewModel(QObject* parent = nullptr);

    QVariantList points() const;
    double amplitude() const { return m_amplitude; }
    void setAmplitude(double val);
    double frequency() const { return m_frequency; }
    void setFrequency(double val);
    double phase() const { return m_phase; }
    void setPhase(double val);
    int sampleCount() const { return m_sampleCount; }
    void setSampleCount(int val);
    QString status() const { return m_status; }
    QColor statusColor() const { return m_statusColor; }

    // 初始化时推送默认参数到 Model
    Q_INVOKABLE void publishDefaults();

signals:
    void pointsChanged();
    void amplitudeChanged();
    void frequencyChanged();
    void phaseChanged();
    void sampleCountChanged();
    void statusChanged();
    void statusColorChanged();

protected:
    void SetupSubscriptions() override;
    void ObserveData(const QString& tag, const QVariant& value) override;

    // ===== View 输入事件 =====
    void onClicked(const QString& senderId) override;
    void onValueChangedDouble(double value, const QString& senderId) override;
    void onValueChanged(int value, const QString& senderId) override;

private:
    void updateStatus(const QString& newStatus);
    QVariantList bufferToVariantList() const;

    // 滑动窗口缓冲区
    QList<QPair<double, double>> m_buffer;
    static const int MAX_BUFFER_SIZE = 500;

    // 参数镜像
    double m_amplitude = 1.0;
    double m_frequency = 100.0;
    double m_phase     = 0.0;
    int    m_sampleCount = 100;

    QString m_status      = "stopped";
    QColor  m_statusColor = Qt::gray;
};

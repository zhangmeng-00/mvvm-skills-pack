#pragma once

#include "BaseModel.h"

#include <QTimer>
#include <QVariantMap>

/*
 * SineModel
 * ============================================================
 * 正弦波信号生成 Model
 *
 * QTimer 驱动，每个 tick 计算 y = A * sin(2π*f*t + φ)
 * Publish sine/point 采样点
 * 订阅 sine/cmd/* 指令调整参数
 */
class SineModel : public BaseModel {
    Q_OBJECT
public:
    explicit SineModel(QObject* parent = nullptr,
                       bool useSeparateThread = false);

    QString modelName() const override { return "SineModel"; }

    void initialize() override;
    void shutdown() override;

protected:
    void SetupSubscriptions() override;

private:
    void setupTimer();
    void startTimer();
    void stopTimer();
    void onTimerTick();

    // 成员变量
    double m_frequency   = 100.0;  // Hz
    double m_amplitude   = 1.0;
    double m_phase       = 0.0;    // degrees
    int    m_sampleCount = 100;
    int    m_intervalMs  = 200;     // ~60 fps
    bool   m_running     = false;
    bool   m_paused      = false;
    double m_currentT    = 0.0;    // seconds

    // 定时器必须在 SineModel 所属线程中创建，避免独立线程模型跨线程启动 QTimer。
    QTimer* m_timer = nullptr;
    bool m_timerConfigured = false;
    bool m_subscriptionsConfigured = false;
};

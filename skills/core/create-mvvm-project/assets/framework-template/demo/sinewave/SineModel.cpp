#include "SineModel.h"

#include "AlwaysPolicy.h"
#include "Logger.h"

#include <QDebug>
#include <QtMath>
#include <memory>

SineModel::SineModel(QObject* parent, bool useSeparateThread)
    : BaseModel(parent, useSeparateThread)
{
}

void SineModel::initialize()
{
    SetupSubscriptions();
}

void SineModel::shutdown()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
    LOG_INFO("SineModel", "Shutdown");
}

void SineModel::SetupSubscriptions()
{
    setupTimer();

    if (m_subscriptionsConfigured) {
        return;
    }
    m_subscriptionsConfigured = true;

    Subscribe("sine/cmd/start", QVariant(false),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/stop", QVariant(false),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/pause", QVariant(false),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/frequency", QVariant(100.0),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/amplitude", QVariant(1.0),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/phase", QVariant(0.0),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/cmd/sampleCount", QVariant(100),
              std::make_shared<AlwaysPolicy>());

    RegisterHandlers({
        {"sine/cmd/start", [this](const QVariant& v) {
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/start = %1").arg(v.toBool() ? "true" : "false"));
            if (v.toBool()) {
                m_running = true;
                m_paused = false;
                startTimer();
                Publish("sine/status", QVariant("running"));
                LOG_INFO("SineModel", "Signal generation started");
            }
        }},
        {"sine/cmd/stop", [this](const QVariant& v) {
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/stop = %1").arg(v.toBool() ? "true" : "false"));
            if (v.toBool()) {
                m_running = false;
                m_paused = false;
                m_currentT = 0.0;
                stopTimer();
                Publish("sine/status", QVariant("stopped"));
                LOG_INFO("SineModel", "Signal generation stopped");
            }
        }},
        {"sine/cmd/pause", [this](const QVariant& v) {
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/pause = %1").arg(v.toBool() ? "true" : "false"));
            if (v.toBool()) {
                m_paused = !m_paused;
                if (m_paused) {
                    if (m_timer && m_timer->isActive()) {
                        m_timer->stop();
                    }
                    Publish("sine/status", QVariant("paused"));
                    LOG_INFO("SineModel", "Signal generation paused");
                } else {
                    if (m_running) {
                        startTimer();
                    }
                    Publish("sine/status", QVariant("running"));
                    LOG_INFO("SineModel", "Signal generation resumed");
                }
            }
        }},
        {"sine/cmd/frequency", [this](const QVariant& v) {
            double freq = v.toDouble();
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/frequency = %1").arg(freq));
            m_frequency = qMax(1.0, qMin(1000.0, freq));
        }},
        {"sine/cmd/amplitude", [this](const QVariant& v) {
            double amp = v.toDouble();
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/amplitude = %1").arg(amp));
            m_amplitude = qMax(0.0, qMin(10.0, amp));
        }},
        {"sine/cmd/phase", [this](const QVariant& v) {
            double ph = v.toDouble();
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/phase = %1").arg(ph));
            m_phase = qMax(0.0, qMin(360.0, ph));
        }},
        {"sine/cmd/sampleCount", [this](const QVariant& v) {
            int sc = v.toInt();
            LOG_DEBUG("SineModel",
                      QString("sine/cmd/sampleCount = %1").arg(sc));
            m_sampleCount = qMax(10, qMin(500, sc));
        }},
    });
}

void SineModel::setupTimer()
{
    if (m_timerConfigured) {
        return;
    }

    /*
     * 定时器职责：
     *   - 在模型所属线程中周期性触发采样
     *   - 不参与 UI 绘制，只发布 sine/point
     *
     * 约束：
     *   - AppContext 实际调用的是 SetupSubscriptions()
     *   - 独立线程模型不能启动构造线程中创建的 QTimer
     */
    m_timer = new QTimer(this);
    m_timer->setInterval(m_intervalMs);
    QObject::connect(m_timer, &QTimer::timeout,
                     this, &SineModel::onTimerTick,
                     Qt::DirectConnection);
    m_timerConfigured = true;
    LOG_INFO("SineModel", "Timer initialized in model thread");
}

void SineModel::startTimer()
{
    setupTimer();
    m_timer->start(m_intervalMs);
}

void SineModel::stopTimer()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void SineModel::onTimerTick()
{
    if (!m_running || m_paused) {
        return;
    }

    // y = A * sin(2π * f * t + φ)
    double phaseRad = m_phase * M_PI / 180.0;
    double y = m_amplitude * qSin(2.0 * M_PI * m_frequency * m_currentT + phaseRad);

    QVariantMap point;
    point["t"] = m_currentT;
    point["y"] = y;

    Publish("sine/point", QVariant::fromValue(point));

    m_currentT += m_intervalMs / 1000.0;
}

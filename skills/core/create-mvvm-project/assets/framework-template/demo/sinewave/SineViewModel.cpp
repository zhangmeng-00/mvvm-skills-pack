#include "SineViewModel.h"

#include "AlwaysPolicy.h"

#include <QVariantMap>
#include <QDebug>

SineViewModel::SineViewModel(QObject* parent)
    : BaseViewModel(parent)
{
}

QVariantList SineViewModel::points() const
{
    return bufferToVariantList();
}

void SineViewModel::setAmplitude(double val)
{
    if (qFuzzyCompare(m_amplitude, val)) {
        return;
    }
    m_amplitude = qMax(0.0, qMin(10.0, val));
    emit amplitudeChanged();
}

void SineViewModel::setFrequency(double val)
{
    if (qFuzzyCompare(m_frequency, val)) {
        return;
    }
    m_frequency = qMax(1.0, qMin(1000.0, val));
    emit frequencyChanged();
}

void SineViewModel::setPhase(double val)
{
    if (qFuzzyCompare(m_phase, val)) {
        return;
    }
    m_phase = qMax(0.0, qMin(360.0, val));
    emit phaseChanged();
}

void SineViewModel::setSampleCount(int val)
{
    if (m_sampleCount == val) {
        return;
    }
    m_sampleCount = qMax(10, qMin(500, val));
    emit sampleCountChanged();
}

void SineViewModel::publishDefaults()
{
    Publish("sine/cmd/frequency", QVariant(m_frequency));
    Publish("sine/cmd/amplitude", QVariant(m_amplitude));
    Publish("sine/cmd/phase", QVariant(m_phase));
    Publish("sine/cmd/sampleCount", QVariant(m_sampleCount));
}

void SineViewModel::SetupSubscriptions()
{
    Subscribe("sine/point", QVariant::fromValue(QVariantMap()),
              std::make_shared<AlwaysPolicy>());
    Subscribe("sine/status", QVariant("stopped"),
              std::make_shared<AlwaysPolicy>());
}

void SineViewModel::ObserveData(const QString& tag, const QVariant& value)
{
    if (tag == "sine/point") {
        QVariantMap point = value.toMap();
        double t = point["t"].toDouble();
        double y = point["y"].toDouble();

        m_buffer.append({t, y});
        while (m_buffer.size() > MAX_BUFFER_SIZE) {
            m_buffer.removeFirst();
        }

        emit pointsChanged();
    }
    else if (tag == "sine/status") {
        updateStatus(value.toString());
    }
}

void SineViewModel::updateStatus(const QString& newStatus)
{
    if (m_status == newStatus) {
        return;
    }
    m_status = newStatus;

    if (m_status == "running") {
        m_statusColor = QColor("#2E7D32");  // green
    } else if (m_status == "paused") {
        m_statusColor = QColor("#F57C00");  // orange
    } else {
        m_statusColor = QColor("#9E9E9E");  // gray
    }

    emit statusChanged();
    emit statusColorChanged();
}

void SineViewModel::onClicked(const QString& senderId)
{
    if (senderId == "startBtn") {
        Publish("sine/cmd/start", QVariant(true));
    } else if (senderId == "pauseBtn") {
        Publish("sine/cmd/pause", QVariant(true));
    } else if (senderId == "stopBtn") {
        Publish("sine/cmd/stop", QVariant(true));
    }
}

void SineViewModel::onValueChangedDouble(double value, const QString& senderId)
{
    if (senderId == "freqSlider") {
        setFrequency(value);
        Publish("sine/cmd/frequency", QVariant(value));
    } else if (senderId == "ampSlider") {
        setAmplitude(value);
        Publish("sine/cmd/amplitude", QVariant(value));
    } else if (senderId == "phaseSlider") {
        setPhase(value);
        Publish("sine/cmd/phase", QVariant(value));
    }
}

void SineViewModel::onValueChanged(int value, const QString& senderId)
{
    if (senderId == "sampleSlider") {
        setSampleCount(value);
        Publish("sine/cmd/sampleCount", QVariant(value));
    }
}

QVariantList SineViewModel::bufferToVariantList() const
{
    QVariantList list;
    list.reserve(m_buffer.size());
    for (const auto& pair : m_buffer) {
        QVariantMap point;
        point["t"] = pair.first;
        point["y"] = pair.second;
        list.append(point);
    }
    return list;
}

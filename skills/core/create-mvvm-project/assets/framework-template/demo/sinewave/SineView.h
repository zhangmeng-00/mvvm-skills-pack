#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SineViewModel;
class LogListView;

namespace QtCharts {
class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;
}

/*
 * SineView
 * ============================================================
 * 正弦波 Demo 主界面
 *
 * 布局：
 * - 顶部：启动/暂停/停止 + 状态指示灯
 * - 中间：QChartView（左）+ 控制面板（右）
 * - 底部：LogListView（嵌入）
 */
class SineView : public QWidget {
    Q_OBJECT
public:
    explicit SineView(QWidget* parent = nullptr);
    ~SineView() override;

    SineViewModel* viewModel() const { return m_viewModel; }
    LogListView* logListView() const { return m_logListView; }

private:
    void setupUI();
    void setupChart();
    void setupControls();
    void setupBindings();
    void setupConnections();
    void updateChartFromPoints();

    // ViewModel
    SineViewModel* m_viewModel = nullptr;

    // 日志列表（嵌入框架组件）
    LogListView* m_logListView = nullptr;

    // Chart 组件
    QtCharts::QChart*      m_chart      = nullptr;
    QtCharts::QChartView*  m_chartView  = nullptr;
    QtCharts::QLineSeries* m_series     = nullptr;
    QtCharts::QValueAxis*  m_axisX      = nullptr;
    QtCharts::QValueAxis*  m_axisY      = nullptr;

    // 控制控件
    QPushButton* m_startBtn  = nullptr;
    QPushButton* m_pauseBtn  = nullptr;
    QPushButton* m_stopBtn   = nullptr;
    QLabel*      m_statusLabel = nullptr;

    QSlider* m_freqSlider   = nullptr;
    QLabel*  m_freqLabel    = nullptr;

    QSlider* m_ampSlider    = nullptr;
    QLabel*  m_ampLabel     = nullptr;

    QSlider* m_phaseSlider  = nullptr;
    QLabel*  m_phaseLabel   = nullptr;

    QSlider* m_sampleSlider = nullptr;
    QLabel*  m_sampleLabel  = nullptr;
};

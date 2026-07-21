#include "SineView.h"
#include "SineViewModel.h"
#include "LogListView.h"
#include "LogListViewModel.h"
#include "Binding.h"
#include "AppContext.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDebug>

SineView::SineView(QWidget* parent)
    : QWidget(parent)
    , m_viewModel(new SineViewModel(this))
    , m_logListView(new LogListView(this))
{
    setupUI();
    setupChart();
    setupControls();
    setupBindings();
    setupConnections();

    // 将 ViewModel 和 LogListViewModel 注册到 Mediator
    AppContext::instance().ConnectObserve(m_viewModel);

    // LogListView 内部的 viewModel 也需要注册
    if (auto* logVM = m_logListView->viewModel()) {
        auto* obs = dynamic_cast<Observe*>(logVM);
        if (obs) {
            AppContext::instance().ConnectObserve(obs);
        }
    }

    // 推送默认参数
    m_viewModel->publishDefaults();
}

SineView::~SineView()
{
}

void SineView::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(6);

    // ===== 顶部按钮栏 =====
    auto* topBar = new QHBoxLayout();

    m_startBtn = new QPushButton(QStringLiteral("启动"));
    m_pauseBtn = new QPushButton(QStringLiteral("暂停"));
    m_stopBtn  = new QPushButton(QStringLiteral("停止"));
    m_statusLabel = new QLabel(QStringLiteral("● stopped"));

    m_startBtn->setMinimumWidth(80);
    m_pauseBtn->setMinimumWidth(80);
    m_stopBtn->setMinimumWidth(80);
    m_statusLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #9E9E9E;");

    topBar->addWidget(m_startBtn);
    topBar->addWidget(m_pauseBtn);
    topBar->addWidget(m_stopBtn);
    topBar->addStretch();
    topBar->addWidget(m_statusLabel);
    mainLayout->addLayout(topBar);

    // ===== 中间区域：chart + controls =====
    auto* middleLayout = new QHBoxLayout();

    // --- Chart ---
    m_chartView = new QtCharts::QChartView();
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumSize(500, 300);
    middleLayout->addWidget(m_chartView, 3);

    // --- 控制面板 ---
    auto* controlsGroup = new QGroupBox(QStringLiteral("参数控制"));
    auto* controlsLayout = new QVBoxLayout(controlsGroup);
    controlsLayout->setSpacing(10);

    // 频率
    controlsLayout->addWidget(new QLabel(QStringLiteral("频率 (Hz):")));
    m_freqSlider = new QSlider(Qt::Horizontal);
    m_freqSlider->setRange(1, 1000);
    m_freqSlider->setValue(100);
    m_freqLabel = new QLabel(QStringLiteral("100"));
    auto* freqRow = new QHBoxLayout();
    freqRow->addWidget(m_freqSlider, 1);
    freqRow->addWidget(m_freqLabel);
    controlsLayout->addLayout(freqRow);

    // 幅值
    controlsLayout->addWidget(new QLabel(QStringLiteral("幅值:")));
    m_ampSlider = new QSlider(Qt::Horizontal);
    m_ampSlider->setRange(0, 100);  // /10 = 0.0 ~ 10.0
    m_ampSlider->setValue(10);       // default 1.0
    m_ampLabel = new QLabel(QStringLiteral("1.0"));
    auto* ampRow = new QHBoxLayout();
    ampRow->addWidget(m_ampSlider, 1);
    ampRow->addWidget(m_ampLabel);
    controlsLayout->addLayout(ampRow);

    // 相位
    controlsLayout->addWidget(new QLabel(QStringLiteral("相位 (度):")));
    m_phaseSlider = new QSlider(Qt::Horizontal);
    m_phaseSlider->setRange(0, 360);
    m_phaseSlider->setValue(0);
    m_phaseLabel = new QLabel(QStringLiteral("0"));
    auto* phaseRow = new QHBoxLayout();
    phaseRow->addWidget(m_phaseSlider, 1);
    phaseRow->addWidget(m_phaseLabel);
    controlsLayout->addLayout(phaseRow);

    // 采样点数
    controlsLayout->addWidget(new QLabel(QStringLiteral("采样点数:")));
    m_sampleSlider = new QSlider(Qt::Horizontal);
    m_sampleSlider->setRange(10, 500);
    m_sampleSlider->setValue(100);
    m_sampleLabel = new QLabel(QStringLiteral("100"));
    auto* sampleRow = new QHBoxLayout();
    sampleRow->addWidget(m_sampleSlider, 1);
    sampleRow->addWidget(m_sampleLabel);
    controlsLayout->addLayout(sampleRow);

    controlsLayout->addStretch();
    middleLayout->addWidget(controlsGroup, 1);
    mainLayout->addLayout(middleLayout, 1);

    // ===== 底部日志列表 =====
    auto* logLabel = new QLabel(QStringLiteral("实时日志:"));
    mainLayout->addWidget(logLabel);
    mainLayout->addWidget(m_logListView, 1);

    setWindowTitle(QStringLiteral("Sine Wave Demo"));
    resize(1000, 700);
}

void SineView::setupChart()
{
    m_chart = new QtCharts::QChart();
    m_chart->setTitle(QStringLiteral("正弦波形"));
    m_chart->legend()->hide();

    m_series = new QtCharts::QLineSeries();
    m_chart->addSeries(m_series);

    m_axisX = new QtCharts::QValueAxis();
    m_axisX->setTitleText(QStringLiteral("Time (s)"));
    m_axisX->setRange(0, 2.0);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_series->attachAxis(m_axisX);

    m_axisY = new QtCharts::QValueAxis();
    m_axisY->setTitleText(QStringLiteral("Amplitude"));
    m_axisY->setRange(-1.2, 1.2);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisY);

    m_chartView->setChart(m_chart);
}

void SineView::setupControls()
{
    // 滑块值变更 → 更新显示的 label + 触发 ViewModel
    QObject::connect(m_freqSlider, &QSlider::valueChanged,
                     this, [this](int val) {
        m_freqLabel->setText(QString::number(val));
        static_cast<BaseViewModel*>(m_viewModel)->onValueChangedDouble(static_cast<double>(val), "freqSlider");
    });

    QObject::connect(m_ampSlider, &QSlider::valueChanged,
                     this, [this](int val) {
        double amp = val / 10.0;
        m_ampLabel->setText(QString::number(amp, 'f', 1));
        static_cast<BaseViewModel*>(m_viewModel)->onValueChangedDouble(amp, "ampSlider");
    });

    QObject::connect(m_phaseSlider, &QSlider::valueChanged,
                     this, [this](int val) {
        m_phaseLabel->setText(QString::number(val));
        static_cast<BaseViewModel*>(m_viewModel)->onValueChangedDouble(static_cast<double>(val), "phaseSlider");
    });

    QObject::connect(m_sampleSlider, &QSlider::valueChanged,
                     this, [this](int val) {
        m_sampleLabel->setText(QString::number(val));
        static_cast<BaseViewModel*>(m_viewModel)->onValueChanged(val, "sampleSlider");
    });

    // 按钮 → ViewModel
    QObject::connect(m_startBtn, &QPushButton::clicked,
                     this, [this]() {
        static_cast<BaseViewModel*>(m_viewModel)->onClicked("startBtn");
    });

    QObject::connect(m_pauseBtn, &QPushButton::clicked,
                     this, [this]() {
        static_cast<BaseViewModel*>(m_viewModel)->onClicked("pauseBtn");
    });

    QObject::connect(m_stopBtn, &QPushButton::clicked,
                     this, [this]() {
        static_cast<BaseViewModel*>(m_viewModel)->onClicked("stopBtn");
    });
}

void SineView::setupBindings()
{
    // VM Property → View 控件 (Binding)
    Binding::BindProperty(m_statusLabel, "text", m_viewModel, "status");
    // Note: statusColor 绑定到 styleSheet 无法直接用 Property
    // 改用下面的 setupConnections 处理
}

void SineView::setupConnections()
{
    // points 变更 → 重绘 chart
    QObject::connect(m_viewModel, &SineViewModel::pointsChanged,
                     this, &SineView::updateChartFromPoints);

    // status 变更 → 更新 label 颜色
    QObject::connect(m_viewModel, &SineViewModel::statusColorChanged,
                     this, [this]() {
        QColor c = m_viewModel->statusColor();
        m_statusLabel->setStyleSheet(
            QString("font-size: 14px; font-weight: bold; color: %1;")
                .arg(c.name()));
    });

    // amplitude 变更 → 更新 Y 轴范围
    QObject::connect(m_viewModel, &SineViewModel::amplitudeChanged,
                     this, [this]() {
        double amp = m_viewModel->amplitude();
        m_axisY->setRange(-amp * 1.2, amp * 1.2);
    });

    // 初始状态
    m_statusLabel->setStyleSheet(
        QString("font-size: 14px; font-weight: bold; color: %1;")
            .arg(m_viewModel->statusColor().name()));
    m_statusLabel->setText(m_viewModel->status());
}

void SineView::updateChartFromPoints()
{
    QVariantList points = m_viewModel->points();
    m_series->clear();

    if (points.isEmpty()) {
        return;
    }

    double minT = points.first().toMap()["t"].toDouble();
    double maxT = points.last().toMap()["t"].toDouble();

    for (const QVariant& p : points) {
        QVariantMap item = p.toMap();
        m_series->append(item["t"].toDouble(), item["y"].toDouble());
    }

    // X 轴自动滚动：显示最近 2 秒
    if (maxT > 2.0) {
        m_axisX->setRange(maxT - 2.0, maxT);
    } else {
        m_axisX->setRange(0, 2.0);
    }
}

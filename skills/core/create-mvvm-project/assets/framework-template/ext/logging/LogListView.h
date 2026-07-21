#pragma once

#include <QWidget>

namespace Ui {
class LogListView;
}

class LogListViewModel;

class LogListView : public QWidget {
    Q_OBJECT
public:
    explicit LogListView(QWidget* parent = nullptr);
    ~LogListView();

    // 提供获取内部 ViewModel 的方法
    LogListViewModel* viewModel() const { return m_viewModel; }

private:
    Ui::LogListView* m_ui = nullptr;
    LogListViewModel* m_viewModel = nullptr;
};

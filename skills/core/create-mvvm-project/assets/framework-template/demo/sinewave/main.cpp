#include <QApplication>

#include "AppContext.h"
#include "LogModel.h"
#include "Logger.h"
#include "SineModel.h"
#include "SineView.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("SinewaveDemo");

    // ============================================================
    // 1. AppContext — 创建 Mediator 及其后台线程
    // ============================================================
    AppContext& ctx = AppContext::instance();

    // ============================================================
    // 2. LogModel — 日志持久化（独立线程，SQLite）
    //    订阅 "user/logging"，发布 "user/logList"
    // ============================================================
    auto* logModel = ctx.AddComponent<LogModel>(
        "user/logging",     // subscribeTag
        "user/logList",     // publishTag
        nullptr,            // parent (AppContext 托管)
        true);              // useSeparateThread

    // ============================================================
    // 3. SineModel — 正弦波生成（独立线程）
    // ============================================================
    auto* sineModel = ctx.AddComponent<SineModel>(
        nullptr,            // parent
        true);              // useSeparateThread

    // ============================================================
    // 4. SineView — 主界面（主线程）
    //    内部创建：SineViewModel + LogListView(含 LogListViewModel)
    //    并自动 ConnectObserve 到 Mediator
    // ============================================================
    SineView* sineView = new SineView();
    sineView->show();

    // ============================================================
    // 5. 确保 Logger Publish 到正确的 tag
    // ============================================================
    Logger::instance()->setPublishTag("user/logging");

    int result = app.exec();

    // 清理
    delete sineView;

    return result;
}

#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

// ==========================================================
// MediatorMVAQt 消息总线压测工具
// ==========================================================
//
// 构建与运行:
//   cmake --build --preset vs2019-x86-release --target MediatorBenchmark
//   build\vs2019-x86\tests\Release\MediatorBenchmark.exe
//
// 结果输出到控制台 + benchmark_YYYYMMDD_HHMMSS.txt

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QAtomicInt>
#include <QThread>
#include <QString>
#include <QVector>
#include <QDebug>
#include <QMetaObject>

#include "AlwaysPolicy.h"
#include "ValueChangedPolicy.h"
#include "Mediator.h"
#include "Observe.h"
#include "LogEntry.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <cstdint>
#include <fstream>
#include <ctime>
#include <chrono>

// ==========================================================
// 全局输出 — 同时写到控制台和文件
// ==========================================================

static std::ofstream g_logFile;

static void initOutput()
{
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    struct tm local;
#ifdef _WIN32
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif
    char buf[64];
    strftime(buf, sizeof(buf), "benchmark_%Y%m%d_%H%M%S.txt", &local);

    g_logFile.open(buf, std::ios::out);
    g_logFile << "MediatorMVAQt Benchmark Results" << std::endl;
    g_logFile << "Started: " << buf << std::endl;
    g_logFile << std::endl;
}

static void out(const std::string& s)
{
    std::cout << s;
    if (g_logFile.is_open())
        g_logFile << s;
}

static void flush()
{
    std::cout << std::flush;
    if (g_logFile.is_open())
        g_logFile << std::flush;
}

// ==========================================================
// 格式化
// ==========================================================

static std::string fmtNum64(int64_t n)
{
    std::string s = std::to_string(n);
    std::string r;
    int cnt = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        if (cnt > 0 && cnt % 3 == 0) r = ',' + r;
        r = s[i] + r;
        cnt++;
    }
    return r;
}

// ==========================================================
// 压测观察者 — 原子计数
// ==========================================================

class BenchObserver : public Observe {
public:
    QAtomicInt received{0};

    explicit BenchObserver(QObject* parent = nullptr) : Observe(parent) {}
    void reset() { received.storeRelaxed(0); }
    int count() const { return received.loadRelaxed(); }

protected:
    void handleData(const QString&, const QVariant&) override {
        received.fetchAndAddRelaxed(1);
    }
};

// ==========================================================
// Mediator 跨线程夹具（模拟 AppContext 的线程模型）
// ==========================================================

struct MediatorFixture {
    Mediator* mediator;
    QThread*  thread;

    MediatorFixture() {
        mediator = new Mediator();
        thread   = new QThread();
        mediator->moveToThread(thread);
        QObject::connect(thread, &QThread::finished,
                         mediator, &QObject::deleteLater);
        thread->start();
    }

    void connectObserve(Observe* obs) {
        QMetaObject::invokeMethod(mediator, [this, obs]() {
            mediator->ConnectObserve(obs);
        }, Qt::BlockingQueuedConnection);
    }

    void subscribe(Observe* obs, const QString& tag,
                   const QVariant& value, PolicyPtr policy)
    {
        QMetaObject::invokeMethod(mediator, [obs, tag, value, policy]() {
            obs->Subscribe(tag, value, policy);
        }, Qt::BlockingQueuedConnection);
    }

    ~MediatorFixture() {
        QMetaObject::invokeMethod(mediator, "deleteLater", Qt::QueuedConnection);
        thread->quit();
        if (!thread->wait(5000)) {
            thread->terminate();
            thread->wait();
        }
        delete thread;
    }
};

// ==========================================================
// 结果记录
// ==========================================================

struct BenchResult {
    std::string  label;
    int          numMessages;
    int          numSubscribers;
    double       elapsedMs;
    int64_t      totalReceived;
    int64_t      totalExpected;
    bool         timedOut;
};

static void printHeader()
{
    out("\n");
    out("  ===========================================================\n");
    out("    MediatorMVAQt 消息总线性能压测\n");
    out("  ===========================================================\n");
    out("\n");
    out("  [环境]\n");
    out("    主线程 ID: ");
    out(std::to_string((uintptr_t)QThread::currentThreadId()));
    out("\n");
#ifdef QT_NO_DEBUG
    out("    构建模式: Release\n");
#else
    out("    构建模式: Debug\n");
#endif
    out("\n");
    flush();
}

static void printSection(const std::string& title)
{
    out("\n");
    out("  ── "); out(title); out(" ──\n");
    out("\n");
    flush();
}

static void printResult(const BenchResult& r)
{
    double throughput    = r.numMessages / (r.elapsedMs / 1000.0);
    double perMsg        = r.elapsedMs * 1000.0 / r.numMessages;
    double perDelivery   = (r.numSubscribers > 0)
                           ? r.elapsedMs * 1000.0 / (r.numMessages * r.numSubscribers)
                           : 0.0;
    double lossPct = (1.0 - (double)r.totalReceived / r.totalExpected) * 100.0;

    std::ostringstream ss;
    ss << "  "
       << std::left << std::setw(32) << r.label
       << std::right
       << std::setw(10) << std::fixed << std::setprecision(2) << r.elapsedMs
       << std::setw(18) << fmtNum64((int64_t)throughput)
       << std::setw(12) << std::fixed << std::setprecision(2) << perMsg
       << std::setw(12) << std::fixed << std::setprecision(2) << perDelivery;

    if (r.timedOut) {
        ss << "  \xE2\x9A\xA0""超时";
    } else if (lossPct > 0.01) {
        ss << "  \xE2\x9A\xA0""丢包 " << std::fixed << std::setprecision(3) << lossPct << "%";
    } else {
        ss << "  \xE2\x9C\x93";
    }
    ss << std::endl;

    out(ss.str());
    flush();
}

// ==========================================================
// 通用等待函数
// ==========================================================

static void pumpUntil(BenchObserver** observers, int count,
                       int64_t totalExpected, int timeoutMs = 60000)
{
    QElapsedTimer t;
    t.start();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    while (t.elapsed() < timeoutMs) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);

        int64_t total = 0;
        for (int i = 0; i < count; i++)
            total += observers[i]->count();
        if (total >= totalExpected)
            return;

        QThread::yieldCurrentThread();
    }
}

static void pumpOne(BenchObserver& obs, int64_t expected, int timeoutMs = 60000)
{
    BenchObserver* arr[1] = { &obs };
    pumpUntil(arr, 1, expected, timeoutMs);
}

// ==========================================================
// [1] 同线程吞吐 — Mediator dispatch 纯开销
// ==========================================================

static BenchResult runSameThread(Mediator& mediator,
                                  const std::string& label,
                                  int numMessages,
                                  int numSubscribers)
{
    std::vector<std::unique_ptr<BenchObserver>> subscribers;
    BenchObserver publisher;
    mediator.ConnectObserve(&publisher);

    for (int i = 0; i < numSubscribers; i++) {
        auto obs = std::make_unique<BenchObserver>();
        mediator.ConnectObserve(obs.get());
        obs->Subscribe("bench/tag", QVariant(0), std::make_shared<AlwaysPolicy>());
        subscribers.push_back(std::move(obs));
    }
    for (int i = 0; i < 5; i++) QCoreApplication::processEvents();
    for (auto& obs : subscribers) obs->reset();

    // 预热
    for (int i = 0; i < 10; i++) publisher.Publish("bench/tag", i);
    for (int i = 0; i < 5; i++) QCoreApplication::processEvents();
    for (auto& obs : subscribers) obs->reset();

    int64_t expected = (int64_t)numMessages * numSubscribers;
    std::vector<BenchObserver*> rawPtrs;
    for (auto& obs : subscribers) rawPtrs.push_back(obs.get());

    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < numMessages; i++)
        publisher.Publish("bench/tag", i);

    pumpUntil(rawPtrs.data(), numSubscribers, expected);
    double elapsed = timer.elapsed();

    int64_t total = 0;
    for (auto& obs : subscribers) total += obs->count();
    return BenchResult{label, numMessages, numSubscribers, elapsed, total, expected, total < expected};
}

// ==========================================================
// [2] 跨线程吞吐 — 真实场景
// ==========================================================

static BenchResult runCrossThread(MediatorFixture& fix,
                                   const std::string& label,
                                   int numMessages,
                                   int numSubscribers)
{
    std::vector<std::unique_ptr<BenchObserver>> subscribers;
    BenchObserver publisher;
    fix.connectObserve(&publisher);

    for (int i = 0; i < numSubscribers; i++) {
        auto obs = std::make_unique<BenchObserver>();
        fix.connectObserve(obs.get());
        fix.subscribe(obs.get(), "bench/tag", QVariant(0), std::make_shared<AlwaysPolicy>());
        subscribers.push_back(std::move(obs));
    }
    QCoreApplication::processEvents();
    for (auto& obs : subscribers) obs->reset();

    // 预热
    publisher.Publish("bench/tag", 0);
    QCoreApplication::processEvents();
    for (auto& obs : subscribers) obs->reset();

    int64_t expected = (int64_t)numMessages * numSubscribers;
    std::vector<BenchObserver*> rawPtrs;
    for (auto& obs : subscribers) rawPtrs.push_back(obs.get());

    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < numMessages; i++)
        publisher.Publish("bench/tag", i);

    pumpUntil(rawPtrs.data(), numSubscribers, expected);
    double elapsed = timer.elapsed();

    int64_t total = 0;
    for (auto& obs : subscribers) total += obs->count();
    return BenchResult{label, numMessages, numSubscribers, elapsed, total, expected, total < expected};
}

// ==========================================================
// [3] 数据类型开销
// ==========================================================

static BenchResult runCrossThreadTyped(MediatorFixture& fix,
                                        const std::string& label,
                                        int numMessages,
                                        const QVariant& sampleValue)
{
    BenchObserver subscriber, publisher;
    fix.connectObserve(&publisher);
    fix.connectObserve(&subscriber);
    fix.subscribe(&subscriber, "bench/type", sampleValue, std::make_shared<AlwaysPolicy>());
    QCoreApplication::processEvents();
    subscriber.reset();

    QElapsedTimer timer;
    timer.start();

    int typeId = sampleValue.userType();
    if (typeId == QMetaType::Int) {
        for (int i = 0; i < numMessages; i++) publisher.Publish("bench/type", QVariant(i));
    } else if (typeId == QMetaType::Double) {
        for (int i = 0; i < numMessages; i++) publisher.Publish("bench/type", QVariant(i * 0.5));
    } else if (typeId == QMetaType::QString) {
        for (int i = 0; i < numMessages; i++) publisher.Publish("bench/type", QVariant(QString::number(i)));
    } else {
        for (int i = 0; i < numMessages; i++) publisher.Publish("bench/type", sampleValue);
    }

    pumpOne(subscriber, numMessages);
    double elapsed = timer.elapsed();

    return BenchResult{label, numMessages, 1, elapsed, subscriber.count(), numMessages, false};
}

// ==========================================================
// [4] 多 Topic 分布
// ==========================================================

static BenchResult runMultiTopic(MediatorFixture& fix,
                                  const std::string& label,
                                  int numMessages,
                                  int numTopics,
                                  int subscribersPerTopic)
{
    struct BenchTopic {
        std::string tag;
        std::vector<std::unique_ptr<BenchObserver>> subscribers;
    };

    std::vector<BenchTopic> topics(numTopics);
    BenchObserver publisher;
    fix.connectObserve(&publisher);

    for (int t = 0; t < numTopics; t++) {
        std::string tag = "bench/topic_" + std::to_string(t);
        topics[t].tag = tag;
        for (int s = 0; s < subscribersPerTopic; s++) {
            auto obs = std::make_unique<BenchObserver>();
            fix.connectObserve(obs.get());
            fix.subscribe(obs.get(), QString::fromStdString(tag), QVariant(0), std::make_shared<AlwaysPolicy>());
            topics[t].subscribers.push_back(std::move(obs));
        }
    }
    QCoreApplication::processEvents();

    for (auto& topic : topics)
        for (auto& obs : topic.subscribers)
            obs->reset();

    // 预热
    publisher.Publish("bench/topic_0", 0);
    QCoreApplication::processEvents();
    for (auto& topic : topics)
        for (auto& obs : topic.subscribers)
            obs->reset();

    std::vector<BenchObserver*> allObservers;
    for (auto& topic : topics)
        for (auto& obs : topic.subscribers)
            allObservers.push_back(obs.get());

    int64_t totalExpected = (int64_t)numMessages * subscribersPerTopic;

    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < numMessages; i++) {
        int topicIdx = i % numTopics;
        publisher.Publish(QString::fromStdString(topics[topicIdx].tag), i);
    }

    pumpUntil(allObservers.data(), (int)allObservers.size(), totalExpected);
    double elapsed = timer.elapsed();

    int64_t total = 0;
    for (auto* obs : allObservers) total += obs->count();

    return BenchResult{label, numMessages, numTopics * subscribersPerTopic, elapsed, total, totalExpected, total < totalExpected};
}

// ==========================================================
// [5] Policy 过滤开销
// ==========================================================

static BenchResult runPolicyCompare(MediatorFixture& fix,
                                     const std::string& label,
                                     int numMessages,
                                     bool useValueChanged)
{
    BenchObserver subscriber, publisher;
    fix.connectObserve(&publisher);
    fix.connectObserve(&subscriber);

    PolicyPtr policy;
    if (useValueChanged)
        policy = std::make_shared<ValueChangedPolicy>();
    else
        policy = std::make_shared<AlwaysPolicy>();

    fix.subscribe(&subscriber, "bench/policy", QVariant(0), policy);
    QCoreApplication::processEvents();
    subscriber.reset();

    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < numMessages; i++)
        publisher.Publish("bench/policy", QVariant(i % 2));

    pumpOne(subscriber, numMessages);
    double elapsed = timer.elapsed();

    return BenchResult{label, numMessages, 1, elapsed, subscriber.count(), numMessages, false};
}

// ==========================================================
// 入口
// ==========================================================

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    qRegisterMetaType<LogEntry>("LogEntry");
    qRegisterMetaType<QList<LogEntry>>("QList<LogEntry>");
    qRegisterMetaType<PolicyPtr>("PolicyPtr");

    initOutput();
    out("[MediatorBenchmark] Started.\n");
    flush();
    printHeader();

    // === [1/5] 同线程吞吐 ===
    out("\n  >> [1/5] 同线程吞吐\n");
    flush();
    {
        Mediator mediator;
        printResult(runSameThread(mediator, "1,000 条 / 1 订阅者",     1000,   1));
        printResult(runSameThread(mediator, "10,000 条 / 1 订阅者",    10000,  1));
        printResult(runSameThread(mediator, "50,000 条 / 1 订阅者",    50000,  1));
        printResult(runSameThread(mediator, "100,000 条 / 1 订阅者",  100000,  1));
    }

    // === [2/5] 跨线程吞吐 ===
    out("\n  >> [2/5] 跨线程吞吐\n");
    flush();
    {
        MediatorFixture fix;
        printResult(runCrossThread(fix, "1,000 条 / 1 订阅者",     1000,   1));
        printResult(runCrossThread(fix, "10,000 条 / 1 订阅者",    10000,  1));
        printResult(runCrossThread(fix, "50,000 条 / 1 订阅者",    50000,  1));
        printResult(runCrossThread(fix, "100,000 条 / 1 订阅者",  100000,  1));
    }

    // === [3/5] 订阅者扩展 ===
    out("\n  >> [3/5] 订阅者扩展性 (10,000 条)\n");
    flush();
    {
        MediatorFixture fix;
        printResult(runCrossThread(fix, "1 个订阅者",  10000, 1));
        printResult(runCrossThread(fix, "5 个订阅者",  10000, 5));
        printResult(runCrossThread(fix, "10 个订阅者", 10000, 10));
        printResult(runCrossThread(fix, "20 个订阅者", 10000, 20));
        printResult(runCrossThread(fix, "50 个订阅者", 10000, 50));
    }

    // === [4/5] 数据类型 ===
    out("\n  >> [4/5] 数据类型开销 (10,000 条)\n");
    flush();
    {
        MediatorFixture fix;
        printResult(runCrossThreadTyped(fix, "int",      10000, QVariant(0)));
        printResult(runCrossThreadTyped(fix, "double",   10000, QVariant(0.0)));
        printResult(runCrossThreadTyped(fix, "QString",  10000, QVariant(QString("x"))));
        printResult(runCrossThreadTyped(fix, "LogEntry", 10000, QVariant::fromValue(LogEntry())));
    }

    // === [5/5] Policy ===
    out("\n  >> [5/5] Policy 过滤开销 (10,000 条)\n");
    flush();
    {
        MediatorFixture fix;
        printResult(runPolicyCompare(fix, "AlwaysPolicy",        10000, false));
        printResult(runPolicyCompare(fix, "ValueChangedPolicy",  10000, true));
    }

    out("\n");
    out("  ── 测试完成 ──\n");
    out("\n");
    out("  结果已保存到 benchmark_YYYYMMDD_HHMMSS.txt\n");
    out("\n");

    if (g_logFile.is_open()) {
        g_logFile << "\n-- Test Complete --\n";
        g_logFile.close();
    }

    return 0;
}

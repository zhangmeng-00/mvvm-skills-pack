#include "AppContext.h"

#include <QMutexLocker>

AppContext& AppContext::instance()
{
    static AppContext ctx;
    return ctx;
}

AppContext::AppContext(QObject* parent)
    : QObject(parent)
{
    m_mediator = new Mediator();
    m_mediatorThread = new QThread(this);

    m_mediator->moveToThread(m_mediatorThread);
    connect(m_mediatorThread, &QThread::finished, m_mediator, &QObject::deleteLater);

    m_mediatorThread->start();
}

AppContext::~AppContext()
{
    shutdownMediatorThread();
}

Mediator* AppContext::mediator() const
{
    return m_mediator;
}

/**
 * @brief 将一个观察者(Observe)对象线程安全地连接到中介者(Mediator)。
 *
 * 这个函数是应用核心的线程安全接入点。它负责处理将一个通常运行在GUI线程的
 * ViewModel(作为Observe的子类)注册到运行在后台线程的中介者上的所有复杂逻辑。
 *
 * 主要职责包括：
 * 1. 线程同步：使用互斥锁防止对观察者列表的并发访问和重复连接。
 * 2. 自动生命周期管理：通过连接QObject::destroyed信号，实现观察者销毁时自动注销。
 * 3. 线程安全的双向连接：
 *    a. 使用阻塞队列连接(BlockingQueuedConnection)将观察者注册到中介者，确保注册成功后才继续。
 *    b. 使用队列连接(QueuedConnection)触发观察者自身的订阅设置函数，允许异步执行。
 *
 * @param obs 指向要连接的Observe对象的指针。
 */
void AppContext::ConnectObserve(Observe* obs)
{
    // --- 1. 前置条件检查 ---
    if (!obs || !m_mediator) {
        return; // 如果观察者或中介者无效，则直接返回
    }

    // --- 2. 线程安全的防重复注册 ---
    {
        QMutexLocker locker(&m_observerMutex); // 加锁以保证线程安全
        if (m_connectedObservers.contains(obs)) {
            return; // 如果已经连接过，则直接返回，防止重复操作
        }
        // 将新的观察者添加到已连接集合中进行跟踪
        m_connectedObservers.insert(obs);
    }

    // --- 3. 自动生命周期管理 ---
    // 当观察者对象被销毁时，自动从已连接集合中移除它
    connect(obs, &QObject::destroyed, this, [this, obs]() {
        QMutexLocker locker(&m_observerMutex); // 同样需要加锁
        m_connectedObservers.remove(obs);
    });

    // --- 4. 线程安全的“握手”第一步：将观察者连接到中介者 ---
    // 定义一个任务：调用中介者的连接函数
    const auto connectTask = [this, obs]() {
        if (m_mediator) {
            m_mediator->ConnectObserve(obs);
        }
    };

    bool connected = false;
    // 判断当前线程是否就是中介者所在的线程
    if (QThread::currentThread() == m_mediator->thread()) {
        connectTask(); // 如果是，直接调用
        connected = true;
    } else {
        // 如果不是，则使用invokeMethod将任务派发到中介者线程执行
        // 使用阻塞队列连接，确保该任务执行完毕后，当前线程才会继续往下走
        connected = QMetaObject::invokeMethod(
            m_mediator,
            connectTask,
            Qt::BlockingQueuedConnection);
    }

    // 如果连接失败，回滚操作，将观察者从集合中移除
    if (!connected) {
        QMutexLocker locker(&m_observerMutex);
        m_connectedObservers.remove(obs);
        return;
    }

    // --- 5. 线程安全的“握手”第二步：触发观察者设置自己的订阅 ---
    // 定义一个任务：调用观察者自己的订阅设置函数
    const auto setupTask = [obs]() {
        obs->SetupSubscriptions();
    };

    // 判断当前线程是否就是观察者所在的线程
    if (QThread::currentThread() == obs->thread()) {
        setupTask(); // 如果是，直接调用
    } else {
        // 如果不是，将任务派发到观察者自己的线程中执行
        // 使用普通队列连接，派发后立即返回，不阻塞当前线程
        QMetaObject::invokeMethod(obs, setupTask, Qt::QueuedConnection);
    }
}

/**
 * @brief 安全地关闭和清理中介者(Mediator)所在的后台线程。
 *
 * 这个函数是应用程序退出时的关键清理步骤。它确保了后台线程被正确停止，
 * 并且所有相关的对象都被安全地删除，防止内存泄漏或程序崩溃。
 *
 * 清理流程如下：
 * 1. 清空所有已连接的观察者列表，断开所有通信。
 * 2. 通过invokeMethod请求在后台线程的事件循环中删除Mediator对象(deleteLater)。
 * 3. 向后台线程发送退出请求(quit)，使其事件循环停止。
 * 4. 等待后台线程结束，并设置超时处理，以防线程阻塞。
 * 5. 清理相关的成员变量指针。
 */
void AppContext::shutdownMediatorThread()
{
    // --- 1. 清理观察者列表 ---
    // 加锁并清空所有观察者的记录，防止在关闭过程中还有新的交互
    {
        QMutexLocker locker(&m_observerMutex);
        m_connectedObservers.clear();
    }

    // 如果线程指针本就无效，则直接返回
    if (!m_mediatorThread) {
        m_mediator = nullptr;
        return;
    }

    // --- 2. 请求删除中介者对象 ---
    // 如果中介者对象存在，通过元对象系统调用它的deleteLater()槽函数
    // 这是一个线程安全的删除方式，该对象将在其所属线程的事件循环中被安全删除
    if (m_mediator) {
        QMetaObject::invokeMethod(m_mediator, "deleteLater", Qt::QueuedConnection);
    }

    // --- 3. 停止并等待线程结束 ---
    if (m_mediatorThread->isRunning()) {
        // 请求线程的事件循环退出
        m_mediatorThread->quit();
        // 等待线程最多3秒钟，让它有机会完成当前事件并正常退出
        if (!m_mediatorThread->wait(3000)) {
            // 如果3秒后线程仍未退出，打印警告并无限期等待，确保线程最终被回收
            qWarning() << "[AppContext] mediator thread quit timeout, waiting until stop";
            m_mediatorThread->wait();
        }
    }

    // --- 4. 清理指针 ---
    // 将成员变量置空，防止悬挂指针
    m_mediator = nullptr;
    m_mediatorThread = nullptr;
}

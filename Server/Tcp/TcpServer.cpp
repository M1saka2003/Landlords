#include "TcpServer.h"
#include <arpa/inet.h>
#include "TcpConnection.h"
#include "Log.h"

int TcpServer::acceptConnection(void *arg) {
    const auto *server = static_cast<TcpServer *>(arg);
    // 和客户端建立连接
    const int cfd = accept(server->m_lfd, nullptr, nullptr);
    // 从线程池中取出一个子线程的反应堆实例, 去处理这个cfd
    EventLoop *evLoop = server->m_threadPool->takeWorkerEventLoop();
    // 将cfd放到 TcpConnection中处理
    new TcpConnection(cfd, evLoop);
    return 0;
}

TcpServer::TcpServer(const unsigned short port, const int threadNum) {
    m_port = port;
    m_mainLoop = new EventLoop;
    m_threadNum = threadNum;
    m_threadPool = new ThreadPool(m_mainLoop, threadNum);
    setListen();
}

void TcpServer::setListen() {
    // 1. 创建监听的fd
    m_lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_lfd == -1) {
        perror("socket");
        return;
    }
    // 2. 设置端口复用
    constexpr int opt = 1;
    int ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (ret == -1) {
        perror("setsockopt");
        return;
    }
    // 3. 绑定
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(m_lfd, reinterpret_cast<sockaddr *>(&addr), sizeof addr);
    if (ret == -1) {
        perror("bind");
        return;
    }
    // 4. 设置监听
    ret = listen(m_lfd, 128);
    if (ret == -1) {
        perror("listen");
    }
}

void TcpServer::run() {
    Debug("服务器程序已经启动了...");
    // 启动线程池
    m_threadPool->run();
    // 添加检测的任务
    // 初始化一个channel实例
    auto *channel = new Channel(m_lfd, FDEvent::ReadEvent, acceptConnection, nullptr, nullptr, this);
    m_mainLoop->addTask(channel, ElemType::ADD);
    // 启动反应堆模型
    m_mainLoop->run();
}

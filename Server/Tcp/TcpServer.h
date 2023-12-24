#ifndef SERVER_TCPSERVER
#define SERVER_TCPSERVER

#include "EventLoop.h"
#include "ThreadPool.h"

class TcpServer {
public:
    TcpServer(unsigned short port, int threadNum);

    // 初始化监听
    void setListen();

    // 启动服务器
    void run();

    static int acceptConnection(void *arg);

private:
    EventLoop *m_mainLoop;
    ThreadPool *m_threadPool;
    int m_lfd{};
    unsigned short m_port;
    [[maybe_unused]]int m_threadNum;

};

#endif

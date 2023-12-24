#ifndef SERVER_EPOLLDISPATCHER
#define SERVER_EPOLLDISPATCHER

#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <sys/epoll.h>

class[[maybe_unused]]EpollDispatcher : public Dispatcher {
public:
    [[maybe_unused]]explicit EpollDispatcher(EventLoop *evloop);

    ~EpollDispatcher() override;

    // 添加
    int add() override;

    // 删除
    int remove() override;

    // 修改
    int modify() override;

    // 事件监测
    int dispatch(int timeout) override; // 单位: s

private:
    int epollCtl(int op);

private:
    int m_epfd;
    struct epoll_event *m_events;
    const int m_maxNode = 520;
};

#endif
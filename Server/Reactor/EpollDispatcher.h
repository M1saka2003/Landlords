#ifndef SERVER_EPOLLDISPATCHER_H
#define SERVER_EPOLLDISPATCHER_H

#include "EventLoop.h"
#include "Dispatcher.h"
#include <sys/epoll.h>

class[[maybe_unused]]EpollDispatcher final : public Dispatcher {
public:
    [[maybe_unused]] explicit EpollDispatcher(EventLoop *evloop);

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
    [[nodiscard]] int epollCtl(int op) const;

private:
    int m_epfd;
    struct epoll_event *m_events;
    const int m_maxNode = 520;
};

#endif

#ifndef SERVER_POLLDISPATCHER_H
#define SERVER_POLLDISPATCHER_H

#include "Dispatcher.h"
#include <poll.h>


class[[maybe_unused]] PollDispatcher final: public Dispatcher {
public:
    [[maybe_unused]]explicit PollDispatcher(EventLoop *evloop);

    ~PollDispatcher() override;

    // 添加
    int add() override;

    // 删除
    int remove() override;

    // 修改
    int modify() override;

    // 事件监测
    int dispatch(int timeout) override; // 单位: s

private:
    int m_maxfd;
    pollfd *m_fds;
    const int m_maxNode = 1024;
};

#endif
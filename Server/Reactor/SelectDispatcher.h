#ifndef SERVER_SELECTDISPATCHER_H
#define SERVER_SELECTDISPATCHER_H

#include "EventLoop.h"
#include "Dispatcher.h"
#include <sys/select.h>

class [[maybe_unused]]SelectDispatcher final : public Dispatcher {
public:
    [[maybe_unused]] explicit SelectDispatcher(EventLoop *evloop);

    ~SelectDispatcher() override;

    // 添加
    int add() override;

    // 删除
    int remove() override;

    // 修改
    int modify() override;

    // 事件监测
    int dispatch(int timeout) override; // 单位: s

private:
    void setFdSet();

    void clearFdSet();

    fd_set m_readSet{};
    fd_set m_writeSet{};
    const int m_maxSize = 1024;
};

#endif

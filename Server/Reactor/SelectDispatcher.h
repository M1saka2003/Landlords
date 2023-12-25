#ifndef SERVER_SELECTDISPATCHER
#define SERVER_SELECTDISPATCHER

#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <sys/select.h>

class [[maybe_unused]]SelectDispatcher : public Dispatcher {
public:
    [[maybe_unused]]explicit SelectDispatcher(EventLoop *evloop);

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

private:
    fd_set m_readSet{};
    fd_set m_writeSet{};
    const int m_maxSize = 1024;
};

#endif
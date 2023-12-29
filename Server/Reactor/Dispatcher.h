#ifndef SERVER_DISPATCHER_H
#define SERVER_DISPATCHER_H

#include "Channel.h"
#include "EventLoop.h"
#include <string>

class EventLoop;

class Dispatcher {
public:
    explicit Dispatcher(EventLoop *evloop);

    virtual ~Dispatcher();

    // 添加
    virtual int add();

    // 删除
    virtual int remove();

    // 修改
    virtual int modify();

    // 事件监测,单位: s
    virtual int dispatch(int timeout);

    void setChannel(Channel *channel) {
        m_channel = channel;
    }

protected:
    std::string m_name{};
    Channel *m_channel{};
    EventLoop *m_evLoop;
};

#endif
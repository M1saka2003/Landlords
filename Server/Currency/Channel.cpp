#include "Channel.h"
#include <utility>

Channel::Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void *arg) {
    m_arg = arg;
    m_fd = fd;
    m_events = (int) events;
    readCallback = std::move(readFunc);
    writeCallback = std::move(writeFunc);
    destroyCallback = std::move(destroyFunc);
}

void Channel::writeEventEnable(bool flag) {
    if (flag) {
        // m_events |= (int)FDEvent::WriteEvent;
        m_events |= static_cast<int>(FDEvent::WriteEvent);
    }
    else {
        m_events = m_events & ~(int) FDEvent::WriteEvent;
    }
}

bool Channel::isWriteEventEnable() const {
    return m_events & (int) FDEvent::WriteEvent;
}

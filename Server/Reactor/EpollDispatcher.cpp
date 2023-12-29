#include "Dispatcher.h"
#include <sys/epoll.h>
#include <unistd.h>
#include "EpollDispatcher.h"
#include <iostream>

[[maybe_unused]] EpollDispatcher::EpollDispatcher(EventLoop *evloop) : Dispatcher(evloop) {
    m_epfd = epoll_create(10);
    if (m_epfd == -1) {
        perror("epoll_create");
        exit(0);
    }
    m_events = new epoll_event[m_maxNode];
    m_name = "Epoll";
}

EpollDispatcher::~EpollDispatcher() {
    close(m_epfd);
    delete[]m_events;
}

int EpollDispatcher::add() {
    const int ret = epollCtl(EPOLL_CTL_ADD);
    if (ret == -1) {
        perror("epoll_crl add");
        exit(0);
    }
    return ret;
}

int EpollDispatcher::remove() {
    const int ret = epollCtl(EPOLL_CTL_DEL);
    if (ret == -1) {
        perror("epoll_crl delete");
        exit(0);
    }
    // 通过 channel 释放对应的 TcpConnection 资源
    m_channel->destroyCallback(const_cast<void *>(m_channel->getArg()));
    return ret;
}

int EpollDispatcher::modify() {
    const int ret = epollCtl(EPOLL_CTL_MOD);
    if (ret == -1) {
        perror("epoll_crl modify");
        exit(0);
    }
    return ret;
}

int EpollDispatcher::dispatch(const int timeout) {
    const int count = epoll_wait(m_epfd, m_events, m_maxNode, timeout * 1000);
    for (int i = 0; i < count; ++i) {
        const auto events = m_events[i].events;
        const int fd = m_events[i].data.fd;

        if (events & EPOLLIN) {
            m_evLoop->eventActive(fd, static_cast<int>(FDEvent::ReadEvent));
        }
        if (events & EPOLLOUT) {
            m_evLoop->eventActive(fd, static_cast<int>(FDEvent::WriteEvent));
        }
    }
    return 0;
}

int EpollDispatcher::epollCtl(const int op) const {
    struct epoll_event ev{};
    ev.data.fd = m_channel->getSocket();
    int events = 0;
    if (m_channel->getEvent() & static_cast<int>(FDEvent::ReadEvent)) {
        events |= EPOLLIN;
    }
    if (m_channel->getEvent() & static_cast<int>(FDEvent::WriteEvent)) {
        events |= EPOLLOUT;
    }
    ev.events = events;
    const int ret = epoll_ctl(m_epfd, op, m_channel->getSocket(), &ev);
    return ret;
}

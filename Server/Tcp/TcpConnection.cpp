#include "TcpConnection.h"
#include "HttpRequest.h"
#include "Log.h"
#include <iostream>

int TcpConnection::processRead(void *arg) {
    const auto *conn = static_cast<TcpConnection *>(arg);
    // 接收数据
    const int socket = conn->m_channel->getSocket();
    const int count = conn->m_readBuf->socketRead(socket);
    Debug("接收到的http请求数据: %s", conn->m_readBuf->data());
    if (count > 0) {
        // 接收到了 http 请求, 解析http请求
        if (const bool flag = conn->m_request->parseHttpRequest(
            conn->m_readBuf, conn->m_response,
            conn->m_writeBuf, socket); !flag) {
            // 解析失败, 回复一个简单的html
            const std::string errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
            conn->m_writeBuf->appendString(errMsg);
        }
    }
    // 断开连接
    // 这里的DELETE事件由EventLoop调用删除,而不是反应堆轮询做出调用
    conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
    return 0;
}

int TcpConnection::processWrite(void *arg) {
    Debug("开始发送数据了(基于写事件发送)....");
    const auto *conn = static_cast<TcpConnection *>(arg);
    // 发送数据
    if (const std::size_t count = conn->m_writeBuf->sendData(conn->m_channel->getSocket()); count > 0) {
        // 判断数据是否被全部发送出去了
        if (conn->m_writeBuf->readableSize() == 0) {
            // 1. 不再检测写事件 -- 修改channel中保存的事件
            conn->m_channel->writeEventEnable(false);
            // 2. 修改dispatcher检测的集合 -- 添加任务节点
            conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
            // 3. 删除这个节点
            conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
        }
    }
    return 0;
}

int TcpConnection::destroy(void *arg) {
    const auto *conn = static_cast<TcpConnection *>(arg);
    delete conn;
    return 0;
}

TcpConnection::TcpConnection(const int fd, EventLoop *evloop) {
    m_evLoop = evloop;
    m_readBuf = new Buffer(10240);
    m_writeBuf = new Buffer(10240);
    // http
    m_request = new HttpRequest;
    m_response = new HttpResponse;
    m_name = "Connection-" + std::to_string(fd);
    std::cout << "fd = " << fd << std::endl;
    m_channel = new Channel(fd, FDEvent::ReadEvent, processRead, processWrite, destroy, this);
    evloop->addTask(m_channel, ElemType::ADD);
}

TcpConnection::~TcpConnection() {
    delete m_readBuf;
    delete m_writeBuf;
    delete m_request;
    delete m_response;
    m_evLoop->freeChannel(m_channel);
    Debug("连接断开, 释放资源, end, connName: %s", m_name.data());
}

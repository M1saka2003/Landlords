#ifndef SERVER_TCPCONNECTION_H
#define SERVER_TCPCONNECTION_H

#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class TcpConnection {
public:
    TcpConnection(int fd, EventLoop *evloop);

    ~TcpConnection();

    static int processRead(void *arg);

    static int processWrite(void *arg);

    static int destroy(void *arg);

private:
    std::string m_name;
    EventLoop *m_evLoop;
    Channel *m_channel;
    Buffer *m_readBuf;
    Buffer *m_writeBuf;
    // http 协议
    HttpRequest *m_request;
    HttpResponse *m_response;
};

#endif
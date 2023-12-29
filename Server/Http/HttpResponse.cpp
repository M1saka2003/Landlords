#include <iostream>
#include "HttpResponse.h"

HttpResponse::HttpResponse() {
    m_statusCode = StatusCode::Unknown;
    m_headers.clear();
    m_fileName = {};
    sendDataFunc = nullptr;
}

HttpResponse::~HttpResponse() = default;

void HttpResponse::addHeader(const std::string &key, const std::string &value) {
    if (key.empty() || value.empty()) {
        return;
    }
    m_headers.insert(std::make_pair(key, value));
}

void HttpResponse::prepareMsg(Buffer *sendBuf, const int socket) {
    // 状态行

    char tmp[1024] = {0};
    const int code = static_cast<int>(m_statusCode);
    sprintf(tmp, "HTTP/1.1 %d %s\r\n", code, m_info.at(code).data());
    sendBuf->appendString(tmp);
    // 响应头
    for (auto &[key,value]: m_headers) {
        sprintf(tmp, "%s: %s\r\n", key.data(), value.data());
        sendBuf->appendString(tmp);
    }
    // 空行
    sendBuf->appendString("\r\n");
#ifndef MSG_SEND_AUTO
    sendBuf->sendData(socket);
#endif
    // 回复的数据
    sendDataFunc(m_fileName, sendBuf, socket);
}

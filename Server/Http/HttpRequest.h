#ifndef SERVER_HTTPREQUEST
#define SERVER_HTTPREQUEST

#include "Buffer.h"
#include "HttpResponse.h"
#include <map>
#include <utility>

// 当前的解析状态
enum class PrecessState : char {
    ParseReqLine,
    ParseReqHeaders,
    ParseReqBody,
    ParseReqDone
};

// 定义http请求结构体
class HttpRequest {
public:
    HttpRequest();

    ~HttpRequest();

    // 重置
    [[maybe_unused]] void reset();

    // 添加请求头
    void addHeader(const std::string& key, const std::string& value);

    // 根据key得到请求头的value
    [[maybe_unused]] std::string getHeader(const std::string& key);

    // 解析请求行
    bool parseRequestLine(Buffer *readBuf);

    // 解析请求头
    bool parseRequestHeader(Buffer *readBuf);

    // 解析http请求协议
    bool parseHttpRequest(Buffer *readBuf, HttpResponse *response, Buffer *sendBuf, int socket);

    // 处理http请求协议
    bool processHttpRequest(HttpResponse *response);

    // 解码字符串
    static std::string decodeMsg(std::string from);

    // MIME
    static const std::string getFileType(const std::string& name);

    static void sendDir(std::string dirName, Buffer *sendBuf, int cfd);

    static void sendFile(std::string dirName, Buffer *sendBuf, int cfd);

    void setMethod(std::string method) {
        m_method = std::move(method);
    }

    void seturl(std::string url) {
        m_url = std::move(url);
    }

    void setVersion(std::string version) {
        m_version = std::move(version);
    }

    // 获取处理状态
    [[maybe_unused]]PrecessState getState() {
        return m_curState;
    }

    void setState(PrecessState state) {
        m_curState = state;
    }

private:
    static char *splitRequestLine(const char *start, const char *end,
                                  const char *sub, const std::function<void(std::string)> &callback);

    static int hexToDec(char c);

private:
    std::string m_method{};
    std::string m_url{};
    std::string m_version{};
    std::map<std::string, std::string> m_reqHeaders;
    PrecessState m_curState{};
};

#endif
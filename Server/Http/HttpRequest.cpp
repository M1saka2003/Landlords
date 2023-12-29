#include "HttpRequest.h"
#include <cstdio>
#include <strings.h>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "TcpConnection.h"
#include <cassert>
#include <cctype>

char *HttpRequest::splitRequestLine(const char *start, const char *end, const char *sub,
                                    const std::function<void(std::string)> &callback) {
    auto *space = const_cast<char *>(end);
    if (sub != nullptr) {
        space = static_cast<char *>(memmem(start, end - start, sub, strlen(sub)));
        assert(space != nullptr);
    }
    const std::size_t length = space - start;
    callback(std::string(start, length));
    return space + 1;
}


// 将字符转换为整形数
int HttpRequest::hexToDec(const char &c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}

HttpRequest::HttpRequest() = default;

HttpRequest::~HttpRequest() = default;

[[maybe_unused]] void HttpRequest::reset() {
    m_curState = PrecessState::ParseReqLine;
    m_method = m_url = m_version = {};
    m_reqHeaders.clear();
}

void HttpRequest::addHeader(const std::string &key, const std::string &value) {
    if (key.empty() || value.empty()) {
        return;
    }
    m_reqHeaders.insert(make_pair(key, value));
}

[[maybe_unused]] std::string HttpRequest::getHeader(const std::string &key) {
    const auto item = m_reqHeaders.find(key);
    if (item == m_reqHeaders.end()) {
        return {};
    }
    return item->second;
}

bool HttpRequest::parseRequestLine(Buffer *readBuf) {
    // 读出请求行, 保存字符串结束地址
    const char *end = readBuf->findCRLF();
    // 保存字符串起始地址
    const char *start = readBuf->data();
    // 请求行总长度
    if (const std::size_t lineSize = end - start; lineSize > 0) {
        auto methodFunc = [this]<typename T>(T &&PH1) { setMethod(std::forward<decltype(PH1)>(PH1)); };
        start = splitRequestLine(start, end, " ", methodFunc);
        auto urlFunc = [this]<typename T>(T &&PH1) { seturl(std::forward<decltype(PH1)>(PH1)); };
        start = splitRequestLine(start, end, " ", urlFunc);
        auto versionFunc = [this]<typename T>(T &&PH1) { setVersion(std::forward<decltype(PH1)>(PH1)); };
        splitRequestLine(start, end, nullptr, versionFunc);
        // 为解析请求头做准备
        readBuf->readPosIncrease(lineSize + 2);
        // 修改状态
        setState(PrecessState::ParseReqHeaders);
        return true;
    }
    return false;
}

bool HttpRequest::parseRequestHeader(Buffer *readBuf) {
    if (const char *end = readBuf->findCRLF(); end != nullptr) {
        const char *start = readBuf->data();
        const std::size_t lineSize = end - start;
        // 基于: 搜索字符串
        if (const char *middle = std::search(start, start + lineSize, std::begin(": "), std::end(": "));
            middle != start + lineSize) {
            if (const std::size_t keyLen = middle - start, valueLen = end - middle - 2;
                keyLen > 0 && valueLen > 0) {
                const std::string key(start, keyLen);
                const std::string value(middle + 2, valueLen);
                addHeader(key, value);
            }
            // 移动读数据的位置
            readBuf->readPosIncrease(lineSize + 2);
        }
        else {
            // 请求头被解析完了, 跳过空行
            readBuf->readPosIncrease(2);
            // 修改解析状态
            // 忽略 post 请求, 按照 get 请求处理
            setState(PrecessState::ParseReqDone);
        }
        return true;
    }
    return false;
}

bool HttpRequest::parseHttpRequest(Buffer *readBuf, HttpResponse *response, Buffer *sendBuf,
                                   const int socket) {
    bool flag = true;
    while (m_curState != PrecessState::ParseReqDone) {
        switch (m_curState) {
            case PrecessState::ParseReqLine:
                flag = parseRequestLine(readBuf);
                break;
            case PrecessState::ParseReqHeaders:
                flag = parseRequestHeader(readBuf);
                break;
            case PrecessState::ParseReqBody:
                [] {
                }(); // 这里写个空lambda防止CLion报warning
                break;
            default:
                break;
        }
        if (!flag) {
            return flag;
        }
        // 判断是否解析完毕了, 如果完毕了, 需要准备回复的数据
        if (m_curState == PrecessState::ParseReqDone) {
            // 1. 根据解析出的原始数据, 对客户端的请求做出处理
            processHttpRequest(response);
            // 2. 组织响应数据并发送给客户端
            response->prepareMsg(sendBuf, socket);
        }
    }
    m_curState = PrecessState::ParseReqLine; // 状态还原, 保证还能继续处理第二条及以后的请求
    return flag;
}

bool HttpRequest::processHttpRequest(HttpResponse *response) {
    if (strcasecmp(m_method.data(), "get") != 0) {
        return false;
    }
    m_url = decodeMsg(m_url);
    // 处理客户端请求的静态资源(目录或者文件)
    const char *file;
    // 根目录为main.cpp中设置的目录
    if (strcmp(m_url.data(), "/") == 0) {
        // 访问根目录
        file = "./";
    }
    else {
        // 跳过 '/' 字符相对目录
        file = m_url.data() + 1;
    }
    // 获取文件属性
    struct stat st{};

    if (const int ret = stat(file, &st); ret == -1) {
        // 文件不存在 -- 回复404
        //sendHeadMsg(cfd, 404, "Not Found", getFileType(".html"), -1);
        //sendFile("404.html", cfd);
        response->setFileName("404.html");
        response->setStatusCode(StatusCode::NotFound);
        // 响应头
        response->addHeader("Content-type", getFileType(".html"));
        response->sendDataFunc = sendFile;
        return false;
    }

    response->setFileName(file);
    response->setStatusCode(StatusCode::OK);
    // 判断文件类型
    if (S_ISDIR(st.st_mode)) {
        // 把这个目录中的内容发送给客户端
        // 响应头
        response->addHeader("Content-type", getFileType(".html"));
        response->sendDataFunc = sendDir;
    }
    else {
        // 把文件的内容发送给客户端
        // 响应头
        response->addHeader("Content-type", getFileType(file));
        response->addHeader("Content-length", std::to_string(st.st_size));
        response->sendDataFunc = sendFile;
    }

    return true;
}

std::string HttpRequest::decodeMsg(const std::string &msg) {
    std::string str{};
    for (const char *from = msg.data(); *from != '\0'; ++from) {
        // isxdigit -> 判断字符是不是16进制格式, 取值在 0-f
        // Linux%E5%86%85%E6%A0%B8.jpg
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {
            // 将16进制的数 -> 十进制 将这个数值赋值给了字符 int -> char
            // B2 == 178
            // 将3个字符, 变成了一个字符, 这个字符就是原始数据
            str.append(1, static_cast<char>(hexToDec(from[1]) * 16 + hexToDec(from[2])));

            // 跳过 from[1] 和 from[2] 因此在当前循环中已经处理过了
            from += 2;
        }
        else {
            // 字符拷贝, 赋值
            str.append(1, *from);
        }
    }
    str.append(1, '\0');
    return str;
}

std::string HttpRequest::getFileType(const std::string_view name) {
    if (name.find_last_of('.') == std::string_view::npos)
        return "text/plain; charset=utf-8"; // 纯文本

    const auto dot = name.substr(name.find_last_of('.'));

    if (dot == ".html" || dot == ".htm")
        return "text/html; charset=utf-8";
    if (dot == ".jpg" || dot == ".jpeg")
        return "image/jpeg";
    if (dot == ".gif")
        return "image/gif";
    if (dot == ".png")
        return "image/png";
    if (dot == ".css")
        return "text/css";
    if (dot == ".au")
        return "audio/basic";
    if (dot == ".wav")
        return "audio/wav";
    if (dot == ".avi")
        return "video/x-msvideo";
    if (dot == ".mov" || dot == ".qt")
        return "video/quicktime";
    if (dot == ".mpeg" || dot == ".mpe")
        return "video/mpeg";
    if (dot == ".vrml" || dot == ".wrl")
        return "model/vrml";
    if (dot == ".midi" || dot == ".mid")
        return "audio/midi";
    if (dot == ".mp3")
        return "audio/mpeg";
    if (dot == ".ogg")
        return "application/ogg";
    if (dot == ".pac")
        return "application/x-ns-proxy-autoconfig";
    if (dot == ".mp4")
        return "video/mp4";
    return "text/plain; charset=utf-8";
}

void HttpRequest::sendDir(const std::string &dirName, Buffer *sendBuf, const int cfd) {
    char buf[4096]{};
    sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName.data());
    struct dirent **namelist;
    const int num = scandir(dirName.data(), &namelist, nullptr, alphasort);
    for (int i = 0; i < num; ++i) {
        // 取出文件名 namelist 指向的是一个指针数组 struct dirent* tmp[]
        char *name = namelist[i]->d_name;
        struct stat st{};
        char subPath[1024] = {0};
        sprintf(subPath, "%s/%s", dirName.data(), name);
        stat(subPath, &st);
        if (S_ISDIR(st.st_mode)) {
            // a标签 <a href="">name</a>
            sprintf(buf + strlen(buf),
                    "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>",
                    name, name, st.st_size);
        }
        else {
            sprintf(buf + strlen(buf),
                    "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
                    name, name, st.st_size);
        }
        // send(cfd, buf, strlen(buf), 0);
        sendBuf->appendString(buf);
#ifndef MSG_SEND_AUTO
        sendBuf->sendData(cfd);
#endif
        memset(buf, 0, sizeof(buf));
        free(namelist[i]);
    }
    sprintf(buf, "</table></body></html>");
    // send(cfd, buf, strlen(buf), 0);
    sendBuf->appendString(buf);
#ifndef MSG_SEND_AUTO
    sendBuf->sendData(cfd);
#endif
    free(namelist);
}

void HttpRequest::sendFile(const std::string &fileName, Buffer *sendBuf, const int cfd) noexcept {
    // 1. 打开文件
    const int fd = open(fileName.data(), O_RDONLY);
    assert(fd > 0);
    while (true) {
        char buf[1024];
        if (const ssize_t len = read(fd, buf, sizeof buf); len > 0) {
            sendBuf->appendString(buf, len);
            sendBuf->sendData(cfd);
        }
        else if (len == 0) {
            break;
        }
        else {
            close(fd);
            perror("read");
        }
    }
    close(fd);
}

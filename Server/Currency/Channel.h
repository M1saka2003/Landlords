#ifndef SERVER_CHANNEL
#define SERVER_CHANNEL

#include <functional>

// 定义文件描述符的读写事件
enum class [[maybe_unused]]FDEvent {
    TimeOut = 0x01,
    ReadEvent = 0x02,
    WriteEvent = 0x04
};

// 可调用对象包装器打包的是什么? 1. 函数指针 2. 可调用对象(可以向函数一样使用)
// 最终得到了地址, 但是没有调用
class Channel {
public:
    using handleFunc = std::function<int(void *)>;

    Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void *arg);

    // 回调函数
    handleFunc readCallback;
    handleFunc writeCallback;
    handleFunc destroyCallback;

    // 修改fd的写事件(检测 or 不检测)
    void writeEventEnable(bool flag);

    // 判断是否需要检测文件描述符的写事件
    [[maybe_unused]][[nodiscard]] bool isWriteEventEnable() const;

    // 取出私有成员的值
    [[nodiscard]] int getEvent() const {
        return m_events;
    }

    [[nodiscard]] int getSocket() const {
        return m_fd;
    }

    [[nodiscard]] const void *getArg() const {
        return m_arg;
    }

private:
    // 文件描述符
    int m_fd;
    // 事件
    int m_events;
    // 回调函数的参数
    void *m_arg;
};

#endif

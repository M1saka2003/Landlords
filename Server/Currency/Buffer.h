#ifndef SERVER_BUFFER
#define SERVER_BUFFER

#include <string>

class Buffer {
public:
    explicit Buffer(std::size_t size);

    ~Buffer();

    // 扩容
    void extendRoom(std::size_t size);

    // 得到剩余的可写的内存容量
    [[nodiscard]] std::size_t writeableSize() const {
        return m_capacity - m_writePos;
    }

    // 得到剩余的可读的内存容量
    [[nodiscard]] std::size_t readableSize() const {
        return m_writePos - m_readPos;
    }

    // 写内存 1. 直接写 2. 接收套接字数据
    int appendString(const char *data, std::size_t size);

    int appendString(const char *data);

    int appendString(const std::string &data);

    int socketRead(int fd);

    // 根据\r\n取出一行, 找到其在数据块中的位置, 返回该位置
    [[nodiscard]] char *findCRLF() const;

    // 发送数据
    std::size_t sendData(int socket); // 指向内存的指针
    // 得到读数据的起始位置
    [[nodiscard]] char *data() const {
        return m_data + m_readPos;
    }

    std::size_t readPosIncrease(const std::size_t count) {
        m_readPos += count;
        return m_readPos;
    }

private:
    char *m_data;
    std::size_t m_capacity;
    std::size_t m_readPos = 0;
    std::size_t m_writePos = 0;
};

#endif

#include "Buffer.h"
#include <cstdlib>
#include <sys/uio.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <thread>

Buffer::Buffer(std::size_t size) : m_capacity(size) {
    m_data = reinterpret_cast<char *>( std::malloc(size));
    std::memset(m_data, 0, size);
}

Buffer::~Buffer() {
    std::free(m_data);
}

void Buffer::extendRoom(std::size_t size) {
    // 1. 内存够用 - 不需要扩容
    if (writeableSize() >= size) {
        return;
    }
        // 2. 内存需要合并才够用 - 不需要扩容
        // 剩余的可写的内存 + 已读的内存 > size
    else if (m_readPos + writeableSize() >= size) {
        // 得到未读的内存大小
        std::size_t readable = readableSize();
        // 移动内存
        std::copy_n(m_data + m_readPos, readable, m_data);
        // 更新位置
        m_readPos = 0;
        m_writePos = readable;
    }
        // 3. 内存不够用 - 扩容
    else {
        void *temp = realloc(m_data, m_capacity + size);
        if (temp == nullptr) {
            return; // 失败了
        }
        std::memset(reinterpret_cast<char *>(temp) + m_capacity, 0, size);
        // 更新数据
        m_data = reinterpret_cast<char *>(temp);
        m_capacity += size;
    }
}

int Buffer::appendString(const char *data, std::size_t size) {
    if (data == nullptr || size <= 0) {
        return -1;
    }
    // 扩容
    extendRoom(size);
    // 数据拷贝
    std::memcpy(m_data + m_writePos, data, size);
    m_writePos += size;
    return 0;
}

int Buffer::appendString(const char *data) {
    std::size_t size = std::strlen(data);
    int ret = appendString(data, size);
    return ret;
}

int Buffer::appendString(const std::string &data) {
    int ret = appendString(data.data());
    return ret;
}

int Buffer::socketRead(int fd) {
    struct iovec vec[2];
    // 初始化数组元素
    std::size_t writeable = writeableSize();
    vec[0].iov_base = m_data + m_writePos;
    vec[0].iov_len = writeable;
    char *tmp_buf = new char[40960];
    vec[1].iov_base = tmp_buf;
    vec[1].iov_len = 40960;
    ssize_t result = readv(fd, vec, 2);
    if (result == -1) {
        return -1;
    }
    else if (result <= writeable) {
        m_writePos += static_cast<int>(result);
    }
    else {
        m_writePos = m_capacity;
        appendString(tmp_buf, static_cast<int>(result - writeable));
    }
    delete[](tmp_buf);

    return static_cast<int>(result);
}

char *Buffer::findCRLF() {
    char *ptr = reinterpret_cast<char*> (memmem(m_data + m_readPos, readableSize(), "\r\n", 2));
    return ptr;
}

std::size_t Buffer::sendData(int socket) {
    // 判断有无数据
    std::size_t readable = readableSize();
    if (readable > 0) {
        int count = static_cast<int>(send(socket, m_data + m_readPos, readable, MSG_NOSIGNAL));
        if (count > 0) {
            m_readPos += count;
            usleep(1);
        }
        return count;
    }
    return 0;
}


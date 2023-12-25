#include <unistd.h>
#include "TcpServer.h"

int main() {
    unsigned short port = 12345;
    chdir("/home/m1saka/test");
    // 启动服务器
    auto *server = new TcpServer(port, 4);
    server->run();
    return 0;
}
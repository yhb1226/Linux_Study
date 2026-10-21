#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

int main(void)
{
    // 网络基本配置
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    typedef struct sockaddr_in SockAddrIn;
    SockAddrIn severaddr;
    memset(&severaddr, 0, sizeof(severaddr));

    severaddr.sin_family = AF_INET;
    severaddr.sin_port = htons(2049);
    severaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&severaddr, sizeof(severaddr)) == -1)
    {
        perror("bind");
        return -1;
    }

    listen(sock_fd, 10); // 监听有没有客户端申请连接

    typedef struct sockaddr_in ClientAddr;
    ClientAddr clientaddr;
    socklen_t len;
    int clinet_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len);

    char buffer[128] = {0};
    int count = recv(clinet_fd, buffer, sizeof(buffer), 0);

    send(clinet_fd, buffer, sizeof(buffer), 0);
}
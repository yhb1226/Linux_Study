#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void)
{
    // 网络基本配置
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    typedef struct sockaddr_in SockAddrIn;
    SockAddrIn severaddr;
    memset(&severaddr, NULL, sizeof(severaddr));

    severaddr.sin_family = AF_INET;
    severaddr.sin_port = htons(2048);
    severaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_fd, (struct sockaddr *)&severaddr, sizeof(severaddr)) == -1)
    {
        perror("bind");
        return -1;
    }

    listen(sock_fd, 10);

// select承接多客户端
#if 1

// poll承接多客户端
#else

#endif

    return 0;
}

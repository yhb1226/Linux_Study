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

// select多客户端
#if 0

    fd_set rfds, rset;

    FD_ZERO(&rfds);
    FD_SET(sock_fd, &rfds);
    int maxfd = sock_fd;

    printf("ready\n");

    while (1)
    {
        rset = rfds;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        // 铃响有客，发编号
        if (FD_ISSET(sock_fd, &rset)) // 查是否有新客户端
        {
            typedef struct sockaddr_in ClientAddr;
            ClientAddr clientaddr;
            socklen_t len = sizeof(clientaddr);
            int client_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len); // 接收新的客户端

            // fd更新
            FD_SET(client_fd, &rfds);
            maxfd = client_fd;
        }

        // 客提要求，收数据
        for (int i = sock_fd + 1; i <= maxfd; i++) // i->fd的编号值
        {
            if (FD_ISSET(i, &rset)) // 判断有无数据发送
            {
                char buffer[128] = {0};
                int count = recv(i, buffer, sizeof(buffer), 0);

                if (count == 0)
                {
                    printf("disconnect");
                    /*

                    为什么需要都关闭？
                    1.close(i);
                    2.FD_CLR(i, &rfds);
                    3.FD_CLR(i, &rfds);
                      close(i);

                    */
                    FD_CLR(i, &rfds); // 从监听名单除名
                    close(i);         // 关闭fd释放内核资源
                    continue;
                }

                send(i, buffer, sizeof(buffer), 0); // 回传
                printf("client_fd:%d,count:%d,buffer:%s\n", i, count, buffer);
            }
        }
    }

// poll多客户端
#else
    struct pollfd fds[100] = {0};
    fds[sock_fd].fd = sock_fd;    // 把fds数组的第sock_fd位置装着sock_fd的编号
    fds[sock_fd].events = POLLIN; // 只读

    int maxfd = sock_fd;
    printf("ready\n");

    while (1)
    {

        int nready = poll(fds, maxfd + 1, -1); // 阻塞一直等待有新事件到来(遍历 fds 里每个 fd，检查它现在有没有事件)

        if (fds[sock_fd].revents & POLLIN) // 监听 socket 可读 = 有新连接
        {
            typedef struct sockaddr_in ClientAddr;
            ClientAddr clientaddr;

            socklen_t len = sizeof(clientaddr);
            int client_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len); // 取出一个已经建立的连接，返回新的通信fd
            printf("Client:%d", client_fd);

            fds[client_fd].fd = client_fd;
            fds[client_fd].events = POLLIN;
            maxfd = client_fd;
        }

        for (int i = sock_fd + 1; i <= maxfd; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[128] = {0};
                int count = recv(i, buffer, sizeof(buffer), 0);
                if (count == 0)
                {
                    printf("disconnect\n");

                    fds[i].events = -1;
                    fds[i].fd = 0;
                    close(i);
                }

                send(i, buffer, sizeof(buffer), 0);
                printf("client:%d,count:%d,buffer:%s\n", i, count, buffer);
            }
        }
    }

#endif

    return 0;
}

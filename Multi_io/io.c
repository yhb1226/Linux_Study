#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/epoll.h>

int main(void)
{
    // 网络基本配置
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0); // sock_fd不变

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
#elif 0
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

                    fds[i].fd = -1;    // 移除断开连接的客户端fd
                    fds[i].events = 0; // 清空事件
                    close(i);          // 防止内存泄露
                }

                send(i, buffer, sizeof(buffer), 0);
                printf("client:%d,count:%d,buffer:%s\n", i, count, buffer);
            }
        }
    }

    */
// 为什么client是从5开始的，难道不是应该从4开始吗
#else
    int epfd = epoll_create(1);

    typedef struct epoll_event EV;
    EV ev;

    ev.data.fd = sock_fd;
    ev.events = POLLIN; // 事件选择可读事件

    epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &ev);
    struct epoll_event events[1024] = {0}; // 所有客户端总共一起最多处理1024个事件

    printf("ready accept\n");
    while (1)
    {
        int nepoll = epoll_wait(epfd, events, 1024, -1); // 等待事件到来

        for (int i = 0; i < nepoll; i++)
        {
            if (events[i].data.fd == sock_fd) // 首先判断是否有新客户端加入进来
            {
                typedef struct sockaddr_in ClientAddr;
                ClientAddr clientaddr;
                socklen_t len;

                int client_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len);

                ev.data.fd = client_fd;
                ev.events = EPOLLIN | EPOLLET;

                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
            }
            else if (events[i].events & EPOLLIN)
            {
                char buffer[10] = {0};
                int count = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (count == 0)
                {
                    printf("disconnect\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL); // 第四个参数是什么，为什么和上面传输的第四个参数不一样
                    close(events[i].data.fd);                                // 为什么此处还需要关闭i，这个地方的i占据了什么位置
                    continue;
                }
                send(events[i].data.fd, buffer, sizeof(buffer), 0);
                printf("client:%d,count:%d,buffer:%s\n", events[i].data.fd, count, buffer);
            }
        }
    }

#endif

        getchar();
}

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
/*
    1.代码出现大量time_wait原因
    2.多个客户端连接同一个服务端
*/
int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(2048);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
    {
        perror("bind");
        return -1;
    }

    listen(sockfd, 10); // 等待客户端连接，设置连接个数缓冲区

    struct sockaddr_in clientaddr; // 接收客户端的信息
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
    printf("accept\n");

#if 1
    char buffer[128] = {0};                     // 接收客户端发送来的数据和完整发回去
    int count = recv(clientfd, buffer, 128, 0); // recv显示0时候是显示客户端已经关闭
    send(clientfd, buffer, count, 0);
    printf("sockfd: %d, clientfd: %d, count: %d, buffer: %s\n", sockfd, clientfd, count, buffer);
#else
    while (1)
    {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        send(clientfd, buffer, count, 0);
        printf("sockfd: %d, clientfd: %d, count: %d, buffer: %s\n", sockfd, clientfd, count, buffer);
    }
#endif
    getchar();
    close(clientfd); // close()放到getchar前后有什么区别
}
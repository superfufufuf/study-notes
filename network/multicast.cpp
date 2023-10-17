#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <thread>
#include <iostream>

#include "common.h"


/// @brief
/// @return
int main(int argc, char **argv)
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int errorId = 0;
    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    printf("socket: %d\n", fd);
    // 2. 通信的套接字和本地的IP与端口绑定
    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(MULTICAST_PORT); // 大端
    bindAddr.sin_addr.s_addr = INADDR_ANY;     // 0.0.0.0
    int ret = bind(fd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));
    if (ret == -1)
    {
        perror("bind");
        exit(0);
    }
    // TODO::
    // 3. 加入到多播组
    struct ip_mreqn opt;
    // 要加入到哪个多播组, 通过组播地址来区分
    // inet_pton(AF_INET, "239.0.1.10", &opt.imr_multiaddr.s_addr);
    inet_pton(AF_INET, MULTICAST_IP, &opt.imr_multiaddr.s_addr);
    opt.imr_address.s_addr = INADDR_ANY;
    opt.imr_ifindex = if_nametoindex(ifName);
    errorId = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &opt, sizeof(opt));
    printf("add muti :%d, ip: %s, port: %d\n", errorId, MULTICAST_IP, MULTICAST_PORT);

    struct sockaddr_in sendAddr;
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(MULTICAST_PORT); // 大端
    sendAddr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
    // 设置多播TTL参数
    unsigned char ttl = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        printf("Error setting multicast TTL\n");
        return -1;
    }

    // 3. 通信
    std::thread([fd]()
                {
        char buf[1024];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        while (true)
        {
            // 接收广播消息
            memset(buf, 0, sizeof(buf));
            // 阻塞等待数据达到
            int size = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_len);
            printf("接收到的组播消息: %s, size: %d, 发送者: %s\n", buf, int(strlen(buf)), inet_ntoa(client_addr.sin_addr));
        } })
        .detach();

    char buf[1024];
    while (true)
    {
        // 发送广播消息
        memset(buf, 0, sizeof(buf));
        scanf("%s", buf);
        if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&sendAddr, sizeof(sendAddr)) < 0)
        {
            printf("Error sending data\n");
            return -1;
        }
        printf("发送的组播消息: %s, size: %d\n", buf, int(strlen(buf)));
    }

    close(fd);

    return 0;
}

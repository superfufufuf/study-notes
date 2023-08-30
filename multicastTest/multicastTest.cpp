#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

/// @brief 
/// @return 
int main(int argc, char **argv)
{
    // 1. 创建通信的套接字
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    int errorId = 0;
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }


    printf("socket: %d\n", fd);
    // 2. 通信的套接字和本地的IP与端口绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(30492);    // 大端
    addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }
    // TODO::
    // 3. 加入到多播组
    struct ip_mreqn opt;
    // 要加入到哪个多播组, 通过组播地址来区分
    //inet_pton(AF_INET, "239.0.1.10", &opt.imr_multiaddr.s_addr);
    inet_pton(AF_INET, "224.224.224.245", &opt.imr_multiaddr.s_addr);
    opt.imr_address.s_addr = INADDR_ANY;
    // char ifName[] = "eth0";
    char ifName[] = "enp0s31f6";
    opt.imr_ifindex = if_nametoindex(ifName);
    errorId = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &opt, sizeof(opt));
    printf("add muti :%d, ip: %s, port: %d\n", errorId,"224.224.224.245", 30492);

    char buf[1024];
    // 3. 通信
    while(1)
    {
        // 接收广播消息
        memset(buf, 0, sizeof(buf));
        // 阻塞等待数据达到
        int size = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("接收到的组播消息: %s, size %d\n", buf, size);
    }

    close(fd);

    return 0;
}

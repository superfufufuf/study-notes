#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define PACKET_SIZE 4096
// 效验算法（百度下有注释，但是还是看不太明白）
unsigned short cal_chksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

// Ping函数
bool ping(const char *ips, int timeout)
{
    struct timeval *tval;
    int maxfds = 0;
    fd_set readfds;

    struct sockaddr_in addr;
    struct sockaddr_in from;
    // 设定Ip信息
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = inet_addr(ips);

    int sockfd;
    // 取得socket  。  如果没加sudo 这里会报错
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        std::cout << "ip:%s,socket errorn" << ips << std::endl;
        return false;
    }

    struct timeval timeo;
    // 设定TimeOut时间
    timeo.tv_sec = timeout / 1000;
    timeo.tv_usec = (timeout % 1000) * 1000;

    // if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
    // {
    //     std::cout << "ip:%s,setsockopt errorn" << ips << std::endl;
    //     return false;
    // }

    char sendpacket[PACKET_SIZE];
    char recvpacket[PACKET_SIZE];
    // 设定Ping包
    memset(sendpacket, 0, sizeof(sendpacket));

    pid_t pid;
    // 取得PID，作为Ping的Sequence ID
    pid = getpid();
    pid = uint16_t(pid);

    std::cout << "pid:" << pid << std::endl;

    struct ip *iph;
    struct icmp *icmp;

    icmp = (struct icmp *)sendpacket;
    icmp->icmp_type = ICMP_ECHO; // 回显请求
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = 0;
    icmp->icmp_id = pid;
    tval = (struct timeval *)icmp->icmp_data;
    gettimeofday(tval, NULL);
    icmp->icmp_cksum = cal_chksum((unsigned short *)icmp, sizeof(struct icmp)); // 校验

    int n;
    // 发包 。可以把这个发包挪到循环里面去。
    n = sendto(sockfd, (char *)&sendpacket, sizeof(struct icmp), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (n < 1)
    {
        std::cout << "ip:%s,sendto errorn" << ips << std::endl;
        return false;
    }

    // 接受
    // 由于可能接受到其他Ping的应答消息，所以这里要用循环
    while (1)
    {
        std::cout << "start loop" << std::endl;
        // 设定TimeOut时间，这次才是真正起作用的
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfds = sockfd + 1;
        n = select(maxfds, &readfds, NULL, NULL, &timeo);
        if (n <= 0)
        {
            std::cout << "ip:" << ips << ",Time out errorn" << std::endl;
            close(sockfd);
            return false;
        }
        std::cout << "checked change, remaining time:" << timeo.tv_sec << "." << std::setw(6) << std::setfill('0') << timeo.tv_usec << "s" << std::endl;

        // 接受
        memset(recvpacket, 0, sizeof(recvpacket));
        int fromlen = sizeof(from);
        n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen);
        std::cout << "recvfrom Len:" << n << std::endl;
        if (n < 1)
        {
            close(sockfd);
            return false;
        }
        std::cout << "recv data" << std::endl;

        char *from_ip = (char *)inet_ntoa(from.sin_addr);
        // 判断是否是自己Ping的回复
        if (strcmp(from_ip, ips) != 0)
        {
            std::cout << "NowPingip:" << ips << " Fromip:" << from_ip << " NowPingip is not same to Fromip,so continue." << std::endl;
            continue;
        }

        iph = (struct ip *)recvpacket;

        icmp = (struct icmp *)(recvpacket + (iph->ip_hl << 2));

        std::cout << "ip:" << ips << ",icmp->icmp_type:" << int(icmp->icmp_type) << ",icmp->icmp_id:" << icmp->icmp_id << std::endl;
        // 判断Ping回复包的状态
        if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid) // ICMP_ECHOREPLY回显应答
        {
            // 正常就退出循环
            std::cout << "icmp succecss .............  " << std::endl;
            close(sockfd);
            break;
        }
        else
        {
            // 否则继续等
            continue;
        }
    }

    close(sockfd);
    return true;
}
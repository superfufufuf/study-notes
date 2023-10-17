#ifndef NETWORK_TEST_COMMON_H
#define NETWORK_TEST_COMMON_H

#include <iostream>
#include <mutex>
#include <algorithm>
#include <string>
#include <list>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

#define SERVER_PORT 6789
#define BUFF_LEN 1024
#define SERVER_IP "10.28.9.187"

char ifName[] = "eth0";
#define MULTICAST_IP "224.244.224.246"
#define MULTICAST_PORT 30489

std::list<std::string> get_local_ip()
{
    static std::list<std::string> allIpList;
    static std::once_flag flag;

    std::call_once(flag, [&]()
                   {
        int i = 0;
        struct ifconf ifc;
        char buf[1024] = {0};
        char ipbuf[20] = {0};
        struct ifreq *ifr;

        ifc.ifc_len = 1024;
        ifc.ifc_buf = buf;

        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            std::cout << "get sockfd failed." << std::endl;
            return;
        }

        ioctl(sockfd, SIOCGIFCONF, &ifc);
        ifr = (struct ifreq *)buf;

        int ipCounts = ifc.ifc_len / sizeof(struct ifreq);
        for (i = 0 ; i < ipCounts; i++)
        {
            memset(ipbuf, 0, sizeof(ipbuf));
            inet_ntop(AF_INET, &((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr, ipbuf, 20);
            ifr = ifr + 1;
            allIpList.push_back(std::string(ipbuf));
            std::cout << "ip[" << i << "]: " << ipbuf << std::endl;
        } });
    return allIpList;
}

bool isLocalIp(const std::string &_ip)
{
    auto ipList = get_local_ip();
    auto iter = std::find(ipList.cbegin(), ipList.cend(), _ip);
    return (iter != ipList.cend());
}

#endif
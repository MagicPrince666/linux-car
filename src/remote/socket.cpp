#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "remote.h"
#include "socket.h"
#include "xepoll.h"

UdpServer::UdpServer(std::string dev, bool debug) : RemoteProduct(dev, debug) {}

UdpServer::~UdpServer()
{
    if (socket_fd_ > 0) {
        MY_EPOLL.EpollDel(socket_fd_);
        close(socket_fd_);
    }
}

int UdpServer::Init()
{
    struct sockaddr_in servaddr;

    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd_ < 0) {
        perror("Create socket error");
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT);

    bind(socket_fd_, (struct sockaddr *)&servaddr, sizeof(servaddr));

    MY_EPOLL.EpollAdd(socket_fd_, std::bind(&UdpServer::GetClientData, this));
    return 0;
}

int UdpServer::GetClientData()
{
    char str[INET_ADDRSTRLEN];
    char buf[MAXLINE];
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;
    cliaddr_len = sizeof(cliaddr);
    int btyes   = recvfrom(socket_fd_, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len);
    if (btyes == -1) {
        perror("recvfrom error");
    }
    printf("Recvfrom %s port %d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));

    return 0;
}

bool UdpServer::Request(struct RemoteState &data)
{
    memset((int8_t *)&data, 0, sizeof(data));
    data.lose_signal = false; // 失控标识
    // data.adslx      = rc_data_.percent[3];  // 左摇杆x轴
    // data.adsly      = rc_data_.percent[1];  // 左摇杆y轴
    // data.adsrx      = rc_data_.percent[0];  // 右摇杆x轴
    // data.adsry      = rc_data_.percent[2];  // 右摇杆y轴
    // data.ads[12]    = {0};   // 扩展通道 sbus 16路通道都是模拟量
    // memcpy(data.ads, rc_data_.percent + 4, 12);
    return false;
}

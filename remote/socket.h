/**
 * @file socket.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 网络通讯,通过udp接收控制指令
 * @version 0.1
 * @date 2023-01-14
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "RemoteFactory.h"
#include <iostream>

#define MAXLINE 80
#define SERV_PORT 8080

class UdpServer : public RemoteProduct
{
public:
    UdpServer(std::string dev, bool debug = false);
    ~UdpServer();

    int Init();
    virtual bool Request(struct RemoteState &data);

private:
    int GetClientData();
    int socket_fd_;
};

// 生产udp遥控工厂
class UdpRemote : public RemoteFactory
{
public:
    RemoteProduct *CreateRemoteProduct()
    {
        return new UdpServer("/dev/input/event0", false);
    }
};

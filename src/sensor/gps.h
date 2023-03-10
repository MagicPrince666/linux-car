/**
 * @file gps.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 获取GPS数据，定位设备所在位置，对时等
 * @version 0.1
 * @date 2023-01-14
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <iostream>

#define DATA_LEN 0xFF

class Gps
{
public:
    Gps(std::string dev = "/dev/ttyS2");
    ~Gps();

    int Posttioning();

private:
    std::string device_;
    int uart_fd_;
    int OpenSerial();
};

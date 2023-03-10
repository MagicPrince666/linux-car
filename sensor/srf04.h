/**
 * @file srf04.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 超声波测距
 * @version 0.1
 * @date 2023-01-12
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>

class Srf04 {
public:
    Srf04(std::string dev = "srf04");
    ~Srf04();

    int Srf04Distance();

private:
    std::string device_dir_;
};

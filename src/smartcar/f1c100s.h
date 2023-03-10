/**
 * @file f1c100s.h
 * @author 黄李全 (846863428@qq.com)
 * @brief f1c100s 遥控小车
 * @version 0.1
 * @date 2023-01-11
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "CarFactory.h"

class F1c100s : public CarProduct {
private:

public:
    F1c100s(std::string name);
    ~F1c100s();
    virtual bool Run(bool debug);
};
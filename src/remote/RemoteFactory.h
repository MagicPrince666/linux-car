/**
 * @file RemoteFactory.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 遥控工厂，生产遥控的工厂，设计模式，工厂模式
 * @version 0.1
 * @date 2023-01-13
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "remote.h"
#include <iostream>

// 遥控产品基类
class RemoteProduct
{
public:
    RemoteProduct(std::string dev, bool debug) : device_(dev), debug_(debug) {}
    virtual ~RemoteProduct() {}
    virtual bool Request(struct RemoteState &data) = 0;

protected:
    std::string device_;
    bool debug_;
};

// 遥控工厂基类
class RemoteFactory
{
public:
    virtual RemoteProduct *CreateRemoteProduct() = 0;
};

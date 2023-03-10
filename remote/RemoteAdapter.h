/**
 * @file RemoteAdapter.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 遥控适配器类 设计模式：适配器模式 -- 类适配器
 * @version 0.1
 * @date 2023-01-12
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "RemoteFactory.h"
#include "remote.h"
#include <iostream>
#include <memory>

// 类模式，适配器类，通过public继承获得接口继承的效果，通过private继承获得实现继承的效果
class RemoteClassAdapter : public RemoteTarget, private RemoteProduct
{
public:
    RemoteClassAdapter();
    ~RemoteClassAdapter();
    virtual void RemoteRequest(RemoteState &rc_data); // 实现RemoteTarget定义的Request接口
};

// 对象模式，适配器类，继承RemoteTarget类，采用组合的方式实现RemoteProduct的复用
class SbusRemoteObjAdapter : public RemoteTarget
{
public:
    // SbusRemoteObjAdapter(RemoteProduct *remote);
    SbusRemoteObjAdapter();
    ~SbusRemoteObjAdapter();
    virtual void RemoteRequest(RemoteState &rc_data); // 实现RemoteTarget定义的Request接口
private:
    std::shared_ptr<RemoteProduct> remote_;
    // RemoteProduct *remote_ptr_;
};

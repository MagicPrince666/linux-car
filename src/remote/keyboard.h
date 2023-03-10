/**
 * @file keyboard.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 键盘操作
 * @version 0.1
 * @date 2023-01-14
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */

#pragma once

#include "RemoteFactory.h"
#include <iostream>

class KeyBoard : public RemoteProduct
{
public:
    KeyBoard(std::string path, bool debug = false);
    ~KeyBoard();

    int Init();
    virtual bool Request(struct RemoteState &data);

private:
    std::string device_;
    int GetKeyBoard();
};

// 生产键盘遥控工厂
class KeyBoardRemote : public RemoteFactory
{
public:
    RemoteProduct *CreateRemoteProduct()
    {
        return new KeyBoard("/dev/input/event0", false);
    }
};

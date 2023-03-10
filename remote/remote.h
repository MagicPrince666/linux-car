/**
 * @file remote.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-01-13
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <stdint.h>

struct RemoteState {
    bool lose_signal = false; // 失控标识

    bool front         = false; // 前进按钮
    bool back          = false; // 后退按钮
    bool left          = false; // 左转按钮
    bool right         = false; // 右转按钮
    bool select        = false; // 选择按钮
    bool start         = false; // 开始按钮
    bool l1            = false; // 左顶部按钮1
    bool l2            = false; // 左顶部按钮2
    bool r1            = false; // 右顶部按钮1
    bool r2            = false; // 右顶部按钮2
    bool adl           = false; // 左摇杆按钮
    bool adr           = false; // 右摇杆按钮
    bool triangle      = false; // 三角按钮
    bool quadrilateral = false; // 四边形按钮
    bool rotundity     = false; // 园形按钮
    bool fork          = false; // 叉按钮
    uint8_t adslx      = 50;    // 左摇杆x轴
    uint8_t adsly      = 50;    // 左摇杆y轴
    uint8_t adsrx      = 50;    // 右摇杆x轴
    uint8_t adsry      = 50;    // 右摇杆y轴
    uint8_t ads[12]    = {0};   // 扩展通道 sbus 16路通道都是模拟量
};

/* 目标抽象类RemoteTarget  遥控接口 */
class RemoteTarget
{
public:
    RemoteTarget(){};
    ~RemoteTarget(){};
    virtual void RemoteRequest(RemoteState &rc_data) = 0;

private:
};

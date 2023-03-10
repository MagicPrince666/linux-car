/**
 * @file speed_pid.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 电机PID控制
 * @version 0.1
 * @date 2023-02-05
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include "rotary_encoder.h"

class SpeedPid
{
public:
    SpeedPid();
    ~SpeedPid();

    bool Init();

private:
    RotaryEncoder rotary_encoder_;
};

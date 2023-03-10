/**
 * @file rotary_encoder.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 旋转编码器
 * @version 0.1
 * @date 2023-02-02
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 * 
 * 选用的减速电机减速比1:34,编码器精度 12CPR，及电机一圈有12个脉冲（不包含减速）
 */
#pragma once

#include <iostream>
#include <string>
#include <mutex>

class RotaryEncoder
{
public:
    RotaryEncoder(float reduction_ratio = 34.0, float precision = 12.0);
    ~RotaryEncoder();

    bool Init();

private:
    /**
     * @brief 脉冲计数
     * @return int 
     */
    int GetKey(void);

    /**
     * @brief 初始化定时器
     * @return bool 
     */
    bool InitTimer(void);

    /**
     * @brief 定时器回调
     * @return int 
     */
    int timeOutCallBack();

    /**
     * @brief T法测速
     * @return int 
     */
    int TSpeed(void);

    std::mutex  counter_lock_;
    int rotary_encoder_fd_;
    int timer_fd_;
    float reduction_ratio_; // 减速比
    float precision_;   // 编码器精度
    int32_t velocity_;
};

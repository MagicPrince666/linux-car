/**
 * @file moto.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 通用电机控制
 * @version 0.1
 * @date 2023-01-05
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

#include "gpio.h"
#include "pwm.h"
#include <memory>

class Moto
{
public:
    Moto(void);
    virtual ~Moto(void);
    virtual bool Init(int ena, int enb, PwmPram moto_pwm, PwmPram servo_pwm);
    /**
     * @brief 前进
     * @param percent 速度百分比
     * @return int 
     */
    virtual int GoForward(int percent);

    /**
     * @brief 后退
     * @param percent 速度百分比
     * @return int 
     */
    virtual int GoBack(int percent);

    /**
     * @brief 电机停转
     * @return int 
     */
    virtual int Stop(void);

    /**
     * @brief 舵机控制
     * @param angle 角度 0-90度
     * @return int 
     */
    virtual int SetServo(float angle);

private:
    /**
     * @brief Set the Speed object
     * @param percent 速度百分比
     * @return int 
     */
    int SetSpeed(int percent);

    char setpin[64] = {0};

protected:
    std::shared_ptr<Gpio> moto_ena_;
    std::shared_ptr<Gpio> moto_enb_;
    std::shared_ptr<Pwm> pwm_moto_;
    std::shared_ptr<Pwm> pwm_servo_;
    int32_t moto_duty_cycle_;
};

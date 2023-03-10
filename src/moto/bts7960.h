/**
 * @file bts7960.h
 * @author 黄李全 (846863428@qq.com)
 * @brief BTS7960 驱动代码
 * @version 0.1
 * @date 2023-01-09
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 * 
方法1：
VCC接单片机的5V电源，GND接单片机的GND。
REN与LEN短路井接5V电平，驱动器可以工作。
LPWM,输入PWM信号或高电平电机正转。
RPWM，输入PWM信号或高电平电机反转。
方法2：
vCC接单片机的5V电源，GND接单片机的GND。
REN与LEN短路并接输入PWM信号调速。
LPWM，脚输入5V电平电机正转。
RPWM，脚输入5V电平电机反转。
 */
#pragma once

#include <memory>
#include "gpio.h"
#include "pwm.h"
#include "moto.h"

class Bts7960 : public Moto
{
private:
    std::shared_ptr<Gpio> moto_r_is_; // 正转报警
    std::shared_ptr<Gpio> moto_l_is_; // 反转报警

public:
    Bts7960(int l_is, int r_is);
    ~Bts7960();
    virtual bool Init(int ena, int enb, PwmPram moto_pwm, PwmPram servo_pwm);
};

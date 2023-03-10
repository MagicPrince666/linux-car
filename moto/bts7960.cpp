#include "bts7960.h"
#include <iostream>

Bts7960::Bts7960(int l_is, int r_is)
{
    moto_duty_cycle_ = 40000;
    // 报警接口设置为输入
    if (l_is != -1) {
        moto_l_is_ = std::make_shared<Gpio>(l_is, false);
    }
    if (r_is != -1) {
        moto_r_is_ = std::make_shared<Gpio>(r_is, false);
    }
}

Bts7960::~Bts7960() {}

bool Bts7960::Init(int ena, int enb, PwmPram moto_pwm, PwmPram servo_pwm)
{
    // 控制方式 bts7960控制 方法2

    // 正反转控制
    if (ena != -1) {
        moto_ena_ = std::make_shared<Gpio>(ena, true);
        moto_ena_->SetGpioValue(true);
    }

    if (enb != -1) {
        moto_enb_ = std::make_shared<Gpio>(enb, true);
        moto_enb_->SetGpioValue(true);
    }

    pwm_moto_  = std::make_shared<Pwm>(moto_pwm);  // pwm0 25KHz 1/25000 * 1000000000 = 40000
    pwm_servo_ = std::make_shared<Pwm>(servo_pwm); // pwm1 20ms周期 舵机控制

    std::cout << "Init bts7960 gpio and pwm" << std::endl;
    return true;
}

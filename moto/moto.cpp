#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "moto.h"

Moto::Moto(void) {
    moto_duty_cycle_ = 40000;
}

Moto::~Moto(void)
{
    Stop();
    SetServo(45.0); // 回中
    std::cout << "Disable moto" << std::endl;
}

bool Moto::Init(int ena, int enb, PwmPram moto_pwm, PwmPram servo_pwm)
{
    moto_ena_ = std::make_shared<Gpio>(ena, true);                    // PE3
    moto_enb_ = std::make_shared<Gpio>(enb, true);                    // PE4
    moto_ena_->SetGpioValue(true);
    moto_enb_->SetGpioValue(true);

    pwm_moto_  = std::make_shared<Pwm>(moto_pwm);   // pwm0 25KHz 1/25000 * 1000000000 = 40000
    pwm_servo_ = std::make_shared<Pwm>(servo_pwm);  // pwm1 20ms周期

    std::cout << "Init moto gpio and pwm" << std::endl;
    return true;
}

int Moto::SetSpeed(int percent)
{
    if(percent > 100) {
        return -1;
    }

    int duty_cycle = (float)percent/100.0 * moto_duty_cycle_;

    pwm_moto_->PwmDutyCycle(duty_cycle);
    return duty_cycle;
}

int Moto::GoForward(int percent)
{
    SetSpeed(0);
    moto_ena_->SetGpioValue(false);
    moto_enb_->SetGpioValue(true);
    return SetSpeed(percent);
}

int Moto::GoBack(int percent)
{
    SetSpeed(0);
    moto_ena_->SetGpioValue(true);
    moto_enb_->SetGpioValue(false);
    return SetSpeed(percent);
}

int Moto::Stop(void)
{
    moto_ena_->SetGpioValue(true);
    moto_enb_->SetGpioValue(true);
    return SetSpeed(0);
}

int Moto::SetServo(float angle)
{
    int32_t duty_cycle = 1000000 + (angle/90.0) * 1000000;
    if(pwm_servo_) {
        pwm_servo_->PwmDutyCycle(duty_cycle);
    }
    return 0;
}

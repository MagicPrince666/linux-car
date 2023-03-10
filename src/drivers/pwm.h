/**
 * @file pwm.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-05
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

struct PwmPram {
    int chip = 0;
    int channel = 0;
    bool polarity = true;
    int period = 0;
    int dutycycle = 0;
};

class Pwm
{
public:
    Pwm(PwmPram parm);
    ~Pwm();
    int SetupPwm();
    int PwmPolarity(bool value);
    int PwmPeriod(int value);
    int PwmDutyCycle(int value);

private:
    int UnExportPwm();
    int PwmEnable(bool enable);
    char setpin_[64] = {0};
    int pwm_chip_;      // pwm控制器
    int pwm_channel_;   // pwm通道
};


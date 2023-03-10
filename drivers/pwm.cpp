#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "pwm.h"

Pwm::Pwm(PwmPram parm) {
    pwm_chip_ = parm.chip;
    pwm_channel_ = parm.channel;
    SetupPwm();
    PwmPeriod(parm.period);
    PwmDutyCycle(parm.dutycycle);
    PwmPolarity(parm.polarity);
    PwmEnable(true);
}

Pwm::~Pwm() {
    PwmEnable(false);
    UnExportPwm();
}

int Pwm::SetupPwm()
{
    int len = snprintf(setpin_, sizeof(setpin_) - 1, "/sys/class/pwm/pwmchip%d/export", pwm_chip_);
    setpin_[len] = 0;
    FILE *set_export = fopen(setpin_, "w");
    if (set_export == nullptr) {
        std::cout << "Can\'t open " << setpin_ << std::endl;
        return -1;
    } else {
        std::cout << "Open " << setpin_ << std::endl;
        fprintf(set_export, "%d", pwm_channel_);
    }
    fclose(set_export);
    return 0;
}

int Pwm::UnExportPwm()
{
    int len = snprintf(setpin_, sizeof(setpin_) - 1, "/sys/class/pwm/pwmchip%d/unexport", pwm_chip_);
    setpin_[len] = 0;
    FILE *set_export = fopen(setpin_, "w");
    if (set_export == nullptr) {
        std::cout << "Can\'t open " << setpin_ << std::endl;
    } else {
        fprintf(set_export, "%d", pwm_channel_);
    }
    assert(set_export != nullptr);
    fclose(set_export);
    return 0;
}

int Pwm::PwmEnable(bool enable)
{
    int len = snprintf(setpin_, sizeof(setpin_) - 1, "/sys/class/pwm/pwmchip%d/pwm%d/enable", pwm_chip_, pwm_channel_);
    setpin_[len] = 0;
    FILE *set_enable = fopen(setpin_, "w");
    if (set_enable == nullptr) {
        printf("open %s error\n", setpin_);
        return -1;
    } else {
        std::cout << "Open " << setpin_ << std::endl;
        if(enable) {
            fprintf(set_enable, "1");
        } else {
            fprintf(set_enable, "0");
        }
    }
    fclose(set_enable);
    return 0;
}

int Pwm::PwmPolarity(bool value)
{
    int len = snprintf(setpin_, sizeof(setpin_) - 1, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", pwm_chip_, pwm_channel_);
    setpin_[len] = 0;
    FILE *set_polarity = fopen(setpin_, "w");
    if (set_polarity == nullptr) {
        printf("open %s error\n", setpin_);
        return -1;
    } else {
        std::cout << "Open " << setpin_ << std::endl;
        if (value) {
            fprintf(set_polarity, "normal");
        } else {
            fprintf(set_polarity, "inversed");
        }
    }
    fclose(set_polarity);
    return 0;
}

int Pwm::PwmPeriod(int value)
{
    int len = snprintf(setpin_, sizeof(setpin_) - 1, "/sys/class/pwm/pwmchip%d/pwm%d/period", pwm_chip_, pwm_channel_);
    setpin_[len] = 0;
    FILE *set_period = fopen(setpin_, "w");
    if (set_period == nullptr) {
        printf("open %s error\n", setpin_);
        return -1;
    } else {
        fprintf(set_period, "%d", value);
    }
    fclose(set_period);
    return 0;
}

int Pwm::PwmDutyCycle(int value)
{
    int len = snprintf(setpin_, sizeof(setpin_) - 1, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", pwm_chip_, pwm_channel_);
    setpin_[len] = 0;
    FILE *set_duty_cycle = fopen(setpin_, "w");
    if (set_duty_cycle == nullptr) {
        printf("open %s error\n", setpin_);
        return -1;
    } else {
        fprintf(set_duty_cycle, "%d", value);
    }
    fclose(set_duty_cycle);
    return 0;
}

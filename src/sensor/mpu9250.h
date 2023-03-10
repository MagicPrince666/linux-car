/**
 * @file mpu9250.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-02-08
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <cstdint>
#include "driver_mpu9250_dmp.h"

class Mpu9250
{
private:
    uint8_t (*g_gpio_irq_)(void) = nullptr;
    int16_t gs_accel_raw[128][3];
    float gs_accel_g[128][3];
    int16_t gs_gyro_raw[128][3];
    float gs_gyro_dps[128][3];
    int32_t gs_quat[128][4];
    float gs_pitch[128];
    float gs_roll[128];
    float gs_yaw[128];
    mpu9250_address_t addr;

public:
    Mpu9250();
    virtual ~Mpu9250();

    virtual bool Init();

    int Mpu9250Test();

protected:
    int GpioInterruptInit();

    void GpioInterruptDeinit();
};
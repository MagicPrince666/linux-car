/**
 * @file orangepi.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 香橙派 智能遥控小车
 * @version 0.1
 * @date 2023-01-11
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <memory>
#include "CarFactory.h"
#include "srf04.h"
#include "mpu6050.h"

class ArmPi : public CarProduct {
private:
    void SensorShow();
    // std::shared_ptr<Mpu6050> imu_data_;
    std::shared_ptr<Srf04> ultrasonic_;
    
public:
    ArmPi(std::string name);
    ~ArmPi();
    virtual bool Run(bool debug);
};
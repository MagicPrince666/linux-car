/**
 * @file makecar.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 小车生产车间
 * @version 0.1
 * @date 2023-01-11
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <memory>

#include "CarFactory.h"
#include "f1c100s.h"
#include "armpi.h"

enum CarType {
    F1C100S_REMOTE_CAR = 0,
    ARM_PI_SMART_CAR,
    UNKNOWN
};

// 根据具体型号生产小车实例 简单工厂模式
class ProduceCar
{
private:
    CarProduct* my_new_car_;

public:
    ProduceCar();
    ~ProduceCar();

    virtual CarProduct *Create(CarType type);
};

// 根据具体型号生产小车实例 工厂方法模式
// 生产f1c100s 遥控车的工厂
class F1c100sRemote : public CarFactory
{
public:
	CarProduct* createCarProduct() {
		return new F1c100s("f1c100s remote car");
	}
};

// 生产香橙派智能小车的工厂
class ArmPiCar : public CarFactory
{
public:
	CarProduct* createCarProduct() {
		return new ArmPi("Orangepi smart car");
	}
};


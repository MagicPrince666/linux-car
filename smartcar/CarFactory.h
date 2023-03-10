/**
 * @file CarFactory.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 汽车工厂 通过这个类创建小车对象，调用者无需知晓是什么类型的小车
 * @version 0.1
 * @date 2023-01-11
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <iostream>
#include "pwm.h"
#include "jsonparse.h"

// 汽车产品基类
class CarProduct {
public:
  CarProduct(std::string name);
  virtual ~CarProduct() {}
  virtual bool Run(bool debug) = 0;
protected:
	std::string car_name_;
  Json::Value conf_json_;

  bool LoadPwmConfig(std::string key, PwmPram &pwm);
  bool LoadPinConfig(std::string key, int &pin);
  bool LoadDistanceConfig(std::string key, bool &enable);
};

//   汽车工厂基类
class CarFactory
{
public:
	virtual CarProduct* createCarProduct() = 0;
};

/**
 * @file iio_bus.h
 * @author 黄李全 (846863428@qq.com)
 * @brief ???用什么设计模式好呢，要支持多种类的IIO总线
 * @version 0.1
 * @date 2023-01-12
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <iostream>

//定义抽象类
class product1
{
public:
    virtual void show() = 0;
};

//定义具体类
class product_A1 :public product1
{
public:
    void show(){ std::cout << "product A1" << std::endl; }
};

class product_B1 :public product1
{
public:
    void show(){ std::cout << "product B1" << std::endl; }
};

//定义抽象类
class product2
{
public:
    virtual void show() = 0;
};

//定义具体类
class product_A2 :public product2
{
public:
    void show(){ std::cout << "product A2" << std::endl; }
};

class product_B2 :public product2
{
public:
    void show(){ std::cout << "product B2" << std::endl; }
};


class Factory
{
public:
    virtual product1 *creat1() = 0;
    virtual product2 *creat2() = 0;
};

class FactoryA
{
public:
    product1 *creat1(){ return new product_A1(); }
    product2 *creat2(){ return new product_A2(); }
};

class FactoryB
{
public:
    product1 *creat1(){ return new product_B1(); }
    product2 *creat2(){ return new product_B2(); }
};

// int main()
// {
//     FactoryA *factoryA = new FactoryA();
//     factoryA->creat1()->show();
//     factoryA->creat2()->show();

//     FactoryB *factoryB = new FactoryB();
//     factoryB->creat1()->show();
//     factoryB->creat2()->show();

//     return 0;
// }

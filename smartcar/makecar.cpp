#include <iostream>
#include <unistd.h>
#include <new>

#include "f1c100s.h"
#include "makecar.h"

ProduceCar::ProduceCar() {
    my_new_car_ = nullptr;
}

ProduceCar::~ProduceCar() {
    if(my_new_car_) {
        delete my_new_car_;
    }
}

CarProduct *ProduceCar::Create(CarType type)
{
    switch (type) {
    case F1C100S_REMOTE_CAR:
        my_new_car_ = new (std::nothrow) F1c100s("f1c100s");
        break;
    case ARM_PI_SMART_CAR:
        my_new_car_ = new (std::nothrow) ArmPi("ArmPi");
        break;
    default:
        break;
    }
    return my_new_car_;
}


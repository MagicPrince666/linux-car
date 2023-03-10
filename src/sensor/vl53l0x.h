#pragma once

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>

class Vl53l0x {
public:
    Vl53l0x(std::string dev = "vl53l0x");
    ~Vl53l0x();
    int GetDistance();

private:
    std::string device_;
};

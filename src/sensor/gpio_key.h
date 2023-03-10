#pragma once
#include <sys/time.h>

class GpioKey
{
public:
    GpioKey();
    ~GpioKey();

    bool init();
    int IRKey();

private:
    int key_input_fd_{-1};
    struct timeval last_time_;
};

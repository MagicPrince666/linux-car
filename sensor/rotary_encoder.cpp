#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#ifndef __APPLE__
#include <linux/input.h>
#include <sys/timerfd.h>
#endif

#include "rotary_encoder.h"
#include "utils.h"
#include "xepoll.h"

RotaryEncoder::RotaryEncoder(float reduction_ratio, float precision)
    : reduction_ratio_(reduction_ratio), precision_(precision)
{
    rotary_encoder_fd_ = -1;
    velocity_          = 0;
    timer_fd_          = -1;
    char buf[256]      = {
        0,
    };

    std::vector<std::string> events;
    Utils::getFiles("/dev/input/", events);
    for (auto iter : events) {
        std::cout << "Device name " << iter << std::endl;
        int fd = -1;
        if ((fd = open(iter.c_str(), O_RDONLY, 0)) >= 0) {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            std::string device(buf);
            std::cout << "Device info " << device << std::endl;
            if (device == "rotary-encoder") {
                rotary_encoder_fd_ = fd;
                break;
            }
            close(fd);
        }
    }

    assert(rotary_encoder_fd_ >= 0);
    Init();
}

RotaryEncoder::~RotaryEncoder()
{
    if (timer_fd_ > 0) {
        MY_EPOLL.EpollDel(timer_fd_);
        close(timer_fd_);
    }
    if (rotary_encoder_fd_ > 0) {
        MY_EPOLL.EpollDel(rotary_encoder_fd_);
        close(rotary_encoder_fd_);
    }
}

int RotaryEncoder::GetKey(void)
{
    struct input_event key;
    int ret = read(rotary_encoder_fd_, &key, sizeof(key));
    if (ret > 0) {
        // 使用旋转编码器相对轴， key.value只有 1/-1
        if (key.type != 0) {
            // std::cout << "Type = " << key.type << " Code = " << key.code << " Value = " << key.value << std::endl;
            std::lock_guard<std::mutex> mylock_guard(counter_lock_);
            if (key.value == 1) { // 正转
                velocity_++;
            } else { // -1 反转
                velocity_--;
            }
        }
    }
    return ret;
}

// T法测速 n = F0/(M1 * C)
int RotaryEncoder::TSpeed(void)
{
    // 速度较高 T法测速精度较低
    return 0;
}

bool RotaryEncoder::InitTimer(void)
{
    // 创建1s定时器fd
    if ((timer_fd_ =
             timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)) < 0) {
        std::cout << "create timer fd fail" << std::endl;
        return false;
    }
    assert(timer_fd_ > 0);
    // 设置1s定时器
    struct itimerspec time_intv;
    time_intv.it_value.tv_sec     = 1;                 // 设定超时 1s
    time_intv.it_value.tv_nsec    = 0;
    time_intv.it_interval.tv_sec  = time_intv.it_value.tv_sec; // 间隔超时
    time_intv.it_interval.tv_nsec = time_intv.it_value.tv_nsec;
    // 启动定时器
    timerfd_settime(timer_fd_, 0, &time_intv, NULL);
    // 绑定回调函数
    MY_EPOLL.EpollAdd(timer_fd_, std::bind(&RotaryEncoder::timeOutCallBack, this));
    return true;
}

bool RotaryEncoder::Init()
{
    // 绑定回调函数
    if (rotary_encoder_fd_ > 0) {
        std::cout << "Bind epoll" << std::endl;
        MY_EPOLL.EpollAdd(rotary_encoder_fd_, std::bind(&RotaryEncoder::GetKey, this));
    }
    InitTimer(); // 初始化定时器
    return true;
}

// M法测速
int RotaryEncoder::timeOutCallBack()
{
    uint64_t value;
    int ret = read(timer_fd_, &value, sizeof(uint64_t));
    std::lock_guard<std::mutex> mylock_guard(counter_lock_);
    float laps = velocity_/reduction_ratio_/precision_; // 实际圈数 = 编码器计数器 / 减速比 / 编码器精度
    std::cout << "Number of laps = " << laps << std::endl;
    velocity_ = 0;
    return ret;
}

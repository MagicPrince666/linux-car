#include <iostream>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#ifndef __APPLE__
#include <linux/input.h>
#endif

#include "gpio_key.h"
#include "xepoll.h"
#include "utils.h"

GpioKey::GpioKey()
{
    char          buf[256] = { 0, };  /* RATS: Use ok */
    int           fd = -1;
    std::vector<std::string> events;
    Utils::getFiles("/dev/input/", events);
    for (auto iter : events) {
        std::cout << "Device name "<< iter << std::endl;
        if ((fd = open(iter.c_str(), O_RDONLY, 0)) >= 0) {
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            std::string dev(buf);
            std::cout << "Device info "<< dev << std::endl;
            if(dev == "r-gpio-keys") {
                key_input_fd_ = fd;
                break;
            }
            close(fd);
        }
    }
    assert(key_input_fd_ >= 0);
    init();
}

GpioKey::~GpioKey(void)
{
    if (key_input_fd_ > 0) {
        MY_EPOLL.EpollDel(key_input_fd_);
        close(key_input_fd_);
    }
}

int GpioKey::IRKey(void)
{
    struct input_event key;
    int ret = read(key_input_fd_, &key, sizeof(key));
    if(ret > 0) {
        std::cout << "Type = " << key.type << " Code = " << key.code << " Value = " << key.value <<std::endl;
    }
    return ret;
}

bool GpioKey::init() {
  // 绑定回调函数
  if (key_input_fd_ > 0) {
        std::cout << "Bind epoll" << std::endl;
        MY_EPOLL.EpollAdd(key_input_fd_, std::bind(&GpioKey::IRKey, this));
  }
  return true;
}

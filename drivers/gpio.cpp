#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <unordered_map>

#include "gpio.h"

Gpio::Gpio(int pin, bool io) : gpio_pin_(pin)
{
    gpio_fd_ = -1;
    std::cout << "Init user gpio: " << pin << std::endl;
    SetupGpio();
    GpioOutPut(io);
    OpenGpio();
}

Gpio::~Gpio()
{
    CloseGpio();
    UnExportGpio();
    std::cout << "Close user gpio: " << gpio_pin_ << std::endl;
}

int Gpio::LightCtl(int leds, bool status)
{
    switch (leds) {
    default:
        break;
    }
    return 0;
}

int Gpio::SetupGpio()
{
    FILE *set_export = fopen("/sys/class/gpio/export", "w");
    if (set_export == nullptr) {
        printf("Can't open /sys/class/gpio/export!\n");
    } else {
        fprintf(set_export, "%d", gpio_pin_);
    }
    assert(set_export != nullptr);
    fclose(set_export);
    return 0;
}

int Gpio::UnExportGpio()
{
    FILE *set_export = fopen("/sys/class/gpio/unexport", "w");
    if (set_export == nullptr) {
        printf("Can't open /sys/class/gpio/unexport!\n");
    } else {
        fprintf(set_export, "%d", gpio_pin_);
    }
    fclose(set_export);
    return 0;
}

int Gpio::SetGpioOut()
{
    int len = snprintf(setpin, sizeof(setpin) - 1, "/sys/class/gpio/gpio%d/direction", gpio_pin_);
    setpin[len] = 0;
    FILE *set_dir = fopen(setpin, "w");
    if (set_dir == nullptr) {
        printf("open %s error\n", setpin);
    } else {
        fprintf(set_dir, "out");
    }
    fclose(set_dir);
    return 0;
}

int Gpio::SetGpioIn()
{
    int len = snprintf(setpin, sizeof(setpin) - 1, "/sys/class/gpio/gpio%d/direction", gpio_pin_);
    setpin[len] = 0;
    FILE *set_dir = fopen(setpin, "w");
    if (set_dir == nullptr) {
        printf("open %s error\n", setpin);
    } else {
        fprintf(set_dir, "in");
    }
    fclose(set_dir);
    return 0;
}

int Gpio::OpenGpio()
{
    int len = snprintf(setpin, sizeof(setpin) - 1, "/sys/class/gpio/gpio%d/value", gpio_pin_);
    setpin[len] = 0;
    gpio_fd_ = open(setpin, O_RDWR);
    if (gpio_fd_ <= 0) {
        printf("can not open %s\n", setpin);
        return -1;
    }
    return 0;
}

int Gpio::CloseGpio()
{
    if (gpio_fd_ > 0) {
        close(gpio_fd_);
    }
    return 0;
}

int Gpio::GpioOutPut(bool io)
{
    FILE *set_export = nullptr;

    int len = snprintf(setpin, sizeof(setpin) - 1, "/sys/class/gpio/gpio%d/direction", gpio_pin_);
    setpin[len] = 0;
    if ((access(setpin, F_OK)) == -1) { // need creat
        set_export = fopen("/sys/class/gpio/export", "w");
        if (set_export == nullptr) {
            printf("Can't open /sys/class/gpio/export!\n");
            return -1;
        } else {
            fprintf(set_export, "%d", gpio_pin_);
        }
        fclose(set_export);
    }

    set_export = fopen(setpin, "w");
    if (set_export == nullptr) {
        printf("open %s error\n", setpin);
        return -2;
    } else {
        if (io) {
            fprintf(set_export, "out");
        } else {
            fprintf(set_export, "in");
        }
    }
    fclose(set_export);

    return gpio_pin_;
}

int Gpio::SetGpioValue(bool value)
{
    if (gpio_fd_ > 0) {
        write(gpio_fd_, std::to_string(value).c_str(), 1);
    }

    return 0;
}

std::string Gpio::ReadFileIntoString(const std::string& path) {
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int Gpio::GetGpioValue()
{
    int value = -1;
#if 1
    int len = snprintf(setpin, sizeof(setpin) - 1, "/sys/class/gpio/gpio%d/value", gpio_pin_);
    setpin[len] = 0;

    std::string data = ReadFileIntoString(setpin);
    if (data.find("0") != std::string::npos) {
        value  = 0;
    } else if (data.find("1") != std::string::npos) {
        value = 1;
    }
#else
    char ch[8] = {0};
	int len = read(gpio_fd_, &ch, sizeof(ch));
    if(len > 0) {
        if (ch[0] != '0') {
            value = 1;
        } else {
            value = 0;
        }
    }
#endif

    return value;
}

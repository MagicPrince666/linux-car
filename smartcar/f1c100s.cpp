#include <sys/stat.h>

#include "spdlog/cfg/env.h" // support for loading levels from the environment variable
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/spdlog.h"

#include "f1c100s.h"
#include "gpio.h"
#include "moto.h"
#include "sonnyps2.h"
#include "pwm.h"

F1c100s::F1c100s(std::string name) : CarProduct(name) {
    std::cout << "The new car you buy is: " << car_name_ << std::endl;
}

F1c100s::~F1c100s() {
    std::cout << "Losed your " << car_name_ << " Car" << std::endl;
}

bool F1c100s::Run(bool debug)
{
    bool loop  = true;
    std::unique_ptr<Moto> moto(new Moto);
    PwmPram moto_pwm, servo_pwm;
    moto_pwm.chip = 0;
    moto_pwm.channel = 0;
    moto_pwm.polarity = true;
    moto_pwm.period = 40000;
    moto_pwm.dutycycle = 0;

    servo_pwm.chip = 0;
    servo_pwm.channel = 1;
    servo_pwm.polarity = true;
    servo_pwm.period = 20000000;
    servo_pwm.dutycycle = 1500000;
    LoadPwmConfig("moto_pwm", moto_pwm);
    LoadPwmConfig("servo_pwm", servo_pwm);
    moto->Init(128 + 3, 128 + 4, moto_pwm, servo_pwm);

    int32_t speed         = 0;
    float angle           = 45.0;
    int32_t angle_correct = 0;
    uint32_t rx = 0, ry = 0;
    int32_t angle_counter = 0;
    int32_t counter       = 0;

    moto->SetServo(angle + angle_correct);
    moto->Stop();

    std::unique_ptr<Gpio> remote_pwr(new Gpio(3, true)); // PA3
    remote_pwr->SetGpioValue(true); // 接收器电源
    sleep(1);

    // // 创建sbus遥控工厂
    // std::shared_ptr<RemoteFactory> factory(new SonnyRemote());
    // // 通过工厂方法创建sbus遥控产品
    // std::shared_ptr<RemoteProduct> remote(factory->CreateRemoteProduct());
    Ps2Remote *remote = new Ps2Remote("/dev/spidev1.0", debug);
    remote->Init();

    RemoteState rc_data;
    // RemoteState last_rc_data;

    while (loop) {
        remote->Request(rc_data);

        if(rc_data.lose_signal) {
            moto->Stop();
            remote->Uninit();
            remote_pwr->SetGpioValue(false); // 关闭接收器电源
            usleep(200000);
            remote_pwr->SetGpioValue(true); // 开启接收器电源
            sleep(1);
            remote->Init();
        }

        bool angle_ctl = false;
        bool speed_ctl = false;

        if (rc_data.l1) { // 左微调
            counter++;
            if (counter % 5 == 0) {
                if (angle_correct > -10) {
                    angle_correct--;
                }
                spdlog::info("angle_correct {}", angle_correct);
            }
        } else if (rc_data.r1) { // 右微调
            counter++;
            if (counter % 5 == 0) {
                if (angle_correct < 10) {
                    angle_correct++;
                }
                spdlog::info("angle_correct {}", angle_correct);
            }
        } else {
            counter = 0;
        }

        if (rc_data.front) {
            moto->GoForward(100);
            speed_ctl = true;
        }
        if (rc_data.back) {
            moto->GoBack(95);
            speed_ctl = true;
        }
        if (rc_data.left) {
            moto->SetServo(10.0 + angle_correct);
            angle_ctl = true;
        }
        if (rc_data.right) {
            moto->SetServo(80.0 + angle_correct);
            angle_ctl = true;
        }

        if (rc_data.triangle) {
            moto->GoForward(100);
            speed_ctl = true;
        }
        if (rc_data.fork) {
            moto->GoBack(95);
            speed_ctl = true;
        }
        if (rc_data.quadrilateral) {
            moto->SetServo(20.0 + angle_correct);
            angle_ctl = true;
        }
        if (rc_data.rotundity) {
            moto->SetServo(70.0 + angle_correct);
            angle_ctl = true;
        }

        rx = rc_data.adsrx;
        ry = rc_data.adsry;

        if (rx < 48 || rx > 52) { // 0 - 100 对应舵机 0 - 90 度
            angle = ((float)rx / 100.0) * 50 + 20;
            moto->SetServo(angle + angle_correct);
            angle_ctl = true;
        }

        if (ry < 40 || ry > 60) {
            if (ry < 40) {
                speed = (50 - ry) * 2;
                moto->GoForward(speed);
                speed_ctl = true;
            }

            if (ry > 60) {
                speed = (ry - 50) * 2;
                moto->GoBack(speed);
                speed_ctl = true;
            }
            // spdlog::info("speed {}", speed);
        }

        if (!speed_ctl) {
            moto->Stop();
        }

        if (!angle_ctl) {
            angle_counter++;
            if (angle_counter > 2) { // 消抖
                angle_counter = 0;
                moto->SetServo(45.0 + angle_correct);
            }
        }

        // // 获取文件大小
        // struct stat info;
        // stat("/tmp/SmartCar.log", &info);
        // int size = info.st_size;
        // if(size > 2*1024*1024) { // 日志文件大于2M
        //     remove("/tmp/SmartCar.log");
        // }
        usleep(20000);
    }
    if(remote) {
        delete remote;
    }
    return true;
}
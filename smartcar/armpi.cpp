#include "CarFactory.h"
#include "bts7960.h"
#include "sbus.h"
#include <thread>
#include <unistd.h>

#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/spdlog.h"

#include "OnDemandRTSPServer.h"
#include "armpi.h"
#include "RemoteAdapter.h"

ArmPi::ArmPi(std::string name) : CarProduct(name)
{
    std::cout << "The new car you buy is: " << car_name_ << std::endl;
}

ArmPi::~ArmPi()
{
    std::cout << "Losed your " << car_name_ << " Car" << std::endl;
}

bool ArmPi::Run(bool debug)
{
    // 创建sbus遥控工厂
    std::shared_ptr<RemoteFactory> factory(new SbusRemote());
    // 通过工厂方法创建sbus遥控产品
    std::shared_ptr<RemoteProduct> sbus(factory->CreateRemoteProduct());

    int ena = -1, enb = -1;
    int ris = -1, lis = -1;
    PwmPram moto_pwm, servo_pwm;
    // PD11
    // PA10
    LoadPinConfig("bts7960_ris", ris);
    LoadPinConfig("bts7960_lis", lis);
    Bts7960 bts7960(lis, ris);
    // PC7 32 * 2 + 7 = 71
    // PD14 32 * 3 + 14 = 110
    LoadPwmConfig("moto_pwm", moto_pwm);
    LoadPwmConfig("servo_pwm", servo_pwm);

    LoadPinConfig("moto_ena", ena);
    LoadPinConfig("moto_enb", enb);

    bts7960.Init(ena, enb, moto_pwm, servo_pwm);
    RemoteState rc_data;

    bts7960.Stop();

    std::thread video_thread_loop(rtsp_start);
    video_thread_loop.detach();

    // std::thread sensor_thread_loop([](OrangePi *p_this) { p_this->SensorShow(); }, this);
    // sensor_thread_loop.detach();

    // imu_data_ = std::make_shared<Mpu6050>();
    bool sonar = false;
    LoadDistanceConfig("srf04", sonar);
    if(sonar) {
        ultrasonic_ = std::make_shared<Srf04>();
    }

    int distance = 1000;

    while (true) {
        sbus->Request(rc_data);
        if(ultrasonic_) {
            distance = ultrasonic_->Srf04Distance();
        }

        if(!rc_data.adslx && !rc_data.adsly && !rc_data.adsrx && !rc_data.adsry) {
            sleep(1);
            continue;
        }

        if (debug) {
            printf("%02d ", rc_data.adslx);
            printf("%02d ", rc_data.adsly);
            printf("%02d ", rc_data.adsrx);
            printf("%02d ", rc_data.adsry);
            for (int i = 0; i < 12; i++) {
                printf("%02d ", rc_data.ads[i]);
            }
            printf("\n");
        }

        // spdlog::info("Ultrasonic distance = {}", distance);
        if (rc_data.adsry > 60) {
            // 前方距离不足50cm 不要在前进了
            if(distance > 500) {
                bts7960.GoForward(rc_data.adsry);
            } else if(distance > 0 && distance <= 400) {
                bts7960.Stop();
            }
            
        } else if (rc_data.adsry > 1 && rc_data.adsry < 40) {
            uint32_t back_percent = 100 - rc_data.adsry;
            bts7960.GoBack(back_percent);
        } else {
            bts7960.Stop();
        }

        float angle = rc_data.adsrx / 100.0 * 90.0;
        bts7960.SetServo(angle);

        usleep(100000);
    }
    return true;
}

void ArmPi::SensorShow()
{
    // imu_data_ = std::make_shared<Mpu6050>();

    while (true) {
        // int accel_x_y_z_[3]   = {0};
        // int anglvel_x_y_z_[3] = {0};
        // imu_data_->MpuGetGyroscope(accel_x_y_z_[0], accel_x_y_z_[1], accel_x_y_z_[2]);
        // imu_data_->MpuGetAccelerometer(anglvel_x_y_z_[0], anglvel_x_y_z_[1], anglvel_x_y_z_[2]);
        // spdlog::info("IMU Gyroscope\t\t gx = {}\t gy =  {}\t gz = {}", accel_x_y_z_[0], accel_x_y_z_[1], accel_x_y_z_[2]);
        // spdlog::info("IMU Accelerometer\t x = {}\t y =  {}\t z = {}", anglvel_x_y_z_[0], anglvel_x_y_z_[1], anglvel_x_y_z_[2]);
        sleep(1);
    }
}
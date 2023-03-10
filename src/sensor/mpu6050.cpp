#include <assert.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/stat.h>

#include "interface.h"
#include "mpu6050.h"
#include "utils.h"
#include "xinotify.h"

Mpu6050::Mpu6050(std::string dev) : device_(dev)
{
    mpu_int_ = nullptr;
    GpioInterruptInit();
    // assert(mpu_int_ != nullptr);
    std::cout << BOLDGREEN << "Iio bus path " << device_ << std::endl;
}

Mpu6050::~Mpu6050()
{
    GpioInterruptDeinit();
    // for (auto accel : g_accel_) {
    //     MY_INOTIFY.DelFileWatch(device_ + accel.first);
    // }

    // for (auto anglvel : g_anglvel_) {
    //     MY_INOTIFY.DelFileWatch(device_ + anglvel.first);
    // }
    std::cout << BOLDGREEN << "Close mpu6050 device!" << std::endl;
}

bool Mpu6050::Init()
{
    device_ = Utils::ScanIioDevice(device_);
    assert(device_ != "");
    // for (auto accel : g_accel_) {
    //     MY_INOTIFY.AddFileWatch(device_ + accel.first, std::bind(&Mpu6050::Gyroscope, this));
    // }

    // for (auto anglvel : g_anglvel_) {
    //     MY_INOTIFY.AddFileWatch(device_ + anglvel.first, std::bind(&Mpu6050::Accelerometer, this));
    // }
    return true;
}

// 得到陀螺仪值(原始值)
// gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
// 返回值:0,成功
//     其他,错误代码
bool Mpu6050::MpuGetGyroscope(int &gx, int &gy, int &gz)
{
    for (std::unordered_map<std::string, int>::iterator iter = g_accel_.begin(); iter != g_accel_.end(); iter++) {
        std::string buf = Utils::ReadFileIntoString(device_ + iter->first);
        iter->second    = atoi(buf.c_str());
        // std::cout << GREEN << iter->first << " = " << iter->second << std::endl;
    }
    gx = g_accel_["in_accel_x_raw"];
    gy = g_accel_["in_accel_y_raw"];
    gz = g_accel_["in_accel_z_raw"];
    return true;
}

// 得到加速度值(原始值)
// ax,ay,az:陀螺仪x,y,z轴的原始读数(带符号)
// 返回值:0,成功
//     其他,错误代码
bool Mpu6050::MpuGetAccelerometer(int &ax, int &ay, int &az)
{
    for (std::unordered_map<std::string, int>::iterator iter = g_anglvel_.begin(); iter != g_anglvel_.end(); iter++) {
        std::string buf = Utils::ReadFileIntoString(device_ + iter->first);
        iter->second    = atoi(buf.c_str());
        // std::cout << GREEN << iter->first << " = " << iter->second << std::endl;
    }
    ax = g_anglvel_["in_anglvel_x_raw"];
    ay = g_anglvel_["in_anglvel_y_raw"];
    az = g_anglvel_["in_anglvel_z_raw"];
    return true;
}

int Mpu6050::Gyroscope()
{
    std::cout << GREEN << __FUNCTION__ << std::endl;
    return 0;
}

int Mpu6050::Accelerometer()
{
    std::cout << GREEN << __FUNCTION__ << std::endl;
    return 0;
}

void Mpu6050::Euler2Quaternion(float roll, float pitch, float yaw, quaternion_t &quat)
{
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);
    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    quat.w       = cy * cp * cr + sy * sp * sr;
    quat.x       = cy * cp * sr - sy * sp * cr;
    quat.y       = sy * cp * sr + cy * sp * cr;
    quat.z       = sy * cp * cr - cy * sp * sr;
}

static void a_receive_callback(uint8_t type)
{
    switch (type) {
    case MPU6050_INTERRUPT_MOTION: {
        mpu6050_interface_debug_print("mpu6050: irq motion.\n");

        break;
    }
    case MPU6050_INTERRUPT_FIFO_OVERFLOW: {
        mpu6050_interface_debug_print("mpu6050: irq fifo overflow.\n");

        break;
    }
    case MPU6050_INTERRUPT_I2C_MAST: {
        mpu6050_interface_debug_print("mpu6050: irq i2c master.\n");

        break;
    }
    case MPU6050_INTERRUPT_DMP: {
        mpu6050_interface_debug_print("mpu6050: irq dmp\n");

        break;
    }
    case MPU6050_INTERRUPT_DATA_READY: {
        mpu6050_interface_debug_print("mpu6050: irq data ready\n");

        break;
    }
    default: {
        mpu6050_interface_debug_print("mpu6050: irq unknown code.\n");

        break;
    }
    }
}

static void a_dmp_tap_callback(uint8_t count, uint8_t direction)
{
    switch (direction) {
    case MPU6050_DMP_TAP_X_UP: {
        mpu6050_interface_debug_print("mpu6050: tap irq x up with %d.\n", count);

        break;
    }
    case MPU6050_DMP_TAP_X_DOWN: {
        mpu6050_interface_debug_print("mpu6050: tap irq x down with %d.\n", count);

        break;
    }
    case MPU6050_DMP_TAP_Y_UP: {
        mpu6050_interface_debug_print("mpu6050: tap irq y up with %d.\n", count);

        break;
    }
    case MPU6050_DMP_TAP_Y_DOWN: {
        mpu6050_interface_debug_print("mpu6050: tap irq y down with %d.\n", count);

        break;
    }
    case MPU6050_DMP_TAP_Z_UP: {
        mpu6050_interface_debug_print("mpu6050: tap irq z up with %d.\n", count);

        break;
    }
    case MPU6050_DMP_TAP_Z_DOWN: {
        mpu6050_interface_debug_print("mpu6050: tap irq z down with %d.\n", count);

        break;
    }
    default: {
        mpu6050_interface_debug_print("mpu6050: tap irq unknown code.\n");

        break;
    }
    }
}

static void a_dmp_orient_callback(uint8_t orientation)
{
    switch (orientation) {
    case MPU6050_DMP_ORIENT_PORTRAIT: {
        mpu6050_interface_debug_print("mpu6050: orient irq portrait.\n");

        break;
    }
    case MPU6050_DMP_ORIENT_LANDSCAPE: {
        mpu6050_interface_debug_print("mpu6050: orient irq landscape.\n");

        break;
    }
    case MPU6050_DMP_ORIENT_REVERSE_PORTRAIT: {
        mpu6050_interface_debug_print("mpu6050: orient irq reverse portrait.\n");

        break;
    }
    case MPU6050_DMP_ORIENT_REVERSE_LANDSCAPE: {
        mpu6050_interface_debug_print("mpu6050: orient irq reverse landscape.\n");

        break;
    }
    default: {
        mpu6050_interface_debug_print("mpu6050: orient irq unknown code.\n");

        break;
    }
    }
}

int Mpu6050::GpioInterruptInit()
{
    // mpu_int_ = new GpioKey;
    // if (mpu_int_) {
    //     return 0;
    // }
    return 0;
}

void Mpu6050::GpioInterruptDeinit()
{
    // delete mpu_int_;
}

int Mpu6050::Mpu6050Test()
{
    uint32_t cnt;
    uint16_t len = 128;
    gpio_irq_ = mpu6050_dmp_irq_handler;

    /* run dmp function */
    addr_ = MPU6050_ADDRESS_AD0_LOW;
    if (mpu6050_dmp_init(addr_, a_receive_callback,
                         a_dmp_tap_callback, a_dmp_orient_callback) != 0) {
        gpio_irq_ = nullptr;
        GpioInterruptDeinit();

        return 1;
    }

    /* delay 500 ms */
    mpu6050_interface_delay_ms(500);

    while(true) {
        

        /* read */
        if (mpu6050_dmp_read_all(gs_accel_raw_, gs_accel_g_,
                                 gs_gyro_raw_, gs_gyro_dps_,
                                 gs_quat_,
                                 gs_pitch_, gs_roll_, gs_yaw_,
                                 &len) != 0) {
            (void)mpu6050_dmp_deinit();
            gpio_irq_ = nullptr;
            GpioInterruptDeinit();

            return 1;
        }

        /* output */
        // mpu6050_interface_debug_print("mpu6050: fifo %d.\n", len);
        mpu6050_interface_debug_print("mpu6050: pitch[0] is %0.2f dps.\n", gs_pitch_[0]);
        mpu6050_interface_debug_print("mpu6050: roll[0] is %0.2f dps.\n", gs_roll_[0]);
        mpu6050_interface_debug_print("mpu6050: yaw[0] is %0.2f dps.\n", gs_yaw_[0]);
        // mpu6050_interface_debug_print("mpu6050: acc x[0] is %0.2fg.\n", gs_accel_g_[0][0]);
        // mpu6050_interface_debug_print("mpu6050: acc y[0] is %0.2fg.\n", gs_accel_g_[0][1]);
        // mpu6050_interface_debug_print("mpu6050: acc z[0] is %0.2fg.\n", gs_accel_g_[0][2]);
        // mpu6050_interface_debug_print("mpu6050: gyro x[0] is %0.2fdps.\n", gs_gyro_dps_[0][0]);
        // mpu6050_interface_debug_print("mpu6050: gyro y[0] is %0.2fdps.\n", gs_gyro_dps_[0][1]);
        // mpu6050_interface_debug_print("mpu6050: gyro z[0] is %0.2fdps.\n", gs_gyro_dps_[0][2]);

        /* delay 500 ms */
        mpu6050_interface_delay_ms(500);

        /* get the pedometer step count */
        int res = mpu6050_dmp_get_pedometer_counter(&cnt);
        if (res != 0) {
            (void)mpu6050_dmp_deinit();
            gpio_irq_ = nullptr;
            GpioInterruptDeinit();

            return 1;
        }
    }

    /* deinit */
    (void)mpu6050_dmp_deinit();
    gpio_irq_ = nullptr;

    return 0;
}
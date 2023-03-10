# 芒果派D1 小车

## 状态
进行中...

## 硬件连接
| 连接设备       | name    | PIN | PIN | name     | 连接设备       |
|---------------|---------|-----|-----|----------|---------------|
| NC            | 3.3v    | 1   | 2   | 5v       | NC            |
| MPU6050 INT   | PG13/SDA| 3   | 4   | 5v       | NC            |
| 编码器B相      | PG12/SCL| 5   | 6   | GND      | NC            |
| 编码器A相      | PB7     | 7   | 8   | TX0      | 调试串口tx     |
| NC            | GND     | 9   | 10  | RX0      | 调试串口rx     |
| NC            | PD21/TX1| 11  | 12  | PB5/PWM0 | BTS7960 RPWM  |
| 遥控接收器SBUS | PD22/RX1| 13  | 14  | GND      | NC            |
| 舵机PWM       | PB0/PWM3| 15  | 16  | PB1/PWM4 | BTS7960 EN    |
| NC            | 3.3v    | 17  | 18  | PD14     | BTS7960 LPWM  |
| MOSI          | MOSI    | 19  | 20  | GND      | NC            |
| MISO          | MISO    | 21  | 22  | PC1      | Lidar RX      |
| SCLK          | SCLK    | 23  | 24  | CS0      | Lcd Cs        |
| NC            | GND     | 25  | 26  | CS1      | Touch Cs      |
| MPU6050 SDA   | PE17    | 27  | 28  | PE16     | MPU6050 SCL   |
| Lidar PWM     | PB10    | 29  | 30  | GND      | NC            |
| Lcd Bl        | PB11    | 31  | 32  | PC0      | NC            |
| SR04 Echo     | PB12    | 33  | 34  | GND      | NC            |
| SR04 Trig     | PB6/PWM1| 35  | 36  | PB2      | Lcd Dc        |
| Touch Int     | PD17    | 37  | 38  | PB3      | NC            |
| NC            | GND     | 39  | 40  | PB4      | VL5310 Int    |

## BTS7960驱动说明
* 方法1：
VCC接单片机的5V电源，GND接单片机的GND。

REN与LEN短路井接5V电平，驱动器可以工作。

LPWM,输入PWM信号或高电平电机正转。

RPWM，输入PWM信号或高电平电机反转。

* 方法2(推荐)：

vCC接单片机的5V电源，GND接单片机的GND。

REN与LEN短路并接输入PWM信号调速。

LPWM，脚输入5V电平电机正转。

RPWM，脚输入5V电平电机反转。

## 编译MQ PRO
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_mqpro.cmake ..
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_mqpro.cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## 设备树配置
开发需要更新设备树配置

[MangoPi MQ PRO devicetree](https://github.com/MagicPrince666/Tina-Linux/blob/main/device/config/chips/d1/configs/mq_pro/linux-5.4/board.dts)

## 已完成
1. 遥控接收机sbus解码
2. 陀螺仪数据获取
3. 电机正反转IO控制
4. 获取USB摄像头数据
5. rtsp 视频直播
6. 超声波传感器数据获取
7. 通过配置文件（json）适配硬件，与软件代码无关
8. 监控文件发生变化
9. 支持GPS定位模块数据获取
10. 更换带码盘电机
11. 支持USB手柄
12. IMU位姿解算

## TO DO
1. 电机支持PID控制
2. 图像识别算法引入
3. 支持udp控制

## 网络连接
```
network={
    ssid="OpenWrt_R619ac_2.4G"
    psk="67123236"
}

wifi_connect_ap_test OpenWrt_R619ac_2.4G 67123236

softap_up <ssid> <pwd> <broadcast/hidden>
softap_down
```

```
tftp -g -r RemoteCar 192.168.3.185
```
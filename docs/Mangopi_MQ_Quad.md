# 芒果派MQ Quad (H616) 小车

## 状态
准备中

## 硬件连接
| 连接设备       | name    | PIN | PIN | name     | 连接设备       |
|---------------|---------|-----|-----|----------|---------------|
| NC            | 3.3v    | 1   | 2   | 5v       | NC            |
| MPU6050 SDA   | SDA     | 3   | 4   | 5v       | NC            |
| MPU6050 SCL   | SCL     | 5   | 6   | GND      | NC            |
| SR04 Echo     | PI0     | 7   | 8   | TX0      | 调试串口tx     |
| NC            | GND     | 9   | 10  | RX0      | 调试串口rx     |
| NC            | TX1     | 11  | 12  | PI1      | BTS7960 LPWM  |
| 遥控接收器SBUS  | RX1     | 13  | 14  | GND      | NC            |
| 舵机PWM        |PI13/PWM3| 15  | 16  | PI14/PWM4| BTS7960 EN    |
| NC            | 3.3v    | 17  | 18  | PH4      | BTS7960 RPWM  |
| SONY PS2 CMMD | MOSI    | 19  | 20  | GND      | NC            |
| SONY PS2 DAT  | MISO    | 21  | 22  | PI6/RX2  | MPU6050 INT   |
| SONY PS2 CLK  | SCLK    | 23  | 24  | CS0      | SONY PS2 ATT  |
| NC            | GND     | 25  | 26  | CS1      | NC            |
| NC            | PI10    | 27  | 28  | PI9      | NC            |
| SR04 Trig     | PI11    | 29  | 30  | GND      | NC            |
| 编码器A相      | PI12    | 31  | 32  | PI5/TX2  | NC            |
| 编码器B相      | PI15    | 33  | 34  | GND      | NC            |
| BTS7960 R_IS  | PI2     | 35  | 36  | PH10     | NC            |
| BTS7960 L_IS  | PI16    | 37  | 38  | PI4      | NC            |
| NC            | GND     | 39  | 40  | PI3      | NC            |

## 编译MQ Quad
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_mqquad.cmake ..
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_mqquad.cmake -DCMAKE_BUILD_TYPE=Debug ..
```

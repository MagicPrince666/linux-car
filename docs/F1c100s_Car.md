# f1c100scar

基于LicheePi nano的遥控小车

## 状态
已完成

![遥控车皂片](遥控车.jpg)

## 硬件连接
| 连接设备       | name    | PIN | PIN | name     | 连接设备       |
|---------------|---------|-----|-----|----------|---------------|
| L298N ENA     | E3      | 1   | 2   | PE2      | PS2 PWR       |
| L298N ENB     | E4      | 3   | 4   | PE1      | TX0           |
| LED2          | E5      | 5   | 6   | PE0      | RX0           |
| 舵机PWM       | E6      | 7   | 8   | 5v       | NC            |
| SONY PS2 ATT  | E7      | 9   | 10  | GND      | NC            |
| SONY PS2 CMMD | E8      | 11  | 12  | 3.3v     | NC            |
| SONY PS2 CLK  | E9      | 13  | 14  | PA0      | TX2            |
| SONY PS2 DAT  | E10     | 15  | 16  | PA1      | RX2           |
| NC            | E11     | 17  | 18  | PA2      | LED1          |
| L298N PWM     | E12     | 19  | 20  | PA3      | 接收器手柄电源使能|

## 设备树配置
开发需要更新设备树配置

[F1c100s licheepi nano devicetree](https://github.com/MagicPrince666/xos/blob/master/board/sipeed/remotecar/devicetree/linux/devicetree.dts)

## 编译 f1c100s
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_f1c100s.cmake ..
$ cmake -DCMAKE_TOOLCHAIN_FILE=cmake/build_for_f1c100s.cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## 已完成
1. GPIO控制
2. PWM设置
3. L298N电机驱动模块和舵机控制
4. 适配索尼PS2无线手柄
5. 遥控逻辑
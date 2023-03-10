# 电机驱动控制
电机控制的逻辑

## 舵机驱动
舵机的控制由pwm来控制，控制周期T=20ms 高电平范围1-2ms,对应角度0-90度
```
$ echo 0 > /sys/class/pwm/pwmchip0/export
$ echo 20000000 > /sys/class/pwm/pwmchip0/pwm0/period
$ echo 1500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
$ echo normal > /sys/class/pwm/pwmchip0/pwm0/polarity
$ echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
```

## 电机控制
通用电机控制由正反转控制的两个IO加一路调速的PWM脚组成

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

## PID 控制速度

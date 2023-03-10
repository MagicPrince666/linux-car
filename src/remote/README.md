# 遥控器
使用适配器模式进行重构

适配器模式不好用，需要为每一种遥控类型添加一个适配器

重构方式使用工厂方法

## sbus初始化代码
对应的串口为串口2
```
Sbus sbus("/dev/ttyS1");
```

把s-bus输出通过反相器后连接到uart接收端。

1. 通过uart接收s-bus数据，uart使用格式为：

    100k波特率，8位数据位，偶校验（Even），2位停止位。

2. uart接收到的数据通过sbus_decoder_get_byte()函数或者sbus_decoder_get_buf()函数传给此模块。

     模块会解析数据，当解析到数据后会把数据保存在rc_data.rawdata（数据范围0-2047） 和 rc_data.percent（数据范围0-100）中。

3. 数据返回方式采用全局变量，一般只要用一个进程不断读取uart数据并往sbus_decoder模块里送，模块就会不断刷新rc_data，

     在需要获取遥控器数据的地方直接取变量 rc_data.rawdata[i] 或 rc_data.percent[i] 即可。

说明：

  对于我的天地飞遥控器，rawdata范围是 340-1740，计算percent方式也是按照这个范围计算的，如果其他遥控器范围不同的话则计算方式也不同。

数据协议简介

大约14ms一帧数据

每帧数据25字节，具体格式如下：

<START> <data0> <data1> ... <data21> <Flags> <END>

其中：

<START>一定是0x0F，<END>一定是0x00。

<data0>到<data21>共22字节，表示16个通道的数值，每个通道11位（0-2047）。具体格式这里不介绍，看解码的代码。

<Flags>这里不讨论，解码时也不关心。

乐迪AT9配置： 波特率：100K(100000)，数据位：9bit，奇偶校验位：偶校验（even）,停止位：2位
/**
 * @file sbus.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 遥控器SBUS接收解码程序
 * @version 0.1
 * @date 2023-01-09
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */

#pragma once

#include "RemoteFactory.h"
#include <iostream>
#include <stdint.h>

typedef struct {
    uint16_t rawdata[16];      // 根据sbus协议，一帧包含16通道，每通道11位（0-2047）
    uint8_t percent[16];       // 我的天地飞遥控器实际rawdata范围是340-1704，中间值1024，这里还原遥控器的百分比
    bool flag_refresh = false; // 解析代码每次成功解析数据都会将此变量设为1，选择使用
} RcData_t;

class Sbus : public RemoteProduct
{
private:
    int sbus_fd_;
    RcData_t rc_data_;

    int SerialSetSpeciBaud(int baud);
    int OpenSerial(std::string SerialName, int Bitrate);
    int GetData();
    void SbusDecoderGetFrame(uint8_t *buf);
    void SbusDecoderGetByte(uint8_t data);
    void SbusDecoderGetBuf(uint8_t *buf, uint16_t len);

public:
    Sbus(std::string dev, bool debug = false);
    ~Sbus();
    RcData_t *GetChanelDataPtr();
    virtual bool Request(struct RemoteState &data);
};

// 生产sbus 遥控工厂
class SbusRemote : public RemoteFactory
{
public:
    RemoteProduct *CreateRemoteProduct()
    {
        return new Sbus("/dev/ttyS1", false);
    }
};

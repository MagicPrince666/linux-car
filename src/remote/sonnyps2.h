/**
 * @file sonnyps2.h
 * @author 黄李全 (846863428@qq.com)
 * @brief sonny ps2 2.4G 无线手柄
 * @version 0.1
 * @date 2023-01-05
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

#include <linux/spi/spidev.h>
#include <memory>
#include <iostream>
#include <stdint.h>
#include "RemoteFactory.h"

enum {
    PSXPAD_KEYSTATE_TYPE_DIGITAL = 0,
    PSXPAD_KEYSTATE_TYPE_ANALOG1,
    PSXPAD_KEYSTATE_TYPE_ANALOG2,
    PSXPAD_KEYSTATE_TYPE_UNKNOWN
};

const uint8_t PSX_CMD_INIT_PRESSURE[] = {0x01, 0x40, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_POLL[]          = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_ENTER_CFG[]     = {0x01, 0x43, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_EXIT_CFG[]      = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
const uint8_t PSX_CMD_ENABLE_MOTOR[]  = {0x01, 0x4D, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t PSX_CMD_ALL_PRESSURE[]  = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_AD_MODE[]       = {0x01, 0x44, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

struct PSXPad {
    uint8_t lu8PoolCmd[sizeof(PSX_CMD_POLL)];
    uint8_t lu8Response[sizeof(PSX_CMD_POLL)];
    uint8_t u8AttPinNo;
    uint8_t bAnalog;
    uint8_t bLock;
    uint8_t bMotor1Enable;
    uint8_t bMotor2Enable;
    uint8_t u8Motor1Level;
    uint8_t u8Motor2Level;
    uint8_t lu8EnableMotor[sizeof(PSX_CMD_ENABLE_MOTOR)];
    uint8_t lu8ADMode[sizeof(PSX_CMD_AD_MODE)];
};

struct PSXPads {
    int i_psx_fd;
    struct spi_ioc_transfer tTransfer;
    uint8_t u8PadsNum;
    struct PSXPad ltPad;
};

#define REVERSE_BIT(x) ((((x)&0x80) >> 7) | (((x)&0x40) >> 5) | (((x)&0x20) >> 3) | (((x)&0x10) >> 1) | (((x)&0x08) << 1) | (((x)&0x04) << 3) | (((x)&0x02) << 5) | (((x)&0x01) << 7))

class Ps2Remote: public RemoteProduct
{
public:
    Ps2Remote(std::string device, bool debug = false);
    ~Ps2Remote();

    int Init();
    void Uninit();

    virtual bool Request(struct RemoteState &data);

private:
    int SpiInit(struct spi_ioc_transfer *o_ptTransfer, const uint8_t i_u8Mode, const uint8_t i_u8Bits, const uint32_t i_u32Speed, const uint16_t i_u16Delay);
    void PSXPadsCommand(const uint8_t *i_lu8SendCmd, uint8_t *o_lu8Response, const uint8_t i_u8SendCmdLen);
    void PSXPadsSetADMode(const uint8_t i_bAnalog, const uint8_t i_bLock);
    void PSXPadsSetEnableMotor(const uint8_t i_bMotor1Enable, const uint8_t i_bMotor2Enable);
    void PSXPadsSetMotorLevel(const uint8_t i_u8Motor1Level, const uint8_t i_u8Motor2Level);
    void PSXPadsPool();
    void PSXPadsGetKeyState(struct RemoteState *o_ptKeyState);

    struct PSXPads tPSXPads_;
    std::time_t last_time_;
    uint32_t conter_;
};

// 生产索尼ps2无线遥控工厂
class SonnyRemote : public RemoteFactory
{
public:
	RemoteProduct* CreateRemoteProduct() {
		return new Ps2Remote("/dev/spidev1.0", false);
	}
};

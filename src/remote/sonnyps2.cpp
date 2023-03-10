#include <fcntl.h>
#include <getopt.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
#include <ctime>

#include "sonnyps2.h"
#include "gpio.h"

static void pabort(const char s[])
{
    perror(s);
    abort();
}

Ps2Remote::Ps2Remote(std::string device, bool debug)
: RemoteProduct(device, debug)
{
    last_time_ = std::time(0);
    conter_ = 0;
}

Ps2Remote::~Ps2Remote()
{
    Uninit();
}

int Ps2Remote::SpiInit(struct spi_ioc_transfer *o_ptTransfer, const uint8_t i_u8Mode, const uint8_t i_u8Bits, const uint32_t i_u32Speed, const uint16_t i_u16Delay)
{
    int ret = 0;

    if (!o_ptTransfer) {
        return -1;
    }

    /*
     * spi mode
     */
    ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_WR_MODE, &i_u8Mode);
    if (ret == -1) {
        pabort("can't set write spi mode");
    }
    ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_RD_MODE, &i_u8Mode);
    if (ret == -1) {
        pabort("can't get read spi mode");
    }

    /*
     * bits per word
     */
    ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_WR_BITS_PER_WORD, &i_u8Bits);
    if (ret == -1) {
        pabort("can't set write bits per word");
    }
    ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_RD_BITS_PER_WORD, &i_u8Bits);
    if (ret == -1) {
        pabort("can't get read bits per word");
    }

    /*
     * max speed hz
     */
    ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_WR_MAX_SPEED_HZ, &i_u32Speed);
    if (ret == -1) {
        pabort("can't set write max speed hz");
    }
    ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_RD_MAX_SPEED_HZ, &i_u32Speed);
    if (ret == -1) {
        pabort("can't get read max speed hz");
    }

    // printf("spi mode: %d\n", i_u8Mode);
    // printf("bits per word: %d\n", i_u8Bits);
    // printf("max speed: %d Hz (%d KHz)\n", i_u32Speed, i_u32Speed / 1000);

    /* set transfer settings */
    o_ptTransfer->delay_usecs   = i_u16Delay;
    o_ptTransfer->speed_hz      = i_u32Speed;
    o_ptTransfer->bits_per_word = i_u8Bits;

    return ret;
}

int Ps2Remote::Init()
{
    if (device_.empty()) {
        return -1;
    }
    memset((int8_t*)&tPSXPads_, 0, sizeof(tPSXPads_));
    printf("spi device: %s\n", device_.c_str());

    tPSXPads_.i_psx_fd = open(device_.c_str(), O_RDWR);
    if (tPSXPads_.i_psx_fd < 0) {
        pabort("can't open device");
    }

    /* mode 3, 125kbps */
    if (SpiInit(&(tPSXPads_.tTransfer), SPI_MODE_3, 8, 125000, 100) < 0) {
        pabort("can't init SPI");
    }

    tPSXPads_.u8PadsNum = 1;

    for (uint8_t u8Loc = 0; u8Loc < sizeof(PSX_CMD_POLL); u8Loc++) {
        tPSXPads_.ltPad.lu8PoolCmd[u8Loc] = PSX_CMD_POLL[u8Loc];
    }
    for (uint8_t u8Loc = 0; u8Loc < sizeof(PSX_CMD_ENABLE_MOTOR); u8Loc++) {
        tPSXPads_.ltPad.lu8EnableMotor[u8Loc] = PSX_CMD_ENABLE_MOTOR[u8Loc];
    }
    for (uint8_t u8Loc = 0; u8Loc < sizeof(PSX_CMD_AD_MODE); u8Loc++) {
        tPSXPads_.ltPad.lu8ADMode[u8Loc] = PSX_CMD_AD_MODE[u8Loc];
    }

    PSXPadsSetADMode(1, 1);
    return 0;
}

void Ps2Remote::Uninit()
{
    if(tPSXPads_.i_psx_fd) {
        close(tPSXPads_.i_psx_fd);
    }
}

void Ps2Remote::PSXPadsCommand(const uint8_t *i_lu8SendCmd, uint8_t *o_lu8Response, const uint8_t i_u8SendCmdLen)
{
    uint8_t u8SendBuf[0x100];

    if (!i_lu8SendCmd) {
        return;
    }
    if (!o_lu8Response) {
        return;
    }
    if (i_u8SendCmdLen == 0) {
        return;
    }

    for (uint8_t u8Loc = 0; u8Loc < i_u8SendCmdLen; u8Loc++) {
        u8SendBuf[u8Loc] = REVERSE_BIT(i_lu8SendCmd[u8Loc]);
    }

    /* set transfer settings */
    tPSXPads_.tTransfer.tx_buf    = (unsigned long)u8SendBuf;
    tPSXPads_.tTransfer.rx_buf    = (unsigned long)o_lu8Response;
    tPSXPads_.tTransfer.len       = i_u8SendCmdLen;
    tPSXPads_.tTransfer.cs_change = 0;

    int ret = ioctl(tPSXPads_.i_psx_fd, SPI_IOC_MESSAGE(1), &(tPSXPads_.tTransfer));
    if (ret < 1) {
        pabort("can't send spi message");
    }

    for (uint8_t u8Loc = 0; u8Loc < i_u8SendCmdLen; u8Loc++) {
        o_lu8Response[u8Loc] = REVERSE_BIT(o_lu8Response[u8Loc]);
    }
}

void Ps2Remote::PSXPadsPool()
{
    PSXPadsCommand(tPSXPads_.ltPad.lu8PoolCmd, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_POLL));
}

void Ps2Remote::PSXPadsSetADMode(const uint8_t i_bAnalog, const uint8_t i_bLock)
{
    tPSXPads_.ltPad.bAnalog = i_bAnalog ? 1 : 0;
    tPSXPads_.ltPad.bLock   = i_bLock ? 1 : 0;

    tPSXPads_.ltPad.lu8ADMode[3] = tPSXPads_.ltPad.bAnalog ? 0x01 : 0x00;
    tPSXPads_.ltPad.lu8ADMode[4] = tPSXPads_.ltPad.bLock ? 0x03 : 0x00;

    PSXPadsCommand(PSX_CMD_ENTER_CFG, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_ENTER_CFG));
    PSXPadsCommand(tPSXPads_.ltPad.lu8ADMode, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_AD_MODE));
    PSXPadsCommand(PSX_CMD_INIT_PRESSURE, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_INIT_PRESSURE));
    PSXPadsCommand(PSX_CMD_ALL_PRESSURE, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_ALL_PRESSURE));
    PSXPadsCommand(PSX_CMD_EXIT_CFG, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_EXIT_CFG));
}

void Ps2Remote::PSXPadsSetEnableMotor(const uint8_t i_bMotor1Enable, const uint8_t i_bMotor2Enable)
{
    tPSXPads_.ltPad.bMotor1Enable = i_bMotor1Enable ? 1 : 0;
    tPSXPads_.ltPad.bMotor2Enable = i_bMotor2Enable ? 1 : 0;

    tPSXPads_.ltPad.lu8EnableMotor[3] = tPSXPads_.ltPad.bMotor1Enable ? 0x00 : 0xFF;
    tPSXPads_.ltPad.lu8EnableMotor[4] = tPSXPads_.ltPad.bMotor2Enable ? 0x01 : 0xFF;

    PSXPadsCommand(PSX_CMD_ENTER_CFG, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_ENTER_CFG));
    PSXPadsCommand(tPSXPads_.ltPad.lu8EnableMotor, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_ENABLE_MOTOR));
    PSXPadsCommand(PSX_CMD_EXIT_CFG, tPSXPads_.ltPad.lu8Response, sizeof(PSX_CMD_EXIT_CFG));
}

void Ps2Remote::PSXPadsSetMotorLevel(const uint8_t i_u8Motor1Level, const uint8_t i_u8Motor2Level)
{
    tPSXPads_.ltPad.u8Motor1Level = i_u8Motor1Level ? 0xFF : 0x00;
    tPSXPads_.ltPad.u8Motor2Level = i_u8Motor2Level;

    tPSXPads_.ltPad.lu8PoolCmd[3] = tPSXPads_.ltPad.u8Motor1Level;
    tPSXPads_.ltPad.lu8PoolCmd[4] = tPSXPads_.ltPad.u8Motor2Level;
}

void Ps2Remote::PSXPadsGetKeyState(struct RemoteState *o_ptKeyState)
{
    if (!o_ptKeyState) {
        return;
    }

    PSXPadsPool();

    if(tPSXPads_.ltPad.lu8Response[1] == 0x73) { // 不需要处理
        o_ptKeyState->adslx          = 50; // 左摇杆x轴
        o_ptKeyState->adsly          = 50; // 左摇杆y轴
        o_ptKeyState->adsrx          = 50; // 右摇杆x轴
        o_ptKeyState->adsry          = 50; // 右摇杆y轴
        return;
    }

    if(tPSXPads_.ltPad.lu8Response[0] != 0xFF) {
        // printf("Please insert Ps2 Receive modules\n");
        o_ptKeyState->adslx          = 50; // 左摇杆x轴
        o_ptKeyState->adsly          = 50; // 左摇杆y轴
        o_ptKeyState->adsrx          = 50; // 右摇杆x轴
        o_ptKeyState->adsry          = 50; // 右摇杆y轴
        return;
    }

    if(tPSXPads_.ltPad.lu8Response[1] != 0xB9 && tPSXPads_.ltPad.lu8Response[1] != 0x73 && tPSXPads_.ltPad.lu8Response[1] != 0xA0) { // 可能是接收器没使能
        o_ptKeyState->adslx          = 50; // 左摇杆x轴
        o_ptKeyState->adsly          = 50; // 左摇杆y轴
        o_ptKeyState->adsrx          = 50; // 右摇杆x轴
        o_ptKeyState->adsry          = 50; // 右摇杆y轴
        return;
    }

    if (debug_) {
        for (int i = 0; i < 16; i++) {
            printf("%02X ", tPSXPads_.ltPad.lu8Response[i]);
        }
        printf("\n");
    }
    
    if((tPSXPads_.ltPad.lu8Response[1] == 0xB9) && (tPSXPads_.ltPad.lu8Response[2] == 0xAD) \
    && (tPSXPads_.ltPad.lu8Response[5] == 0x81) && (tPSXPads_.ltPad.lu8Response[6] == 0x81) \
    && (tPSXPads_.ltPad.lu8Response[7] == 0x81) && (tPSXPads_.ltPad.lu8Response[8] == 0x81)) {
        // 摇杆模式下 可以判断是不是都是0x81
        conter_++;
        std::time_t now = std::time(0);
        if(now - last_time_ > 15) { // 15s后判定失控
            last_time_ = now;
            if(conter_ > 500) {
                o_ptKeyState->lose_signal = true;
                printf("The same response = %d\n", conter_);
            }
            conter_ = 0;
        }
    } else if((tPSXPads_.ltPad.lu8Response[1] == 0xA0) && (tPSXPads_.ltPad.lu8Response[2] == 0xAD) \
    && (tPSXPads_.ltPad.lu8Response[3] == 0xFF) && (tPSXPads_.ltPad.lu8Response[4] == 0xFF) \
    && (tPSXPads_.ltPad.lu8Response[5] == 0xFF) && (tPSXPads_.ltPad.lu8Response[6] == 0xFF) \
    && (tPSXPads_.ltPad.lu8Response[7] == 0xFF) && (tPSXPads_.ltPad.lu8Response[8] == 0xFF)) {
        // 按键模式下 无法判断失控和长时间不操作 只能长时间没有操作 和 失控都需要重启
        conter_++;
        std::time_t now = std::time(0);
        if(now - last_time_ > 15) { // 15s后判定失控 
            last_time_ = now;
            if(conter_ > 500) {
                o_ptKeyState->lose_signal = true;
                printf("The same response = %d\n", conter_);
            }
            conter_ = 0;
        }
    } else {
        conter_ = 0;
    }
 
    if((tPSXPads_.ltPad.lu8Response[3] != 0xFF) || (tPSXPads_.ltPad.lu8Response[4] != 0xFF)) {
        o_ptKeyState->front         = (tPSXPads_.ltPad.lu8Response[3] & 0x10) ? false : true; // 前进按钮
        o_ptKeyState->back          = (tPSXPads_.ltPad.lu8Response[3] & 0x40) ? false : true; // 后退按钮
        o_ptKeyState->left          = (tPSXPads_.ltPad.lu8Response[3] & 0x80) ? false : true; // 左转按钮
        o_ptKeyState->right         = (tPSXPads_.ltPad.lu8Response[3] & 0x20) ? false : true; // 右转按钮
        o_ptKeyState->select        = (tPSXPads_.ltPad.lu8Response[3] & 0x01) ? false : true; // 选择按钮
        o_ptKeyState->start         = (tPSXPads_.ltPad.lu8Response[3] & 0x08) ? false : true; // 开始按钮
        o_ptKeyState->adl           = (tPSXPads_.ltPad.lu8Response[3] & 0x02) ? false : true; // 左摇杆按钮
        o_ptKeyState->adr           = (tPSXPads_.ltPad.lu8Response[3] & 0x04) ? false : true; // 右摇杆按钮
        o_ptKeyState->l1            = (tPSXPads_.ltPad.lu8Response[4] & 0x04) ? false : true; // 左顶部按钮1
        o_ptKeyState->l2            = (tPSXPads_.ltPad.lu8Response[4] & 0x01) ? false : true; // 左顶部按钮2
        o_ptKeyState->r1            = (tPSXPads_.ltPad.lu8Response[4] & 0x08) ? false : true; // 右顶部按钮1
        o_ptKeyState->r2            = (tPSXPads_.ltPad.lu8Response[4] & 0x02) ? false : true; // 右顶部按钮2
        o_ptKeyState->triangle      = (tPSXPads_.ltPad.lu8Response[4] & 0x10) ? false : true; // 三角按钮
        o_ptKeyState->quadrilateral = (tPSXPads_.ltPad.lu8Response[4] & 0x80) ? false : true; // 四边形按钮
        o_ptKeyState->rotundity     = (tPSXPads_.ltPad.lu8Response[4] & 0x20) ? false : true; // 园形按钮
        o_ptKeyState->fork          = (tPSXPads_.ltPad.lu8Response[4] & 0x40) ? false : true; // 叉按钮
    }

    if(tPSXPads_.ltPad.lu8Response[1] == 0xB9) {
        o_ptKeyState->adslx          = tPSXPads_.ltPad.lu8Response[7]/255.0 * 100; // 左摇杆x轴
        o_ptKeyState->adsly          = tPSXPads_.ltPad.lu8Response[8]/255.0 * 100; // 左摇杆y轴
        o_ptKeyState->adsrx          = tPSXPads_.ltPad.lu8Response[5]/255.0 * 100; // 右摇杆x轴
        o_ptKeyState->adsry          = tPSXPads_.ltPad.lu8Response[6]/255.0 * 100; // 右摇杆y轴
    } else {
        o_ptKeyState->adslx          = 50; // 左摇杆x轴
        o_ptKeyState->adsly          = 50; // 左摇杆y轴
        o_ptKeyState->adsrx          = 50; // 右摇杆x轴
        o_ptKeyState->adsry          = 50; // 右摇杆y轴
    }
}

bool Ps2Remote::Request(struct RemoteState &data)
{
    memset((int8_t*)&data, 0, sizeof(data));
    PSXPadsGetKeyState(&data);
    return false;
}

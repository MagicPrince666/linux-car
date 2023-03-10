#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#define termios asmtermios
#include <asm-generic/ioctls.h>
#include <asm-generic/termbits.h>
#undef termios
#include <linux/serial.h>
#include <termios.h>

#include "sbus.h"
#include "xepoll.h"

#define BAUDRATE 100000
#define DATA_LEN 25

Sbus::Sbus(std::string dev, bool debug) : RemoteProduct(dev, debug)
{
    std::cout << "Sbus init" << std::endl;
    sbus_fd_ = -1;

    sbus_fd_ = OpenSerial(device_, BAUDRATE);
    if (sbus_fd_ < 0) {
        std::cout << "can\'t open " << device_ << " !" << std::endl;
    }
    assert(sbus_fd_ > 0);
    memset((int8_t *)&rc_data_, 0, sizeof(rc_data_));
    MY_EPOLL.EpollAdd(sbus_fd_, std::bind(&Sbus::GetData, this));
}

Sbus::~Sbus()
{
    std::cout << "Sbus deinit" << std::endl;

    if (sbus_fd_ > 0) {
        MY_EPOLL.EpollDel(sbus_fd_);
        close(sbus_fd_);
    }
}

int Sbus::SerialSetSpeciBaud(int baud)
{
    struct termios2 tio;
    memset((int8_t *)&tio, 0, sizeof(tio));
    tio.c_cflag  = BOTHER | CS8 | CLOCAL | CREAD;
    tio.c_iflag  = IGNPAR;
    tio.c_oflag  = 0;
    tio.c_ispeed = baud;
    tio.c_ospeed = baud;
    return ioctl(sbus_fd_, TCSETS2, &tio);
}

int Sbus::OpenSerial(std::string SerialName, int Bitrate)
{
    /**
     * open the serial port
     */
    int fd = open(SerialName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        printf("Open SBUS input %s failed, status %d \n", SerialName.c_str(),
               (int)fd);
        fflush(stdout);
        return -1;
    }
    struct termios2 tio {
    };
    if (0 != ioctl(fd, TCGETS2, &tio)) {
        close(fd);
        return -1;
    }
    /**
     * Setting serial port,8E2, non-blocking.100Kbps
     */
    tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tio.c_iflag |= (INPCK | IGNPAR);
    tio.c_oflag &= ~OPOST;
    tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio.c_cflag &= ~(CSIZE | CRTSCTS | PARODD | CBAUD);
    /**
     * use BOTHER to specify speed directly in c_[io]speed member
     */
    tio.c_cflag |= (CS8 | CSTOPB | CLOCAL | PARENB | BOTHER | CREAD);
    tio.c_ispeed    = BAUDRATE;
    tio.c_ospeed    = BAUDRATE;
    tio.c_cc[VMIN]  = DATA_LEN;
    tio.c_cc[VTIME] = 0;
    if (0 != ioctl(fd, TCSETS2, &tio)) {
        close(fd);
        return -1;
    }
    sbus_fd_ = fd;
    return sbus_fd_;
}

int Sbus::GetData()
{
    uint8_t sbus_buf[100];
    int len = read(sbus_fd_, sbus_buf, sizeof(sbus_buf));
    if (len > 0) {
        memset((int8_t *)&rc_data_, 0, sizeof(rc_data_));
        SbusDecoderGetBuf(sbus_buf, len); // 解码一帧
        if (debug_) {
            for (int i = 0; i < 16; i++) {
                printf("%02d ", rc_data_.percent[i]);
            }
            printf("\n");
        }
    }

    return len;
}

bool Sbus::Request(struct RemoteState &data)
{
    memset((int8_t *)&data, 0, sizeof(data));
    data.lose_signal = false;               // 失控标识
    data.adslx       = rc_data_.percent[3]; // 左摇杆x轴
    data.adsly       = rc_data_.percent[1]; // 左摇杆y轴
    data.adsrx       = rc_data_.percent[0]; // 右摇杆x轴
    data.adsry       = rc_data_.percent[2]; // 右摇杆y轴
    // data.ads[12]    = {0};   // 扩展通道 sbus 16路通道都是模拟量
    memcpy(data.ads, rc_data_.percent + 4, 12);
    return false;
}

RcData_t *Sbus::GetChanelDataPtr()
{
    return &rc_data_;
}

void Sbus::SbusDecoderGetFrame(uint8_t *buf) // 传入一帧数据，解析成各个通道数据，一帧长度必然是25字节
{
    rc_data_.rawdata[0]  = ((buf[1] | buf[2] << 8) & 0x07FF);
    rc_data_.rawdata[1]  = ((buf[2] >> 3 | buf[3] << 5) & 0x07FF);
    rc_data_.rawdata[2]  = ((buf[3] >> 6 | buf[4] << 2 | buf[5] << 10) & 0x07FF);
    rc_data_.rawdata[3]  = ((buf[5] >> 1 | buf[6] << 7) & 0x07FF);
    rc_data_.rawdata[4]  = ((buf[6] >> 4 | buf[7] << 4) & 0x07FF);
    rc_data_.rawdata[5]  = ((buf[7] >> 7 | buf[8] << 1 | buf[9] << 9) & 0x07FF);
    rc_data_.rawdata[6]  = ((buf[9] >> 2 | buf[10] << 6) & 0x07FF);
    rc_data_.rawdata[7]  = ((buf[10] >> 5 | buf[11] << 3) & 0x07FF);
    rc_data_.rawdata[8]  = ((buf[12] | buf[13] << 8) & 0x07FF);
    rc_data_.rawdata[9]  = ((buf[13] >> 3 | buf[14] << 5) & 0x07FF);
    rc_data_.rawdata[10] = ((buf[14] >> 6 | buf[15] << 2 | buf[16] << 10) & 0x07FF);
    rc_data_.rawdata[11] = ((buf[16] >> 1 | buf[17] << 7) & 0x07FF);
    rc_data_.rawdata[12] = ((buf[17] >> 4 | buf[18] << 4) & 0x07FF);
    rc_data_.rawdata[13] = ((buf[18] >> 7 | buf[19] << 1 | buf[20] << 9) & 0x07FF);
    rc_data_.rawdata[14] = ((buf[20] >> 2 | buf[21] << 6) & 0x07FF);
    rc_data_.rawdata[15] = ((buf[21] >> 5 | buf[22] << 3) & 0x07FF);

    for (uint8_t i = 0; i < 16; i++) {
        rc_data_.percent[i] = (rc_data_.rawdata[i] - 300) * 100 / (1700 - 300);
    }
    rc_data_.flag_refresh = true;
}

void Sbus::SbusDecoderGetByte(uint8_t data)
{
    static int8_t off_set  = 0;   // 指向下个字节将要保存的位置
    static uint8_t buf[25] = {0}; // 保存一帧数据
    buf[off_set]           = data;
    // 判断当前缓存是否满足一帧的格式
    int8_t index = off_set + 1;
    if (index == 25) {
        index = 0;
    }

    if (buf[off_set] == 0x00 && buf[index] == 0x0F) { // 当前缓存数据满足0x0F开头且0x00结尾
        uint8_t buf_frame[25] = {0};
        memcpy(buf_frame, buf + index, 25 - index);
        memcpy(buf_frame + 25 - index, buf, index);
        SbusDecoderGetFrame(buf_frame);
    }

    off_set++;
    if (off_set == 25) {
        off_set = 0;
    }
}

void Sbus::SbusDecoderGetBuf(uint8_t *buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        SbusDecoderGetByte(buf[i]);
    }
}

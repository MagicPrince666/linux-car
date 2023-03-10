#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "gps.h"
#include "xepoll.h"

Gps::Gps(std::string dev) : device_(dev)
{
    uart_fd_ = OpenSerial();
    assert(uart_fd_ > 0);
    MY_EPOLL.EpollAdd(uart_fd_, std::bind(&Gps::Posttioning, this));
}

Gps::~Gps()
{
    if (uart_fd_ > 0) {
        MY_EPOLL.EpollDel(uart_fd_);
        close(uart_fd_);
    }
}

int Gps::OpenSerial()
{
    struct termios opt;

    int iFd = open(device_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (iFd < 0) {
        perror(device_.c_str());
        return -1;
    }

    tcgetattr(iFd, &opt);

    cfsetispeed(&opt, B115200);
    cfsetospeed(&opt, B115200);

    /*
     * raw mode
     */
    opt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    opt.c_oflag &= ~(OPOST);
    opt.c_cflag &= ~(CSIZE | PARENB);
    opt.c_cflag |= CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]  = DATA_LEN;
    opt.c_cc[VTIME] = 150;

    if (tcsetattr(iFd, TCSANOW, &opt) < 0) {
        return -1;
    }

    return iFd;
}

int Gps::Posttioning()
{
    uint8_t uart_buf[1024];
    int len = read(uart_fd_, uart_buf, sizeof(uart_buf));
    if (len > 0) {
        uart_buf[len] = 0;
        printf("%s\n", uart_buf);
    }
    return 0;
}

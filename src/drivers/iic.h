/**
 * @file iic.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-02-06
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */

#pragma once

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define IIC_DEVICE_NAME "/dev/i2c-3"

class IicBus
{
public:
    IicBus(std::string dev);
    ~IicBus();

    uint8_t IicInit();

    /**
     * @brief      iic bus read command
     * @param[in]  addr is the iic device write address
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       addr = device_address_7bits << 1
     */
    uint8_t IicReadCmd(uint8_t addr, uint8_t *buf, uint16_t len);

    /**
     * @brief      iic bus read
     * @param[in]  addr is the iic device write address
     * @param[in]  reg is the iic register address
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       addr = device_address_7bits << 1
     */
    uint8_t IicRead(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief      iic bus read with 16 bits register address
     * @param[in]  addr is the iic device write address
     * @param[in]  reg is the iic register address
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       addr = device_address_7bits << 1
     */
    uint8_t IicReadAddress16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief     iic bus write command
     * @param[in] addr is the iic device write address
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of the data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      addr = device_address_7bits << 1
     */
    uint8_t IicWriteCmd(uint8_t addr, uint8_t *buf, uint16_t len);

    /**
     * @brief     iic bus write
     * @param[in] addr is the iic device write address
     * @param[in] reg is the iic register address
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of the data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      addr = device_address_7bits << 1
     */
    uint8_t IicWrite(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief     iic bus write with 16 bits register address
     * @param[in] addr is the iic device write address
     * @param[in] reg is the iic register address
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of the data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      addr = device_address_7bits << 1
     */
    uint8_t IicWriteAddress16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);

private:
    std::string device_;
    int iic_fd_;
};

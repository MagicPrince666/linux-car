/**
 * @file spi.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-02-08
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#define SPI_DEVICE_NAME "/dev/spidev0.0"

/**
 * @brief spi mode type enumeration definition
 */
typedef enum {
    SPI_MODE_TYPE_0 = SPI_MODE_0, /**< mode 0 */
    SPI_MODE_TYPE_1 = SPI_MODE_1, /**< mode 1 */
    SPI_MODE_TYPE_2 = SPI_MODE_2, /**< mode 2 */
    SPI_MODE_TYPE_3 = SPI_MODE_3, /**< mode 3 */
} spi_mode_type_t;

class SpiBus
{
public:
    SpiBus(std::string device, spi_mode_type_t mode, uint32_t freq);
    ~SpiBus();

    /**
     * @brief      spi bus read command
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       none
     */
    uint8_t spiReadCmd(uint8_t *buf, uint16_t len);

    /**
     * @brief      spi bus read
     * @param[in]  reg is the spi register address
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       none
     */
    uint8_t spiRead(uint8_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief      spi bus read address 16
     * @param[in]  reg is the spi register address
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       none
     */
    uint8_t spiReadAddress16(uint16_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief     spi bus write command
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of the data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      none
     */
    uint8_t spiWriteCmd(uint8_t *buf, uint16_t len);

    /**
     * @brief     spi bus write
     * @param[in] reg is the spi register address
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of the data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      none
     */
    uint8_t spiWrite(uint8_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief     spi bus write address 16
     * @param[in] reg is the spi register address
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of the data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      none
     */
    uint8_t spiWriteAddress16(uint16_t reg, uint8_t *buf, uint16_t len);

    /**
     * @brief      spi bus write read
     * @param[in]  *in_buf points to an input buffer
     * @param[in]  in_len is the input length
     * @param[out] *out_buf points to an output buffer
     * @param[in]  out_len is the output length
     * @return     status code
     *             - 0 success
     *             - 1 write read failed
     * @note       none
     */
    uint8_t spiWriteRead(uint8_t *in_buf, uint32_t in_len, uint8_t *out_buf, uint32_t out_len);

    /**
     * @brief      spi transmit
     * @param[in]  *tx points to a tx buffer
     * @param[out] *rx points to a rx buffer
     * @param[in]  len is the length of the data buffer
     * @return     status code
     *             - 0 success
     *             - 1 transmit failed
     * @note       none
     */
    uint8_t spiTransmit(uint8_t *tx, uint8_t *rx, uint16_t len);

private:
    /**
     * @brief      spi bus init
     * @param[in]  *name points to a spi device name buffer
     * @param[out] *fd points to a spi device handle buffer
     * @param[in]  mode is the spi mode.
     * @param[in]  freq is the spi running frequence
     * @return     status code
     *             - 0 success
     *             - 1 init failed
     * @note       none
     */
    uint8_t init(spi_mode_type_t mode, uint32_t freq);

    std::string device_;
    int spi_fd_;
};

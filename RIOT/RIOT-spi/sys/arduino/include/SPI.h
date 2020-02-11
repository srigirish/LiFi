/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @file
 * @brief       Definition of the Arduino 'SPI' interface
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef SPI_H
#define SPI_H

extern "C" {
#include "arduino_board.h"
#include "periph/spi.h"
}

/**
 * @name    Arduino compatible SPI modes
 * @{
 */
#define SPI_MODE0       (0)     /**< CPOL=0, CPHA=0 */
#define SPI_MODE1       (1)     /**< CPOL=0, CPHA=1 */
#define SPI_MODE2       (2)     /**< CPOL=1, CPHA=0 */
#define SPI_MODE3       (3)     /**< CPOL=1, CPHA=1 */
/** @} */

/**
 * @name    Arduino compatible bit order values for SPI
 * @{
 */
#define MSBFIRST        (0)     /**< most significat bit first */
/** @} */

/**
 * @brief   Arduino SPI configuration interface
 */
class SPISettings {
    public:
    /**
     * @brief   Create a new SPI settings instance
     *
     * @param   clock       SPI clock in Hz to use
     * @param   bitOrder    Has to be `MSBFIST`, for compatibility only
     * @param   dataMode    SPI mode to use
     */
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);

    /**
     * @brief   Create a new SPI settings instance with default settings
     */
    SPISettings() : SPISettings(4000000, MSBFIRST, SPI_MODE0) { }

    private:
    spi_mode_t mode;
    spi_clk_t clock;

    friend class SPIClass;
};

/**
 * @brief   Arduino SPI interface
 */
class SPIClass {
    public:
    /**
     * @brief   Create a new SPI interface instance
     * @param   spi_dev     The RIOT SPI device to use under the hood
     */
    SPIClass(spi_t spi_dev);

    /**
     * @brief   Create a new SPI interface instance for SPI device 0
     * @param   uc_pinMISO  Ignored, for compatibility only
     * @param   uc_pinSCK   Ignored, for compatibility only
     * @param   uc_pinMOSI  Ignored, for compatibility only
     * @param   uc_pinSS    Ignored, for compatibility only
     * @param   uc_mux      Ignored, for compatibility only
     */
    SPIClass(uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI,
             uint8_t uc_pinSS, uint8_t uc_mux) : SPIClass(SPI_DEV(0))
    {
        (void)uc_pinMISO;
        (void)uc_pinSCK;
        (void)uc_pinMOSI;
        (void)uc_pinSS;
        (void)uc_mux;
    }

    /**
     * @brief   Transfer a single byte of data
     * @param[in]   data        Byte to send
     * @return  The received byte
     */
    uint8_t transfer(uint8_t data) {
        transfer(&data, sizeof(data));
        return data;
    }

    /**
     * @brief   Transfer two bytes of data
     * @param[in]       data        The two bytes to send
     * @return  The received two bytes
     */
    uint16_t transfer16(uint16_t data) {
        transfer(&data, sizeof(data));
        return data;
    }

    /**
     * @brief   Transfer data
     * @param[inout]    buf     Buffer containing the data to send, received
     *                          data will be written here
     * @param[in]       count   Number of bytes to send
     */
    void transfer(void *buf, size_t count);

    /**
     * @brief   Doesn't do anything, for compatibility only
     */
    void begin() { }

    /**
     * @brief   Doesn't do anything, for compatibility only
     */
    void end() { }

    /**
     * @brief   Acquires the SPI interface and applies the given settings
     * @param[in]   settings    Settings to apply
     */
    void beginTransaction(SPISettings settings);

    /**
     * @brief   Releases the SPI interface
     */
    void endTransaction();

    private:
    spi_t spi_dev;
};

extern SPIClass SPI;

#endif /* SPI_H */
/** @} */

/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_arduino_api
 * @{
 *
 * @file
 * @brief       Implementation of the Arduino 'SPI' interface
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

extern "C" {
#include "assert.h"
}

#include "SPI.h"

SPISettings::SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
{
    static const spi_clk_t clocks[] = {
        SPI_CLK_10MHZ, SPI_CLK_5MHZ, SPI_CLK_1MHZ, SPI_CLK_400KHZ
    };
    static const uint32_t steps [] = {
        7500000, 3000000, 700000, 250000
    };

    assert(bitOrder == MSBFIRST);
    switch(dataMode) {
        default:
        case SPI_MODE0:
            mode = SPI_MODE_0;
            break;
        case SPI_MODE1:
            mode = SPI_MODE_1;
            break;
        case SPI_MODE2:
            mode = SPI_MODE_2;
            break;
        case SPI_MODE3:
            mode = SPI_MODE_3;
            break;
    }

    for (uint8_t i = 0; i < sizeof(steps)/sizeof(steps[0]); i++) {
        if (clock >= steps[i]) {
            clock = clocks[i];
            return;
        }
    }

    clock = SPI_CLK_100KHZ;
}

SPIClass::SPIClass(spi_t spi_dev)
{
    this->spi_dev = spi_dev;
}

void SPIClass::beginTransaction(SPISettings settings)
{
    /* Call spi_acquire first to prevent data races */
    int retval = spi_acquire(spi_dev, SPI_CS_UNDEF,
                             settings.mode, settings.clock);
    /* No support for exceptions (at least on AVR), resort to assert() */
    assert(retval == SPI_OK);
}

void SPIClass::endTransaction()
{
    spi_release(spi_dev);
}

void SPIClass::transfer(void *buf, size_t count)
{
    spi_transfer_bytes(spi_dev, SPI_CS_UNDEF, false, buf, buf, count);
}

#if defined(SPI_NUMOF) && (SPI_NUMOF >= 1)
SPIClass SPI(SPI_DEV(0));
#endif

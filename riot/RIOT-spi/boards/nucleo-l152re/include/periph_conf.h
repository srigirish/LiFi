/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-l152re
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the nucleo-l152re board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_timer_tim5.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Clock system configuration
 * @{
 **/
#define CLOCK_HSI           (16000000U)             /* frequency of internal oscillator */
#define CLOCK_CORECLOCK     (32000000U)             /* targeted core clock frequency */
/*
 * 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz)
 *
 * LSE might not be available by default in early (C-01) Nucleo boards.
 * For newer revisions, an LSE crystal is present and CLOCK_LSE can be set to 1
 * if one wants to use it.
 */
#ifndef CLOCK_LSE
#define CLOCK_LSE           (0)
#endif
/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSI / CLOCK_PLL_DIV * CLOCK_PLL_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLDIV2
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMUL4
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 32MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 32MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 32MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name    DMA streams configuration
 * @{
 */
#ifdef MODULE_PERIPH_DMA
static const dma_conf_t dma_config[] = {
    { .stream = 1 },    /* DMA1 Channel 2 - SPI1_RX / USART3_TX */
    { .stream = 2 },    /* DMA1 Channel 3 - SPI1_TX */
    { .stream = 6 },    /* DMA1 Channel 7 - USART2_TX */
    { .stream = 4 },    /* DMA1 Channel 4 - USART1_TX */
};

#define DMA_0_ISR  isr_dma1_ch2
#define DMA_1_ISR  isr_dma1_ch3
#define DMA_2_ISR  isr_dma1_ch7
#define DMA_3_ISR  isr_dma1_ch4

#define DMA_NUMOF           ARRAY_SIZE(dma_config)
#endif
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = USART2,
        .rcc_mask = RCC_APB1ENR_USART2EN,
        .rx_pin   = GPIO_PIN(PORT_A, 3),
        .tx_pin   = GPIO_PIN(PORT_A, 2),
        .rx_af    = GPIO_AF7,
        .tx_af    = GPIO_AF7,
        .bus      = APB1,
        .irqn     = USART2_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 2,
        .dma_chan   = 2
#endif
    },
    {
        .dev      = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin   = GPIO_PIN(PORT_A, 9),
        .tx_pin   = GPIO_PIN(PORT_A, 10),
        .rx_af    = GPIO_AF7,
        .tx_af    = GPIO_AF7,
        .bus      = APB2,
        .irqn     = USART1_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 3,
        .dma_chan   = 2
#endif
    },
    {
        .dev      = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin   = GPIO_PIN(PORT_C, 11),
        .tx_pin   = GPIO_PIN(PORT_C, 10),
        .rx_af    = GPIO_AF7,
        .tx_af    = GPIO_AF7,
        .bus      = APB1,
        .irqn     = USART3_IRQn,
#ifdef MODULE_PERIPH_DMA
        .dma        = 0,
        .dma_chan   = 2
#endif
    },
};

#define UART_0_ISR          (isr_usart2)
#define UART_1_ISR          (isr_usart1)
#define UART_2_ISR          (isr_usart3)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 3) /* D3 */,  .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 10) /* D6 */, .cc_chan = 2 },
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0 },
                      { .pin = GPIO_UNDEF,                    .cc_chan = 0 } },
        .af       = GPIO_AF1,
        .bus      = APB1
    },
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4) /* D5 */, .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7) /* D9 */, .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 8),          .cc_chan = 2 },
                      { .pin = GPIO_PIN(PORT_C, 9),          .cc_chan = 3 } },
        .af       = GPIO_AF2,
        .bus      = APB1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   SPI configuration
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 32000000Hz */
        7,  /* -> 125000Hz */
        5,  /* -> 500000Hz */
        4,  /* -> 1000000Hz */
        2,  /* -> 4000000Hz */
        1   /* -> 8000000Hz */
    },
    {       /* for APB2 @ 32000000Hz */
        7,  /* -> 125000Hz */
        5,  /* -> 500000Hz */
        4,  /* -> 1000000Hz */
        2,  /* -> 4000000Hz */
        1   /* -> 8000000Hz */
    }
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_A, 7),
        .miso_pin = GPIO_PIN(PORT_A, 6),
        .sclk_pin = GPIO_PIN(PORT_A, 5),
        .cs_pin   = GPIO_UNDEF,
        .mosi_af  = GPIO_AF5,
        .miso_af  = GPIO_AF5,
        .sclk_af  = GPIO_AF5,
        .cs_af    = GPIO_AF5,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2,
#ifdef MODULE_PERIPH_DMA
        .tx_dma   = 1,
        .tx_dma_chan = 1,
        .rx_dma   = 0,
        .rx_dma_chan = 1,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name I2C configuration
  * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev            = I2C1,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 8),
        .sda_pin        = GPIO_PIN(PORT_B, 9),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C1EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C1_EV_IRQn
    },
    {
        .dev            = I2C2,
        .speed          = I2C_SPEED_NORMAL,
        .scl_pin        = GPIO_PIN(PORT_B, 10),
        .sda_pin        = GPIO_PIN(PORT_B, 11),
        .scl_af         = GPIO_AF4,
        .sda_af         = GPIO_AF4,
        .bus            = APB1,
        .rcc_mask       = RCC_APB1ENR_I2C2EN,
        .clk            = CLOCK_APB1,
        .irqn           = I2C2_EV_IRQn
    }
};

#define I2C_0_ISR           isr_i2c1_ev
#define I2C_1_ISR           isr_i2c2_ev

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name   ADC configuration
 * @{
 */
#define ADC_CONFIG {             \
    { GPIO_PIN(PORT_A, 0), 0 },  \
    { GPIO_PIN(PORT_A, 1), 1 },  \
    { GPIO_PIN(PORT_A, 4), 4 },  \
    { GPIO_PIN(PORT_B, 0), 8 },  \
    { GPIO_PIN(PORT_C, 1), 11 }, \
    { GPIO_PIN(PORT_C, 0), 10 }, \
}

#define ADC_NUMOF           (6U)
/** @} */

/**
 * @name   DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    { .pin = GPIO_PIN(PORT_A,  4), .chan = 0 },
    { .pin = GPIO_PIN(PORT_A,  5), .chan = 1 }
};

#define DAC_NUMOF           ARRAY_SIZE(dac_config)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

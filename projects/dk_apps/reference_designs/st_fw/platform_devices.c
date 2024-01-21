/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of customized devices connected to board.
 *
 * Copyright (C) 2019-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "platform_devices.h"

#if ((dg_configUART_ADAPTER == 1) && dg_configUSE_DGTL == 1)
#include "ad_uart.h"
#include "dgtl_config.h"

ad_uart_io_conf_t platform_dgtl_io_conf = {
        /* Rx UART2 */
        .rx = {
                .port = SER1_RX_PORT,
                .pin = SER1_RX_PIN,
                /* On */
                {
                        .mode = SER1_RX_MODE,
                        .function = SER1_RX_FUNC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = SER1_RX_MODE,
                        .function = SER1_RX_FUNC,
                        .high = true,
                },
        },
        /* Tx UART2 */
        .tx = {
                .port = SER1_TX_PORT,
                .pin = SER1_TX_PIN,
                /* On */
                {
                        .mode = SER1_TX_MODE,
                        .function = SER1_TX_FUNC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = SER1_TX_MODE,
                        .function = SER1_TX_FUNC,
                        .high = true,
                },
        },
        /* RTSN */
        .rtsn = {
                .port = DGTL_AUTO_FLOW_CONTROL ? SER1_RTS_PORT : HW_GPIO_PORT_NONE,
                .pin = DGTL_AUTO_FLOW_CONTROL ? SER1_RTS_PIN : HW_GPIO_PIN_NONE,
                /* On */
                {
                        .mode = DGTL_AUTO_FLOW_CONTROL ? SER1_RTS_MODE : HW_GPIO_MODE_NONE,
                        .function = SER1_RTS_FUNC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = DGTL_AUTO_FLOW_CONTROL ? SER1_RTS_MODE : HW_GPIO_MODE_NONE,
                        .function = SER1_RTS_FUNC,
                        .high = true,
                },
        },
        /* CTSN */
        .ctsn = {
                .port = DGTL_AUTO_FLOW_CONTROL ? SER1_CTS_PORT : HW_GPIO_PORT_NONE,
                .pin = DGTL_AUTO_FLOW_CONTROL ? SER1_CTS_PIN : HW_GPIO_PIN_NONE,
                /* On */
                {
                        .mode = DGTL_AUTO_FLOW_CONTROL ? SER1_CTS_MODE : HW_GPIO_MODE_NONE,
                        .function = SER1_CTS_FUNC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = DGTL_AUTO_FLOW_CONTROL ? SER1_CTS_MODE : HW_GPIO_MODE_NONE,
                        .function = SER1_CTS_FUNC,
                        .high = true,
                },
        },
        /* Voltage Level */
        .voltage_level = HW_GPIO_POWER_V33,
};

ad_uart_driver_conf_t platform_dgtl_uart_driver_conf = {
        {

                .baud_rate = HW_UART_BAUDRATE_115200,
                .data = HW_UART_DATABITS_8,
                .parity = HW_UART_PARITY_NONE,
                .stop = HW_UART_STOPBITS_1,
                .auto_flow_control = DGTL_AUTO_FLOW_CONTROL,
                .use_fifo = 1,
                .use_dma = 1,
                .tx_dma_channel = HW_DMA_CHANNEL_3,
                .rx_dma_channel = HW_DMA_CHANNEL_2,
                .tx_fifo_tr_lvl = 0,
                .rx_fifo_tr_lvl = 0,
        }
};

ad_uart_controller_conf_t platform_dgtl_controller_conf = {
        .id = SER1_UART,
        .io = &platform_dgtl_io_conf,
        .drv = &platform_dgtl_uart_driver_conf,
};

#endif /* (dg_configUART_ADAPTER == 1) && dg_configUSE_DGTL == 1 */

#if (dg_configGPADC_ADAPTER == 1)
#include "ad_gpadc.h"

const ad_gpadc_driver_conf_t drv_conf_se_temp = {
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .positive               = HW_GPADC_INP_DIE_TEMP,
        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_0V9,
        .result_mode            = HW_GPADC_RESULT_NORMAL,
        .temp_sensor            = 0, /* Not applicable */
        .sample_time            = HW_GPADC_DIE_TEMP_SMPL_TIME,
        .chopping               = true,
        .oversampling           = HW_GPADC_OVERSAMPLING_128_SAMPLES,
        .continuous             = false,
        .interval               = 0,
#if HW_GPADC_DMA_SUPPORT
        .dma_setup = NULL,
#endif
};

const ad_gpadc_controller_conf_t temp_sens_conf ={
        .id  = HW_GPADC_1,
        .io  = NULL,
        .drv = &drv_conf_se_temp,
};
#endif /* dg_configGPADC_ADAPTER == 1 */

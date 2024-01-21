/**
 ****************************************************************************************
 *
 * @file st_fw.h
 *
 * @brief ST Firmware API
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef ST_FW_H_
#define ST_FW_H_

#include "dgtl_msg.h"
#include "dgtl_pkt.h"

#define ST_VERSION_STR "1.1"

/* sleep modes */
typedef enum {
        ST_SLEEP_MODE_ACTIVE = 0,
        ST_SLEEP_MODE_EXTENDED_SLEEP,
        ST_SLEEP_MODE_DEEP_SLEEP
} st_sleep_mode_t;

/* operation types */
typedef enum {
        ST_OPERATION_READ_REG32 = 0,
        ST_OPERATION_WRITE_REG32,
        ST_OPERATION_READ_REG16,
        ST_OPERATION_WRITE_REG16
} st_operation_t;

/* GPIO modes */
typedef enum {
        ST_GPIO_MODE_INPUT = 0,
        ST_GPIO_MODE_INPUT_PULLUP,
        ST_GPIO_MODE_INPUT_PULLDOWN,
        ST_GPIO_MODE_OUTPUT,
        ST_GPIO_MODE_OUTPUT_PUSH_PULL,
        ST_GPIO_MODE_OUTPUT_OPEN_DRAIN
} st_gpio_mode_t;

/* LED IDs */
typedef enum {
        ST_LED_ID_1 = 0xF1,
        ST_LED_ID_2 = 0xF2,
        ST_LED_ID_3 = 0xF3,
} st_led_id_t;

/* Common event header */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_evt_t hci_evt;
        uint8_t            num_hci_cmd_packets;
        uint16_t           opcode;
} st_evt_hdr_t;

/* HCI Command Status Event */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_evt_t hci_evt;
        uint8_t            status;
        uint8_t            num_hci_cmd_packets;
        uint16_t           opcode;
} st_cmd_status_evt_t;

/* xtrim HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint8_t            operation;
        uint16_t           value;
} st_cmd_hci_xtrim_t;

/* xtrim HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint16_t      trim_value;
} st_evt_hci_xtrim_t;

/* sleep HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint8_t            mode;
        uint8_t            min;
        uint8_t            sec;
} st_cmd_hci_sleep_t;

/* read OTP command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint16_t           addr;
        uint8_t            word_count;
} st_cmd_hci_read_otp_t;

/* read OTP event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint8_t       status;
        uint8_t       word_count;
        uint32_t      data[];
} st_evt_hci_read_otp_t;

/* write OTP command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint16_t           addr;
        uint8_t            word_count;
        uint32_t           data[];
} st_cmd_hci_write_otp_t;

/* write OTP event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint8_t       status;
        uint8_t       word_count;
} st_evt_hci_write_otp_t;

/* read/write register command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint8_t            operation_mode;
        uint32_t           address;
        uint32_t           data;
} st_cmd_hci_rw_reg_t;

/* read/write register event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint8_t       operation_mode;
        uint8_t       status;
        uint32_t      data;
} st_evt_hci_rw_reg_t;

/* hci_firmware_version_get HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
} st_cmd_hci_firmware_version_get_t;

/* hci_firmware_version_get HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint8_t       ble_version_length;
        uint8_t       app_version_length;
        char          ble_fw_version[32];
        char          app_fw_version[32];
} st_evt_hci_firmware_version_get_t;

/* hci_custom_action HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint8_t            custom_action;
} st_cmd_hci_custom_action_t;

/* hci_custom_action HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint8_t       custom_action;
} st_evt_hci_custom_action_t;

/* hci_read_adc HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t hci_cmd;
        uint8_t            samples_nr;
        uint8_t            samples_period;
} st_cmd_hci_read_adc_t;

/* hci_read_adc HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t hdr;
        uint16_t      result;
} st_evt_hci_read_adc_t;

/* hci_sensor_test HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
        uint8_t                 iface;                  /* 0 = SPI, 1 = I2C */
        uint8_t                 rd_wr;                  /* 0 = RD, 1 = WR */
        uint8_t                 spi_clk_i2c_scl_port;
        uint8_t                 spi_clk_i2c_scl_pin;
        uint8_t                 spi_di_i2c_sda_port;
        uint8_t                 spi_di_i2c_sda_pin;
        uint8_t                 spi_do_port;
        uint8_t                 spi_do_pin;
        uint8_t                 spi_cs_port;
        uint8_t                 spi_cs_pin;
        uint8_t                 reg_addr;               /* The sensor register address to read or write. */
        uint8_t                 reg_data;               /* The data to write if rd_wr=1. */
        uint8_t                 i2c_slave_addr;
        uint8_t                 int_gpio_check;         /* 0 = Do not test DRDY/INT GPIO, 1 = Test DRDY/INT GPIO. */
        uint8_t                 int_port;               /* The DRDY/INT GPIO port. */
        uint8_t                 int_pin;                /* The DRDY/INT GPIO pin. */
        uint8_t                 gpio_lvl;               /* 0 = 3.3V, 1 = 1.8V. Used for SPI and DRDY/INT GPIOs. */
} st_cmd_hci_sensor_test_t;

/* hci_sensor_test HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 data;
        uint8_t                 error;                  /* 0=Command succeeded. Valid data. 0xFF=Command error. Invalid data. */
} st_evt_hci_sensor_test_t;

/* hci_gpio_set HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
        uint8_t                 gpio_pad;               /* An enumeration of all available GPIOs. GPIO Px_y is encoded as (`x` << 5) + `y`. E.g. P1_5 is encoded as 37 (0x25 in hex). */
        uint8_t                 mode;                   /* 0=HW_GPIO_MODE_INPUT, 1=HW_GPIO_MODE_INPUT_PULLUP, 2=HW_GPIO_MODE_INPUT_PULLDOWN, 3=HW_GPIO_MODE_OUTPUT, 4=HW_GPIO_MODE_OUTPUT_PUSH_PULL, 5=HW_GPIO_MODE_OUTPUT_OPEN_DRAIN. */
        uint8_t                 gpio_lvl;               /* The voltage rail. 0 = 3.3V, 1 = 1.8V. */
        uint8_t                 val;                    /* 0=Reset, 1=Set. Only valid in Output mode. */
        uint8_t                 pwm;                    /* 0=No PWM, Other=PWM frequency */
        uint8_t                 duty_cycle;             /* If PWM, PWM duty cycle. */
} st_cmd_hci_gpio_set_t;

/* hci_gpio_set HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 error;                  /* 0=Succeeded. 0xFF=Error. */
} st_evt_hci_gpio_set_t;

/* hci_gpio_read HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
        uint8_t                 gpio_pad;               /* An enumeration of all available GPIOs.GPIO Px_y is encoded as (`x` << 5) + `y`. E.g. P1_5 is encoded as 37 (0x25 in hex).  */
} st_cmd_hci_gpio_read_t;

/* hci_gpio_read HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 data;                   /* 0=Low, 1=High, 0xFF=Error. */
} st_evt_hci_gpio_read_t;

/* hci_uart_loop HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
        uint8_t                 data[0];                /* Variable length received data to be looped back to UART. */
} st_cmd_hci_uart_loop_t;

/* hci_uart_loop HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 data[0];                /* Variable length loop back UART data. */
} st_evt_hci_uart_loop_t;

/* hci_uart_baud HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
        uint8_t                 data;                   /* The UART baud rate. 0=9600, 1=19200, 2=57600, 3=115200, 4=1000000. */
} st_cmd_hci_uart_baud_t;

/* hci_uart_baud HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 error;                  /* 1=error, 0=Succeed. */
} st_evt_hci_uart_baud_t;

/* hci_gpio_wd HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
        uint8_t                 gpio_pad;               /* An enumeration of all available GPIOs. GPIO Px_y is encoded as (`x` << 5) + `y`. */
        uint8_t                 gpio_lvl;               /* The voltage rail. 0 = 3.3V, 1 = 1.8V. */
} st_cmd_hci_gpio_wd_t;

/* hci_gpio_wd HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 error;                  /* 0=Succeeded. 0xFF=Error. */
} st_evt_hci_gpio_wd_t;

/* hci_ext32khz_test HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
} st_cmd_hci_ext32khz_test_t;

/* hci_ext32khz_test HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t           hdr;
        uint8_t                 error;                  /* 0=Succeeded. 0xFF=Error. */
} st_evt_hci_ext32khz_test_t;

/* hci_get_die_temp HCI command */
typedef __PACKED_STRUCT {
        dgtl_pkt_hci_cmd_t      hci_cmd;
} st_cmd_hci_get_die_temp_t;

/* hci_get_die_temp HCI event response */
typedef __PACKED_STRUCT {
        st_evt_hdr_t            hdr;
        uint8_t                 error;                  /* 0=Succeeded. 0xFF=Error. */
        int8_t                  temp;                   /* Temperature in celsius degree */
} st_evt_hci_get_die_temp_t;


void st_parse_dgtl_msg(dgtl_msg_t *msg);

#endif /* ST_FW_H_ */

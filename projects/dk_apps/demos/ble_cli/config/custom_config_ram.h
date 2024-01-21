/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for RAM mode.
 *
 * Copyright (C) 2017-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      ( MODE_IS_RAM )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_NONE )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8F )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    ( 36788 )   /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#define dg_configGPADC_ADAPTER                  ( 1 )

#define CONFIG_RETARGET
#define dg_configUART_ADAPTER                   ( 1 )
#define dg_configUART_SOFTWARE_FIFO             ( 1 )
#ifndef dg_configUART2_SOFTWARE_FIFO_SIZE
#define dg_configUART2_SOFTWARE_FIFO_SIZE       ( 128 )
#endif /* dg_configUART2_SOFTWARE_FIFO_SIZE */
#define dg_configUSE_CLI                        ( 1 )
#define dg_configUSE_CONSOLE                    ( 1 )
#ifndef CONFIG_CLI_LINEBUF_SIZE
#define CONFIG_CLI_LINEBUF_SIZE                 ( 128 )
#endif /* CONFIG_CLI_LINEBUF_SIZE */
#ifndef CONFIG_CLI_QUEUE_LEN
#define CONFIG_CLI_QUEUE_LEN                    ( 3 )
#endif /* CONFIG_CLI_QUEUE_LEN */


/*************************************************************************************************\
 * BLE configuration
 */
#define defaultBLE_ATT_DB_CONFIGURATION         ( 0x30 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"
#endif /* CUSTOM_CONFIG_RAM_H_ */

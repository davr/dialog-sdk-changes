/**
 ****************************************************************************************
*
* @file custom_config_oqspi.h
*
* @brief Board Support Package. User Configuration file for cached OQSPI mode.
*
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
*
 ****************************************************************************************
*/
#ifndef CUSTOM_CONFIG_OQSPI_H_
#define CUSTOM_CONFIG_OQSPI_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * System configuration
 */
#  define dg_configUSE_LP_CLK                           ( LP_CLK_32768 )

#define dg_configEXEC_MODE                              ( MODE_IS_CACHED )
#define dg_configCODE_LOCATION                          ( NON_VOLATILE_IS_OQSPI_FLASH )
#define dg_configFLASH_CONNECTED_TO                     ( FLASH_CONNECTED_TO_1V8)

#define dg_configUSB_SUSPEND_MODE                       ( USB_SUSPEND_MODE_IDLE )
#define dg_configUSE_SYS_USB                            ( 1 )
#define dg_configUSE_USB_ENUMERATION                    ( 1 )

#define dg_configUSE_WDOG                               ( 1 )
#define dg_configWDOG_MAX_TASKS_CNT                     ( 7 )

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                                                   /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                           ( 30 * 1024 ) /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configUSE_HW_QSPI                            ( 0 )

#define dg_configFLASH_ADAPTER                          ( 1 )
#define dg_configNVMS_ADAPTER                           ( 1 )
#define dg_configNVMS_VES                               ( 1 )


/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_BLE_STORAGE
#define CONFIG_USE_BLE_CLIENTS

#define dg_configBLE_GATT_SERVER                        ( 0 )
#define dg_configBLE_OBSERVER                           ( 0 )
#define dg_configBLE_BROADCASTER                        ( 0 )
#define dg_configBLE_L2CAP_COC                          ( 0 )

#define defaultBLE_MAX_BONDED                           ( 1 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_OQSPI_H_ */

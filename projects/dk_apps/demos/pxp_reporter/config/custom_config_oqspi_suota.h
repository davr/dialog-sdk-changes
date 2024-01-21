/**
 ****************************************************************************************
 *
 * @file custom_config_oqspi_suota.h
 *
 * @brief Board Support Package. User Configuration file for cached OQSPI mode.
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_OQSPI_SUOTA_H_
#define CUSTOM_CONFIG_OQSPI_SUOTA_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      ( MODE_IS_CACHED )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_OQSPI_FLASH )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8F )

/* Uncomment the following line to enable State-of-Charge debugging or performance test */
//#define DEBUG_SOC
#if defined(DEBUG_SOC)
        #define CONFIG_RETARGET
#endif


#define dg_configUSE_SYS_CHARGER                ( 1 )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/*************************************************************************************************\
 * Available options for partition scheme with SUOTA support:
 *
 * USE_PARTITION_TABLE_8MB_WITH_SUOTA
 * USE_PARTITION_TABLE_128MB_WITH_SUOTA
 *
 * Note: The OQSPI flash device should be large enough to fit the partition scheme.
 */
#define USE_PARTITION_TABLE_8MB_WITH_SUOTA

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define SUOTA_HEAP_OVERHEAD                     ( 4096 )  /* Heap overhead while SUOTA is ongoing */
#define configTOTAL_HEAP_SIZE                   ( 30544 + SUOTA_HEAP_OVERHEAD )  /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#define dg_configNVPARAM_APP_AREA               ( 1 )
#define dg_configGPADC_ADAPTER                  ( 1 )

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_SERVICES

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )

#define defaultBLE_ATT_DB_CONFIGURATION         ( 0x10 )  // Peripheral Pref. Conn. Param. attribute
#define defaultBLE_PPCP_INTERVAL_MIN            ( BLE_CONN_INTERVAL_FROM_MS( 500 ) )    // 500 ms
#define defaultBLE_PPCP_INTERVAL_MAX            ( BLE_CONN_INTERVAL_FROM_MS( 750 ) )    // 750 ms
#define defaultBLE_PPCP_SLAVE_LATENCY           ( 0 )                                   // 0 events
#define defaultBLE_PPCP_SUP_TIMEOUT             ( BLE_SUPERVISION_TMO_FROM_MS( 6000 ) ) // 6000 ms

/* Use dynamic list for BLE events instead of FreeRTOS queues */
#define BLE_MGR_USE_EVT_LIST                    ( 1 )

#define dg_configSUOTA_SUPPORT                  ( 1 )

/*
 * SUOTA loader configuration:
 * - To enable SUOTA over GATT only, set SUOTA_VERSION to any version >= SUOTA_VERSION_1_1
 *      and leave SUOTA_PSM undefined.
 * - To enable SUOTA over GATT and L2CAP CoC, set SUOTA_VERSION to any version >= SUOTA_VERSION_1_2
 *      and also define SUOTA_PSM to match the desired PSM. In this case the central device
 *      can use either of both according to its preference.
 */
#define SUOTA_VERSION                           ( SUOTA_VERSION_1_3 )
#define SUOTA_PSM                               ( 0x81 )

#ifndef SUOTA_PSM
        #define dg_configBLE_L2CAP_COC          ( 0 )
#endif

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"
#endif /* CUSTOM_CONFIG_OQSPI_SUOTA_H_ */

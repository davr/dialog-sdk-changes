/**
 ****************************************************************************************
 *
 * @file custom_config_oqspi.h
 *
 * @brief Board Support Package. User Configuration file for execution from OQSPI.
 *
 * Copyright (C) 2021-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_OQSPI_H_
#define CUSTOM_CONFIG_OQSPI_H_

#include "bsp_definitions.h"

/*************************************************************************************************\
 * System and Services configuration
 */
#define dg_configUSB_SUSPEND_MODE               USB_SUSPEND_MODE_IDLE
#define dg_configUSE_USB_ENUMERATION            (1)
#define dg_configUSB_DMA_SUPPORT                (0)

#define dg_configUSE_SYS_CHARGER                (1)

#define dg_configUSE_SYS_TRNG                   (0)
#define dg_configUSE_SYS_DRBG                   (0)

#define dg_configUSE_WDOG                       (1)

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_OQSPI_FLASH
#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8F)
#define dg_configOQSPI_FLASH_POWER_DOWN         (1)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                   (40 * 1024)  /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */
#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

/* Disable QSPI/QSPI2 so that the application doesn't look for devices that are on the ProDK motherboard */
#define dg_configUSE_HW_QSPI                    (0)
#define dg_configUSE_HW_QSPI2                   (0)

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_OQSPI_H_ */

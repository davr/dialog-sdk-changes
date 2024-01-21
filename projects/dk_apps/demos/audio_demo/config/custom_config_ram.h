/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Custom configuration file for both non-FreeRTOS and FreeRTOS applications executing from RAM.
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
*/

#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)

#define dg_configUSE_HW_USB                     (0)

#define dg_configUSE_SW_CURSOR                  (1)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */

/*************************************************************************************************\
 * Peripheral specific config
 */
#define dg_configFLASH_ADAPTER                  (1)
#define dg_configNVMS_ADAPTER                   (1)
#define dg_configNVMS_VES                       (0)
#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configUSE_CLOCK_MGR			(1)
#define dg_configUSE_HW_DMA                     (1)

#define dg_configUSE_HW_I2C                     (1)
#define dg_configUSE_HW_PDM                     (1)
#define dg_configUSE_HW_SDADC                   (1)
#define dg_configUSE_HW_SRC                     (1)
#define dg_configUSE_HW_PCM                     (1)
#define dg_configUSE_SYS_AUDIO_MGR              (1)

#define configTOTAL_HEAP_SIZE    412288  /* This is the FreeRTOS Total Heap Size */

#define dg_configUSE_SYS_TRNG                   (0)
#define dg_configUSE_SYS_DRBG                   (0)

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */

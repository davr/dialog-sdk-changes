/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_RETARGET

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)

#define dg_configUSE_SW_CURSOR                  (1)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    14000   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

#define dg_configUSE_HW_USB                     (0)


#define dg_configUSE_SYS_TRNG                   (0)
#define dg_configUSE_SYS_DRBG                   (0)

#ifdef CONFIG_RETARGET
#define dg_configUSE_HW_DMA                     (1)
#define dg_configUSE_HW_UART                    (1)
#else
#define dg_configUSE_HW_DMA                     (0)
#define dg_configUSE_HW_UART                    (0)
#endif

#define dg_configLCDC_ADAPTER                   ( 1 )
#define dg_configUSE_HW_LCDC                    ( 1 )
#define dg_configUSE_GPU                        ( 1 )
#define dg_configI2C_ADAPTER                    ( 1 )
#define dg_configUSE_HW_I2C                     ( 1 )
#define dg_configUSE_HW_WKUP                    ( 1 )

#define dg_configUSE_HW_QSPI2                   ( 0 )
#define dg_configUSE_HW_OQSPI                   ( 0 )
#define dg_configUSE_HW_QSPI1                   ( 0 )

#define dg_configQSPIC2_DEV_AUTODETECT          ( 0 )

#define dg_configOQSPI_FLASH_AUTODETECT         ( 0 )
#define dg_configFLASH_AUTODETECT               ( 0 )

/*************************************************************************************************\
 * Display model selection. Note that one display model can be selected at a time.
 */
#define dg_configUSE_BOE139F454SM               ( 0 )
#define dg_configUSE_DT280QV10CT                ( 0 )
#define dg_configUSE_E120A390QSR                ( 0 )
#define dg_configUSE_ILI9341                    ( 0 )
#define dg_configUSE_LPM010M297B                ( 0 )
#define dg_configUSE_LPM012M134B                ( 0 )
#define dg_configUSE_LPM012M503A                ( 0 )
#define dg_configUSE_LPM013M091A                ( 0 )
#define dg_configUSE_LS013B7DH03                ( 0 )
#define dg_configUSE_LS013B7DH06                ( 0 )
#define dg_configUSE_MCT024L6W240320PML         ( 0 )
#define dg_configUSE_MRB3973_DBIB               ( 0 )
#define dg_configUSE_NHD43480272EFASXN          ( 0 )
#define dg_configUSE_PSP27801                   ( 0 )
#define dg_configUSE_T1D3BP006_DSPI             ( 0 )
#define dg_configUSE_T1D3BP006                  ( 1 )
#define dg_configUSE_T1D54BP002                 ( 0 )

#define LV_CONF_INCLUDE_SIMPLE
#define LV_LVGL_H_INCLUDE_SIMPLE
/*************************************************************************************************\
 * Touch controller selection. Note that one touch driver can be selected at a time.
 */
#ifdef PERFORMANCE_METRICS
#define dg_configUSE_TOUCH_SIMULATION           ( 0 )
#else
#define dg_configUSE_FT6206                     ( 0 )
#define dg_configUSE_FT5306                     ( 0 )
#define dg_configUSE_ZT2628                     ( 1 )
#endif


#if dg_configUSE_BOE139F454SM
#define AD_LCDC_DEFAULT_CLK                     sysclk_RCHS_96
#else
#define AD_LCDC_DEFAULT_CLK                     sysclk_RCHS_96
#endif

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */

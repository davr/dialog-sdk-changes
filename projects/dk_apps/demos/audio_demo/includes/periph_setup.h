/**
 ****************************************************************************************
 *
 * @file periph_setup.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef INCLUDES_PERIPH_SETUP_H_
#define INCLUDES_PERIPH_SETUP_H_

#include "hw_gpio.h"
#include "audio_task.h"


        /*  PCM Interface */
        #define PCM_CLK_PIN     HW_GPIO_PORT_0, HW_GPIO_PIN_13
        #define PCM_DO_PIN      HW_GPIO_PORT_0, HW_GPIO_PIN_12
        #define PCM_FSC_PIN     HW_GPIO_PORT_0, HW_GPIO_PIN_30
        #define PCM_DI_PIN      HW_GPIO_PORT_1, HW_GPIO_PIN_4

#   if DEMO_PDM_MIC || DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_PDM_RECORD_PLAYBACK
        /* PDM Interface */
        #define PDM_CLK_PIN         HW_GPIO_PORT_2, HW_GPIO_PIN_2
        #define PDM_DATA_PIN        HW_GPIO_PORT_2, HW_GPIO_PIN_3
#   endif

#   if DEMO_SDADC_MIC || DEMO_SDADC_RECORD_PLAYBACK
        /* SDADC Interface */
        // Enables MIC
        #define MIC_PWR          HW_GPIO_PORT_0, HW_GPIO_PIN_21
        #define PGA_INP          HW_GPIO_PORT_1, HW_GPIO_PIN_5
        #define PGA_INM          HW_GPIO_PORT_1, HW_GPIO_PIN_6
#   endif

        /* CODEC7218 Interface */
        //Enables CODEC7218
        #define DA_PWRON_PAD    HW_GPIO_PORT_0, HW_GPIO_PIN_28
        #define MCLK_PAD        HW_GPIO_PORT_0, HW_GPIO_PIN_20
        #define _GPIO_LVL       HW_GPIO_POWER_VDD1V8P
        #define I2C_SCL_PAD     HW_GPIO_PORT_1, HW_GPIO_PIN_12
        #define I2C_SDA_PAD     HW_GPIO_PORT_1, HW_GPIO_PIN_11

#   if DEMO_PDM_RECORD_PLAYBACK || DEMO_PCM_RECORD_PLAYBACK || \
        DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM || DEMO_SDADC_RECORD_PLAYBACK
        #define BTN_PIN         KEY1_PORT, KEY1_PIN
#   endif
#endif /* INCLUDES_PERIPH_SETUP_H_ */


 /**
 ****************************************************************************************
 *
 * @file config_audio_demo.c
 *
 * @brief Audio demo configuration
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "audio_task.h"

/* Check if the selected demo is valid */
# if (DEMO_MEM_TO_MEM + DEMO_PCM_MIC + DEMO_PCM_RECORD_PLAYBACK + DEMO_PDM_MIC + DEMO_PDM_RECORD_PLAYBACK + \
          DEMO_PDM_MIC_PARALLEL_SRCS + DEMO_SDADC_MIC > 1)
# error "Only one audio demo should be selected!!!"
# endif

#if (DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_PDM_RECORD_PLAYBACK || DEMO_MEM_TO_MEM) \
        && !dg_configUSE_HW_DMA
#error "This demo requires DMA hardware to be enabled. " \
        "Please revisit the application custom configuration."
#endif

#if (DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_PDM_MIC || DEMO_PDM_RECORD_PLAYBACK ) && !dg_configUSE_HW_PDM
#error "This demo requires PDM hardware to be enabled. " \
        "Please revisit the application custom configuration."
#endif

#if (DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_PCM_RECORD_PLAYBACK || DEMO_PCM_MIC || \
        DEMO_PDM_MIC || DEMO_SDADC_MIC) && !dg_configUSE_HW_PCM
#error "This demo requires PCM hardware to be enabled. " \
        "Please revisit the application custom configuration."
#endif

#if (DEMO_SDADC_MIC) && !dg_configUSE_HW_SDADC
#error "This demo requires SDADC hardware to be enabled. " \
        "Please revisit the application custom configuration."
#endif

/* Check if the selected system clock for PCM is valid */
#   if SYS_CLK_DIV1
#    if (SYS_CLK_RCHS64 + SYS_CLK_PLL160 + SYS_CLK_RCHS96 > 1)
#error "Only one DIV1 sys_clk should be selected!!!"
#    endif
#    if !SYS_CLK_PLL160
#     if (SYS_CLK_RCHS64 + SYS_CLK_RCHS96 > 1) || ((SYS_CLK_RCHS64 + SYS_CLK_RCHS96) == 0)
#error "One mode for rchs should be selected!!!"
#     endif
#    endif
#   endif

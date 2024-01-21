/**
 ****************************************************************************************
 *
 * @file audio_tack.c
 *
 * @brief Audio task
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdbool.h>
#include "audio_task.h"
#include "hw_clk.h"
#include "DA7218_driver.h"
#if (DEMO_PDM_RECORD_PLAYBACK || DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM || DEMO_SDADC_RECORD_PLAYBACK)
#include "hw_gpio.h"
#include "periph_setup.h"
#endif
#if (DEMO_MEM_TO_MEM) || (DEMO_PDM_MIC_PARALLEL_SRCS)
#include "demo_helpers.h"
#endif

const uint32_t flash_memory_channel_addr[2] = {0x0, NVMS_LOG_PART_SIZE / 2};
#define MEMORY_BASE     MEMORY_OQSPIC_S_BASE

#if DEMO_PDM_MIC_PARALLEL_SRCS
#define DEMO_PATH_1_DONE_NOTIF        (1 << 0)
#define DEMO_PATH_2_DONE_NOTIF        (1 << 1)
#else
#define DEMO_INPUT_DONE_NOTIF        (1 << 0)
#define DEMO_OUTPUT_DONE_NOTIF       (1 << 1)
#endif

/*
 * Initialize devices related with Audio
 */
static  sys_audio_device_t dev_1_in = {
        /* Select interface as input of path 1 */
        .device_type = AUDIO_INVALID,
        /* Initialize interfaces for input of path 1*/
#if DEMO_MEM_TO_MEM
        .memory_param = {
                .app_ud = 0,
                .bit_depth = BIT_DEPTH,
                .buff_addr[0] = 0,
                .buff_addr[1] = 0,
                .cb_buffer_len = DEMO_CHANNEL_DATA_BUF_CB_SIZE,
                .cb = 0,
                .dma_channel[0] = HW_DMA_CHANNEL_INVALID,
                .dma_channel[1] = HW_DMA_CHANNEL_INVALID,
                .dma_prio.use_prio = true,
                .dma_prio.prio[0] = HW_DMA_PRIO_3,
                .dma_prio.prio[1] = HW_DMA_PRIO_3,
                .sample_rate = SAMPLE_RATE_1,
                .stereo = true,
                .total_buffer_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
                .circular = false,
        },
#elif DEMO_PCM_MIC
        .pcm_param = {
                .bit_depth = BIT_DEPTH,
                .channel_delay = 0,
                .clk_generation = HW_PCM_CLK_GEN_FRACTIONAL,
#if SYS_CLK_DIV1
                .clock = HW_PCM_CLK_DIV1,
#else
                .clock = HW_PCM_CLK_DIVN,
#endif
                .cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT,
                .format = I2S_MODE,
                .fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT,
                .fsc_length = 2,
                .inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED,
                .inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL,
                .mode = MODE_MASTER,
                .output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL,
                .sample_rate = SAMPLE_RATE_1,
                .total_channel_num = 2,
        },
#elif DEMO_PDM_MIC || DEMO_PDM_MIC_PARALLEL_SRCS
        .pdm_param = {
                .mode = MODE_MASTER,
                .clk_frequency = PDM_FREQ,
                .in_delay = HW_PDM_DI_NO_DELAY,
                .swap_channel = 0,
        },
#elif DEMO_SDADC_MIC || DEMO_SDADC_RECORD_PLAYBACK
        .sdadc_param = {
                .pga_gain = HW_SDADC_PGA_GAIN_18dB,
                .pga_mode = HW_SDADC_PGA_MODE_DIFF,
        }
#endif
};

static sys_audio_device_t dev_1_out = {
        /* Select interface as output of path 1 */
        .device_type = AUDIO_INVALID,
        /* Initialize interfaces for output of path 1*/
#if (DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM)
        .memory_param = {
                .app_ud = 0,
                .bit_depth = BIT_DEPTH,
                .buff_addr[0] = 0,
                .buff_addr[1] = 0,
                .cb_buffer_len = DEMO_CHANNEL_DATA_BUF_CB_SIZE,
                .cb = 0,
                .dma_channel[0] = HW_DMA_CHANNEL_INVALID,
                .dma_channel[1] = HW_DMA_CHANNEL_INVALID,
                .sample_rate = SAMPLE_RATE_2,
                .stereo = true,
                .total_buffer_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
                .circular = false,
        },
#elif DEMO_SDADC_RECORD_PLAYBACK
        .memory_param = {
                .app_ud = 0,
                .bit_depth = BIT_DEPTH,
                .buff_addr[0] = 0,
                .buff_addr[1] = 0,
                .cb_buffer_len = DEMO_CHANNEL_DATA_BUF_CB_SIZE,
                .circular = false,
                .cb = 0,
                .dma_channel[0] = HW_DMA_CHANNEL_INVALID,
                .dma_channel[1] = HW_DMA_CHANNEL_INVALID,
                .sample_rate = SAMPLE_RATE_1,
                .stereo = false,
                .total_buffer_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
        },
#elif DEMO_PCM_MIC || DEMO_PDM_MIC || DEMO_SDADC_MIC
        .pcm_param = {
                .bit_depth = BIT_DEPTH,
                .channel_delay = 0,
                .clk_generation = HW_PCM_CLK_GEN_FRACTIONAL,
#if SYS_CLK_DIV1
                .clock = HW_PCM_CLK_DIV1,
#else
                .clock = HW_PCM_CLK_DIVN,
#endif
                .cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT,
                .format = I2S_MODE,
                .fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT,
                .fsc_length = 2,
                .inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED,
                .inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL,
                .mode = MODE_MASTER,
                .output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL,
                .sample_rate = SAMPLE_RATE_1,
                .total_channel_num = 2,
        },
#endif
};

static sys_audio_device_t dev_2_in = {
        /* Select interface as input of path 2 */
        .device_type = AUDIO_INVALID,
        /* Initialize interfaces for input of path 2*/
#if DEMO_PDM_MIC_PARALLEL_SRCS
        .memory_param = {
                .app_ud = 0,
                .bit_depth = BIT_DEPTH,
                .buff_addr[0] = 0,
                .buff_addr[1] = 0,
                .cb_buffer_len = DEMO_CHANNEL_DATA_BUF_CB_SIZE,
                .cb = 0,
                .dma_channel[0] = HW_DMA_CHANNEL_INVALID,
                .dma_channel[1] = HW_DMA_CHANNEL_INVALID,
                .sample_rate = SAMPLE_RATE_2,
                .stereo = true,
                .total_buffer_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
                .circular = false,
        },
#endif
};

static sys_audio_device_t dev_2_out = {
        /* Select interface as output of path 2 */
        .device_type = AUDIO_INVALID,
        /* Initialize interfaces for output of path 2*/
#if DEMO_PDM_MIC_PARALLEL_SRCS
        .pcm_param = {
                .bit_depth = BIT_DEPTH,
                .channel_delay = 0,
                .clk_generation = HW_PCM_CLK_GEN_FRACTIONAL,
#if SYS_CLK_DIV1
                .clock = HW_PCM_CLK_DIV1,
#else
                .clock = HW_PCM_CLK_DIVN,
#endif
                .cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT,
                .format = I2S_MODE,
                .fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT,
                .fsc_length = 0,
                .inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED,
                .inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL,
                .mode = MODE_MASTER,
                .output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL,
                .sample_rate = SAMPLE_RATE_1,
                .total_channel_num = 2,
        },
#endif
};

context_audio_demo_t context_audio_demo = {
        .audio_dev = 0,
        .audio_task = 0,
        .paths_cfg = {
                .audio_path[PATH_1] = {
                        .dev_in = (sys_audio_device_t*)&dev_1_in,
                        .dev_out = (sys_audio_device_t*)&dev_1_out,
                },
                .audio_path[PATH_2] = {
                        .dev_in = (sys_audio_device_t*)&dev_2_in,
                        .dev_out = (sys_audio_device_t*)&dev_2_out,
                },
        },
};

#if DEMO_PDM_MIC || DEMO_PCM_MIC || DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_RECORD_PLAYBACK || \
        DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM || DEMO_SDADC_MIC || DEMO_SDADC_RECORD_PLAYBACK
static void audio_mgr_start(PATH_NO idx)
{
        printf("\n\r\n\r>>> Start PATH_%d <<<\n\r", idx+1);
        printf_settings(context_audio_demo.paths_cfg.audio_path[idx].dev_in, INPUT_DEVICE);
        printf_settings(context_audio_demo.paths_cfg.audio_path[idx].dev_out, OUTPUT_DEVICE);

        /* Enable devices of the required path */
        ASSERT_ERROR(sys_audio_mgr_start(idx));
}

static void audio_mgr_stop(PATH_NO idx)
{
        printf("\n\r\n\r>>> Stop PATH_%d <<<\n\r", idx+1);

        /* Disable devices of the required path */
        ASSERT_ERROR(sys_audio_mgr_stop(idx));
}

#if DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_RECORD_PLAYBACK || \
        DEMO_MEM_TO_MEM || DEMO_SDADC_RECORD_PLAYBACK
volatile uint32_t notified_value = 0x00;
static void audio_buffer_ready_cb(sys_audio_mgr_buffer_data_block_t *buff_data_block, void *app_ud)
{
        context_audio_demo_t *audio_demo = app_ud;
        uint8_t chs = 2;

# if DEMO_SDADC_RECORD_PLAYBACK
                chs = 1;
# endif
        audio_demo->available_to_read += buff_data_block->buff_len_cb;

        if (audio_demo->available_to_read == chs * buff_data_block->buff_len_total) {
                context_audio_demo.available_to_read = 0;
#  if DEMO_PDM_MIC_PARALLEL_SRCS
                OS_TASK_NOTIFY_FROM_ISR(audio_demo->audio_task, DEMO_PATH_1_DONE_NOTIF, OS_NOTIFY_SET_BITS);
#  else
                OS_TASK_NOTIFY_FROM_ISR(audio_demo->audio_task, DEMO_INPUT_DONE_NOTIF, OS_NOTIFY_SET_BITS);
#  endif
        }
}

# if DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM
#if DEMO_MEM_TO_MEM
static void audio_buffer_ready_cb_2(sys_audio_mgr_buffer_data_block_t *buff_data_block, void *app_ud)
#elif DEMO_PDM_MIC_PARALLEL_SRCS
static void audio_buffer_ready_cb_path_2(sys_audio_mgr_buffer_data_block_t *buff_data_block, void *app_ud)
#endif
{
        context_audio_demo_t *audio_demo = app_ud;

        uint8_t chs = 2;

        audio_demo->available_to_read_2 += buff_data_block->buff_len_cb;

        if (audio_demo->available_to_read_2 == chs * buff_data_block->buff_len_total) {
                context_audio_demo.available_to_read_2 = 0;
#   if DEMO_PDM_MIC_PARALLEL_SRCS
                OS_TASK_NOTIFY_FROM_ISR(audio_demo->audio_task, DEMO_PATH_2_DONE_NOTIF, OS_NOTIFY_SET_BITS);
#   else
                OS_TASK_NOTIFY_FROM_ISR(audio_demo->audio_task, DEMO_OUTPUT_DONE_NOTIF, OS_NOTIFY_SET_BITS);
#   endif
        }
}
# endif

__STATIC_INLINE bool audio_task_get_pin_status(HW_GPIO_PORT port, HW_GPIO_PIN pin)
{
        if (!hw_gpio_get_pin_status(port, pin)) {
                return true;
        }
        return false;
}
#endif

#if (DEMO_PDM_MIC || DEMO_PCM_MIC || DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM || DEMO_SDADC_MIC)
# if (DEMO_MEM_TO_MEM)
OS_TASK_FUNCTION(audio_task, pvParameters)
{
        /*
         * Initialize path 1
         */
        uint8_t i;

        /* Select MEMORY as input device*/
        dev_1_in.device_type = AUDIO_MEMORY;

        /* Initialize additional parameters for memory output*/

        dev_1_in.memory_param.app_ud = &context_audio_demo;
        dev_1_in.memory_param.cb = audio_buffer_ready_cb;

        // Channels 1, 3, 5 or 7 must be used for SRC input
        dev_1_in.memory_param.dma_channel[0] = HW_DMA_CHANNEL_3;
        dev_1_in.memory_param.dma_channel[1] = HW_DMA_CHANNEL_1;

        uint32_t notif_val = 0;
        /* Select MEMORY output device*/
        dev_1_out.device_type = AUDIO_MEMORY;

        /* Initialize additional parameters for memory output for path 1*/
        dev_1_out.memory_param.app_ud = &context_audio_demo;
        dev_1_out.memory_param.cb = audio_buffer_ready_cb_2;

        dev_1_out.memory_param.dma_channel[0] = HW_DMA_CHANNEL_2;
        dev_1_out.memory_param.dma_channel[1] = HW_DMA_CHANNEL_0;

        uint32_t size = dev_1_in.memory_param.total_buffer_len;

        for (i = 0 ; i < (dev_1_in.memory_param.stereo ? 2 : 1); i++) {

                if (dev_1_in.memory_param.dma_channel[i] != HW_DMA_CHANNEL_INVALID) {

                        if (size > OS_GET_FREE_HEAP_SIZE()) {
                                printf("No enough heap size %ld, reduce num of channel or memory buffer", size);
                                dev_1_in.memory_param.buff_addr[i] = 0;
                                return;
                        }

                        dev_1_in.memory_param.buff_addr[i] = (uint32_t)OS_MALLOC(size);

                        if (!dev_1_in.memory_param.buff_addr[i]) {
                                ASSERT_ERROR(0);
                        }

                        size = (dev_1_in.memory_param.total_buffer_len) * (dev_1_out.memory_param.bit_depth / 8);

                        if (dev_1_out.memory_param.dma_channel[i] != HW_DMA_CHANNEL_INVALID) {

                                if (size > OS_GET_FREE_HEAP_SIZE()) {
                                        printf("No enough heap size %ld, reduce num of channel or memory buffer", size);
                                        dev_1_out.memory_param.buff_addr[i] = 0;
                                        return;
                                }

                                dev_1_out.memory_param.buff_addr[i] = (uint32_t)OS_MALLOC(size);

                                if (!dev_1_out.memory_param.buff_addr[i]) {
                                        ASSERT_ERROR(0);
                                        return;
                                }

# if (SIN_DATA_SR_8K_F_1K || SIN_DATA_SR_96K_F_1K || SIN_DATA_SR_192K_F_12K || SIN_DATA_SR_192K_F_19_2K || SIN_DATA_SR_8K_TO_96K_F_1K)
                        demo_set_sinusoidal_pattern((uint32_t*)dev_1_in.memory_param.buff_addr[i],
                                dev_1_in.memory_param.total_buffer_len,
                                pcm_sin_data, ARRAY_LENGTH(pcm_sin_data),
                                dev_1_in.memory_param.sample_rate,
                                SIGNAL_INPUT_FREQ,
                                dev_1_in.memory_param.bit_depth);
# elif  (COS_DATA_SR_8K_F_1K || COS_DATA_SR_192K_F_12K || COS_DATA_SR_192K_F_19_2K)
                        demo_set_sinusoidal_pattern((uint32_t*)dev_1_in.memory_param.buff_addr[i],
                                dev_1_in.memory_param.total_buffer_len,
                                pcm_sin_data, ARRAY_LENGTH(pcm_cos_data),
                                dev_1_in.memory_param.sample_rate,
                                SIGNAL_INPUT_FREQ,
                                dev_1_in.memory_param.bit_depth);
# endif
                }}
        }

        dev_1_out.memory_param.total_buffer_len = size;

        uint8_t idx_1 = sys_audio_mgr_open_path(&dev_1_in, &dev_1_out, SRC_AUTO);

        for (;;) {
                if (audio_task_get_pin_status(BTN_PIN)) {

                        OS_BASE_TYPE xResult;

                        /* Start PATH_1 */
                        audio_mgr_start(idx_1);

                        while (notif_val != DEMO_OUTPUT_DONE_NOTIF) {

                                /*
                                 * Wait on any of the event group bits, then clear them all
                                 */
                                xResult = OS_TASK_NOTIFY_WAIT(OS_TASK_NOTIFY_NONE, OS_TASK_NOTIFY_ALL_BITS,
                                        &notif_val, OS_TASK_NOTIFY_FOREVER);

                                OS_ASSERT(xResult == OS_OK);

                                /* Check if the data are received */
                                if (notif_val & DEMO_OUTPUT_DONE_NOTIF) {
                                        audio_mgr_stop(idx_1);
                                }
                        }
                        notif_val = 0x00;
                        printf("For a new record, press the button\n");
                }
        }
}
# else
OS_TASK_FUNCTION(audio_task, pvParameters)
{
        /*
         * Initialize path 1
         */
# if DEMO_PDM_MIC || DEMO_PDM_MIC_PARALLEL_SRCS

        /* Select PDM input device*/
        dev_1_in.device_type = AUDIO_PDM;

# elif DEMO_PCM_MIC
        /* Select PCM input device*/
        dev_1_in.device_type = AUDIO_PCM;

# elif DEMO_SDADC_MIC || DEMO_SDADC_RECORD_PLAYBACK
        /* Select SDADC input device*/
        dev_1_in.device_type = AUDIO_SDADC;
# endif

# if DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_MEM_TO_MEM || DEMO_SDADC_RECORD_PLAYBACK
#  if DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_SDADC_RECORD_PLAYBACK
        uint8_t i;
#  endif
        uint32_t notif_val = 0;
        /* Select MEMORY output device*/
        dev_1_out.device_type = AUDIO_MEMORY;

        /* Initialize additional parameters for memory output for path 1*/
        dev_1_out.memory_param.app_ud = &context_audio_demo;

# if DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_SDADC_RECORD_PLAYBACK
        dev_1_out.memory_param.cb = audio_buffer_ready_cb;
# endif

#  if DEMO_PDM_MIC_PARALLEL_SRCS
        // Channels 0, 2, 4 or 6 must be used for SRC output
        dev_1_out.memory_param.dma_channel[0] = HW_DMA_CHANNEL_2;
        dev_1_out.memory_param.dma_channel[1] = HW_DMA_CHANNEL_0;
        uint32_t size = dev_1_out.memory_param.total_buffer_len;
#  elif DEMO_SDADC_RECORD_PLAYBACK
        dev_1_out.memory_param.dma_channel[0] = HW_DMA_CHANNEL_2;
        uint32_t size = dev_1_out.memory_param.total_buffer_len;
#  endif

        for (i = 0 ; i < (dev_1_out.memory_param.stereo ? 2 : 1); i++) {

                if (dev_1_out.memory_param.dma_channel[i] != HW_DMA_CHANNEL_INVALID) {

                        if (size > OS_GET_FREE_HEAP_SIZE()) {
                                printf("No enough heap size %ld, reduce num of channel or memory buffer", size);
                                dev_1_out.memory_param.buff_addr[i] = 0;
                                return;
                        }

                        dev_1_out.memory_param.buff_addr[i] = (uint32_t)OS_MALLOC(size);

                        if (!dev_1_out.memory_param.buff_addr[i]) {
                                ASSERT_ERROR(0);
                                return;
                        }
                }
        }
# else
        /* Select PCM output device*/
        dev_1_out.device_type = AUDIO_PCM;
# endif

# if DEMO_PDM_MIC_PARALLEL_SRCS
        /*
         * Initialize path 2
         */
        /* Select MEMORY input device*/
        dev_2_in.device_type = AUDIO_MEMORY;

        /* Initialize additional parameters for memory output for path 2*/

        dev_2_in.memory_param.app_ud = &context_audio_demo;
        dev_2_in.memory_param.cb = audio_buffer_ready_cb_path_2;

        // Channels 1, 3, 5 or 7 must be used for SRC input
        dev_2_in.memory_param.dma_channel[0] = HW_DMA_CHANNEL_7;
        dev_2_in.memory_param.dma_channel[1] = HW_DMA_CHANNEL_5;
        size = dev_2_in.memory_param.total_buffer_len;

        for (i = 0 ; i < (dev_2_in.memory_param.stereo ? 2 : 1); i++) {

                if (dev_2_in.memory_param.dma_channel[i] != HW_DMA_CHANNEL_INVALID) {

                        if (size > OS_GET_FREE_HEAP_SIZE()) {
                                printf("No enough heap size %ld, reduce num of channel or memory buffer", size);
                                dev_2_in.memory_param.buff_addr[i] = 0;
                                return;
                        }

                        dev_2_in.memory_param.buff_addr[i] = (uint32_t)OS_MALLOC(size);

                        if (!dev_2_in.memory_param.buff_addr[i]) {
                                ASSERT_ERROR(0);
                        }

# if (SIN_DATA_SR_8K_F_1K || SIN_DATA_SR_96K_F_1K || SIN_DATA_SR_192K_F_12K || SIN_DATA_SR_192K_F_19_2K || SIN_DATA_SR_8K_TO_96K_F_1K)
                        demo_set_sinusoidal_pattern((uint32_t*)dev_2_in.memory_param.buff_addr[i],
                                dev_2_in.memory_param.total_buffer_len,
                                pcm_sin_data, ARRAY_LENGTH(pcm_sin_data),
                                dev_2_in.memory_param.sample_rate,
                                SIGNAL_INPUT_FREQ,
                                dev_2_in.memory_param.bit_depth);
# elif  (COS_DATA_SR_8K_F_1K || COS_DATA_SR_192K_F_12K || COS_DATA_SR_192K_F_19_2K)
                        demo_set_sinusoidal_pattern((uint32_t*)dev_2_in.memory_param.buff_addr[i],
                                dev_2_in.memory_param.total_buffer_len,
                                pcm_sin_data, ARRAY_LENGTH(pcm_cos_data),
                                dev_2_in.memory_param.sample_rate,
                                SIGNAL_INPUT_FREQ,
                                dev_2_in.memory_param.bit_depth);
# endif
                }
        }

        /* Select PCM output device*/
        dev_2_out.device_type = AUDIO_PCM;
# endif /* end of DEMO_PDM_MIC_PARALLEL_SRCS */

        uint8_t idx_1 = sys_audio_mgr_open_path(&dev_1_in, &dev_1_out, SRC_1);
#  if DEMO_PDM_MIC_PARALLEL_SRCS
        uint8_t idx_2 = sys_audio_mgr_open_path(&dev_2_in, &dev_2_out, SRC_2);
#  endif

        /* Enable Audio Codec7218 */
        DA7218_Enable();


        for (;;) {
# if DEMO_PDM_MIC_PARALLEL_SRCS
                if (audio_task_get_pin_status(BTN_PIN)) {

                        OS_BASE_TYPE xResult;
# endif /* end of DEMO_PDM_MIC_PARALLEL_SRCS */

                        /* Start PATH_1 */
                        audio_mgr_start(idx_1);

# if DEMO_PDM_MIC_PARALLEL_SRCS
                        uint8_t cnt[MAX_NO_PATHS] = {0, 0};
                        /* Start PATH_2 */
                        audio_mgr_start(idx_2);

                        while (notif_val == 0x00) {

                                /*
                                 * Wait on any of the event group bits, then clear them all
                                 */
                                xResult = OS_TASK_NOTIFY_WAIT(OS_TASK_NOTIFY_NONE, OS_TASK_NOTIFY_ALL_BITS,
                                        &notif_val, OS_TASK_NOTIFY_FOREVER);

                                OS_ASSERT(xResult == OS_OK);

                                if (notif_val & DEMO_PATH_1_DONE_NOTIF) {
                                        audio_mgr_stop(idx_1);
                                        cnt[idx_1]++;
                                        if (cnt[idx_2] == 0) {
                                                notif_val = 0x00;
                                        }
                                }

                                if (notif_val & DEMO_PATH_2_DONE_NOTIF) {
                                        audio_mgr_stop(idx_2);
                                        cnt[idx_2]++;
                                        if (cnt[idx_1] == 0) {
                                                notif_val = 0x00;
                                        }
                                }
                        }
                        notif_val = 0x00;
                        printf("For a new record, press the button\n");
                }
                /* end of DEMO_PDM_MIC_PARALLEL_SRCS */
#else

        OS_DELAY_MS(10000);

        audio_mgr_stop(idx_1);
#endif
        }
}
# endif
#elif DEMO_PDM_RECORD_PLAYBACK || DEMO_PCM_RECORD_PLAYBACK || DEMO_SDADC_RECORD_PLAYBACK
static void  copy_ram_pattern_to_qspi(void)
{
        bool success = true;
        uint8_t i;
        uint32_t buff_size = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE;
        nvms_t part = ad_nvms_open(NVMS_LOG_PART);

        for (i = 0 ; i < (dev_1_out.memory_param.stereo ? 2 : 1); i++) {

                if (dev_1_out.memory_param.dma_channel[i] != HW_DMA_CHANNEL_INVALID) {
                        uint32_t flash_memory_addr = flash_memory_channel_addr[i];

                        /* addr is any address in partition address space
                         * buf can be any address in memory including QSPI mapped flash */
                        ad_nvms_write(part, flash_memory_addr,
                                (uint8_t *)dev_1_out.memory_param.buff_addr[i],
                                buff_size);

                        flash_memory_addr += (NVMS_LOG_PART_START + MEMORY_BASE);
                        if (memcmp((uint8_t *)dev_1_out.memory_param.buff_addr[i],
                                (uint8_t *)flash_memory_addr, buff_size)) {
                                printf("wrong write at addr : %ld\n\r", flash_memory_addr);
                                success = false;
                        }

                        if (dev_1_out.memory_param.buff_addr[i]) {
                                OS_FREE((void *)dev_1_out.memory_param.buff_addr[i]);
                        }
                }
        }

        printf("\r\nWrite with : %s\r\n", (success ? "Success" : "Failure"));
}

static void mic_record_init(void)
{
        uint8_t  i;

# if DEMO_PCM_RECORD_PLAYBACK
        /* Select PCM input device*/
        dev_1_in.device_type = AUDIO_PCM;

        /* Initialize PCM input */
        dev_1_in.pcm_param.bit_depth = BIT_DEPTH;
        dev_1_in.pcm_param.channel_delay = 0;
        dev_1_in.pcm_param.clk_generation = HW_PCM_CLK_GEN_FRACTIONAL;
#if SYS_CLK_DIV1
        dev_1_in.pcm_param.clock = HW_PCM_CLK_DIV1;
#else
        dev_1_in.pcm_param.clock = HW_PCM_CLK_DIVN;
#endif
        dev_1_in.pcm_param.cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT;
        dev_1_in.pcm_param.format = I2S_MODE;
        dev_1_in.pcm_param.fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT;
        dev_1_in.pcm_param.fsc_length = 2;
        dev_1_in.pcm_param.inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED;
        dev_1_in.pcm_param.inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL;
        dev_1_in.pcm_param.mode = MODE_MASTER;
        dev_1_in.pcm_param.output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL;
        dev_1_in.pcm_param.sample_rate = SAMPLE_RATE_1;
        dev_1_in.pcm_param.total_channel_num = 2;
# elif DEMO_PDM_RECORD_PLAYBACK
        /* Select PDM input device */
        dev_1_in.device_type = AUDIO_PDM;

        /* Initialize PDM input */
        dev_1_in.pdm_param.mode = MODE_MASTER;
        dev_1_in.pdm_param.clk_frequency = PDM_FREQ;
        dev_1_in.pdm_param.in_delay = HW_PDM_DI_NO_DELAY;
        dev_1_in.pdm_param.swap_channel = 0;
# elif DEMO_SDADC_RECORD_PLAYBACK
        /* Select SDADC input device*/
        dev_1_in.device_type = AUDIO_SDADC;
        dev_1_in.sdadc_param.pga_gain = HW_SDADC_PGA_GAIN_18dB;
        dev_1_in.sdadc_param.pga_mode = HW_SDADC_PGA_MODE_DIFF;
# endif
        /* Select memory output device */
        dev_1_out.device_type = AUDIO_MEMORY;

        /* Initialize memory output */
        dev_1_out.memory_param.bit_depth = BIT_DEPTH;
        dev_1_out.memory_param.cb_buffer_len = DEMO_CHANNEL_DATA_BUF_CB_SIZE;
        dev_1_out.memory_param.sample_rate = SAMPLE_RATE_2;
        dev_1_out.memory_param.stereo = true,
        dev_1_out.memory_param.total_buffer_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
        dev_1_out.memory_param.app_ud = &context_audio_demo;
        dev_1_out.memory_param.cb = audio_buffer_ready_cb;
        dev_1_out.memory_param.circular = false;

        // Channels 0, 2, 4 or 6 must be used for SRC input
        dev_1_out.memory_param.dma_channel[0] = HW_DMA_CHANNEL_2;
# if DEMO_SDADC_RECORD_PLAYBACK
        dev_1_out.memory_param.dma_channel[1] = HW_DMA_CHANNEL_INVALID;
#else
        dev_1_out.memory_param.dma_channel[1] = HW_DMA_CHANNEL_0;
# endif

        uint32_t size = dev_1_out.memory_param.total_buffer_len;

        for (i = 0 ; i < (dev_1_out.memory_param.stereo ? 2 : 1); i++) {

                if (dev_1_out.memory_param.dma_channel[i] != HW_DMA_CHANNEL_INVALID) {

                        if (size > OS_GET_FREE_HEAP_SIZE()) {
                                printf("No enough heap size %ld, reduce num of channel or memory buffer", (long int)size);
                                dev_1_out.memory_param.buff_addr[i] = 0;
                                return;
                        }

                        dev_1_out.memory_param.buff_addr[i] = (uint32_t)OS_MALLOC(size);

                        if (!dev_1_out.memory_param.buff_addr[i]) {
                                ASSERT_ERROR(0);
                                return;
                        }
                }
        }
}

static void mic_playback_init(void)
{
        uint8_t i;

        /* Select memory input device */
        dev_1_in.device_type = AUDIO_MEMORY;

        /* Initialize parameters for memory input */
        dev_1_in.memory_param.app_ud = &context_audio_demo;
        dev_1_in.memory_param.cb = audio_buffer_ready_cb;
        dev_1_in.memory_param.bit_depth = BIT_DEPTH;
        dev_1_in.memory_param.cb_buffer_len = DEMO_CHANNEL_DATA_BUF_CB_SIZE;
        dev_1_in.memory_param.sample_rate = SAMPLE_RATE_2;
        dev_1_in.memory_param.stereo = true;
        dev_1_in.memory_param.total_buffer_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE;
        dev_1_in.memory_param.circular = false;

        // Channels 1, 3, 5 or 7 must be used for SRC output
        dev_1_in.memory_param.dma_channel[0] = HW_DMA_CHANNEL_3;
#  if DEMO_SDADC_RECORD_PLAYBACK
        dev_1_in.memory_param.dma_channel[1] = HW_DMA_CHANNEL_INVALID;
#  else
        dev_1_in.memory_param.dma_channel[1] = HW_DMA_CHANNEL_1;
#  endif

        for (i = 0 ; i < (dev_1_in.memory_param.stereo ? 2 : 1); i++) {

                dev_1_in.memory_param.buff_addr[i] = flash_memory_channel_addr[i]+
                                                     NVMS_LOG_PART_START +
                                                     MEMORY_BASE;
        }

        /* Select PCM output device */
        dev_1_out.device_type = AUDIO_PCM;

        /* Initialize parameters for PCM output */
        dev_1_out.pcm_param.bit_depth = BIT_DEPTH;
        dev_1_out.pcm_param.channel_delay = 0;
        dev_1_out.pcm_param.clk_generation = HW_PCM_CLK_GEN_FRACTIONAL;
#if SYS_CLK_DIV1
        dev_1_out.pcm_param.clock = HW_PCM_CLK_DIV1;
#else
        dev_1_out.pcm_param.clock = HW_PCM_CLK_DIVN;
#endif
        dev_1_out.pcm_param.cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT;
        dev_1_out.pcm_param.format = I2S_MODE;
        dev_1_out.pcm_param.fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT;
        dev_1_out.pcm_param.fsc_length = 2;
        dev_1_out.pcm_param.inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED;
        dev_1_out.pcm_param.inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL;
        dev_1_out.pcm_param.mode = MODE_MASTER;
        dev_1_out.pcm_param.output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL;
        dev_1_out.pcm_param.sample_rate = SAMPLE_RATE_1;
        dev_1_out.pcm_param.total_channel_num = 2;
}

OS_TASK_FUNCTION(audio_task, pvParameters)
{
        int8_t idx_1 = 0;
        for ( ;; ) {
                if (audio_task_get_pin_status(BTN_PIN)) {

                        OS_BASE_TYPE xResult;

                        mic_record_init();

                        /* Open audio path */
                        idx_1 = sys_audio_mgr_open_path(&dev_1_in, &dev_1_out, SRC_1);
                        ASSERT_ERROR(idx_1 >= 0);
# if DEMO_PCM_RECORD_PLAYBACK

                        /* Enable Audio Codec7218 */
                        DA7218_Enable();

# endif /* DEMO_PCM_RECORD_PLAYBACK */
                        /* Start PATH_1 */
                        audio_mgr_start(idx_1);

                        /*
                         * Wait on any of the event group bits, then clear them all
                         */
                        xResult = OS_TASK_NOTIFY_WAIT(OS_TASK_NOTIFY_NONE, OS_TASK_NOTIFY_ALL_BITS,
                                (uint32_t *)notified_value, OS_TASK_NOTIFY_FOREVER);
                        OS_ASSERT(xResult == OS_OK);

                        audio_mgr_stop(idx_1);

                        /* Copy ram pattern of memory audio to XiP flash */
                        copy_ram_pattern_to_qspi();

                        /* Close audio path */
                        sys_audio_mgr_close_path(idx_1);

# if DEMO_PCM_RECORD_PLAYBACK
                        /* Disable Audio Codec7218 */
                        DA7218_Disable();

# endif /* DEMO_PCM_RECORD_PLAYBACK */
                        mic_playback_init();

                        /* Open audio path */
                        idx_1 = sys_audio_mgr_open_path(&dev_1_in, &dev_1_out, SRC_1);
                        ASSERT_ERROR(idx_1 >= 0);

                        /* Enable Audio Codec7218 */
                        DA7218_Enable();

                        /* Start PATH_1 */
                        audio_mgr_start(idx_1);

                        /*
                         * Wait on any of the event group bits, then clear them all
                         */
                        xResult = OS_TASK_NOTIFY_WAIT(OS_TASK_NOTIFY_NONE, OS_TASK_NOTIFY_ALL_BITS,
                                (uint32_t *)notified_value, OS_TASK_NOTIFY_FOREVER);
                        OS_ASSERT(xResult == OS_OK);

                        audio_mgr_stop(idx_1);

                        /* Close audio path */
                        sys_audio_mgr_close_path(idx_1);

                        /* Disable Audio Codec7218 */
                        DA7218_Disable();

                        printf("For a new record, press the button\n");
                }
        }
}
#endif
#endif

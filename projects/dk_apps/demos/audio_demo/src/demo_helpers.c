/**
 ****************************************************************************************
 *
 * @file demo_helpers.c
 *
 * @brief Audio demo help functions set
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "sdk_defs.h"

void demo_set_sinusoidal_pattern(uint32_t *data, uint32_t data_len, const int32_t *pcm_data, uint16_t pcm_data_len,
                                 uint32_t sampling_rate, uint32_t signal_frequency, uint32_t bit_depth)
{
        uint16_t emul_index = 0;
        uint32_t i;
        uint8_t *data8 = (uint8_t*)data;
        uint16_t *data16 = (uint16_t*)data;

        ASSERT_ERROR(bit_depth == 8 || bit_depth == 16 || bit_depth == 32);
        ASSERT_ERROR(sampling_rate >= 2 * signal_frequency);
        ASSERT_ERROR(sampling_rate % signal_frequency == 0);

        const uint16_t signal_available_samples = pcm_data_len;
        const uint8_t nr_samples = sampling_rate / signal_frequency;

        ASSERT_ERROR(signal_available_samples % nr_samples == 0);

        const uint8_t step = signal_available_samples / nr_samples;

        ASSERT_ERROR(step > 0);
        ASSERT_ERROR(signal_available_samples % step == 0);

        for (i = 0; i < data_len; i++) {
                switch (bit_depth) {
                case 8:
                        data8[i] = (uint8_t)(pcm_data[emul_index] >> 24);
                        break;
                case 16:
                        data16[i] = (uint16_t)(pcm_data[emul_index] >> 16);
                        break;
                case 32:
                        data[i] = pcm_data[emul_index];
                        break;
                }

                emul_index += step;

                if (emul_index >= signal_available_samples) {
                        emul_index = 0;
                }
        }
}

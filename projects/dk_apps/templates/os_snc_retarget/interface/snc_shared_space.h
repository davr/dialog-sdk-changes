/**
 *****************************************************************************************
 *
 * @file snc_shared_space.h
 *
 * @brief SNC-SYSCPU shared space environment.
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef SNC_SHARED_SPACE_H_
#define SNC_SHARED_SPACE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief Application shared data type
 */
typedef struct {
        volatile uint32_t buffer[2];
} app_shared_data_t;

/**
 * \brief Application shared space handle ids
 *
 * Use snc_set_shared_space_addr() (in SNC) to publish the address of application shared data.
 * Use snc_get_shared_space_addr() to acquire the address of application shared data.
 */
typedef enum {
        APP_SHARED_SPACE_CTRL,          /**< Handle id for control data in shared space */
        APP_SHARED_SPACE_DATA           /**< Handle id for application data in shared space */
} APP_SHARED_SPACE_TYPE;

/*
 * SHARED SPACE CONTROL FUNCTIONS
 **************************************
 */

#if (SNC_PROCESSOR_BUILD)
/**
 * \brief Initialize application shared space control
 *
 * \note It can be called only in SNC context. SNC defines the shared space environment.
 */
void app_shared_space_ctrl_init(void);

/**
 * \brief Set application shared space as ready
 *
 * \note It can be called only in SNC context. SNC indicates that shared space environment is ready.
 */
void app_shared_space_ctrl_set_ready(void);
#endif /* SNC_PROCESSOR_BUILD */

/**
 * \brief Check whether application shared space environment is ready
 */
bool app_shared_space_ctrl_is_ready(void);

/**
 * \brief Acquire semaphore for this master
 */
void app_semph_take(void);

/**
 * \brief Release semaphore for this master
 */
void app_semph_give(void);

#endif /* SNC_SHARED_SPACE_H_ */

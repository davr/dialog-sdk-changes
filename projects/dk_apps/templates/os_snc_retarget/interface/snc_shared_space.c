/**
 *****************************************************************************************
 *
 * @file snc_shared_space.c
 *
 * @brief SNC-SYSCPU shared space environment definition.
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <string.h>
#include <stdbool.h>
#include "sdk_defs.h"
#include "snc.h"
#include "snc_shared_space.h"

typedef struct {
        volatile uint32_t snc_sf;
        volatile uint32_t syscpu_sf;
} app_semph_t;

typedef struct {
        volatile uint32_t shared_space_ready;   /* Shared space is ready */
        volatile app_semph_t semph;             /* Shared space access semaphore */
} app_shared_info_t;

/*
 * Application shared space info
 */
#if (MAIN_PROCESSOR_BUILD)
__RETAINED app_shared_info_t *app_shared_info_ptr;
#elif (SNC_PROCESSOR_BUILD)
__SNC_SHARED app_shared_info_t app_shared_info;
#define app_shared_info_ptr     ( &app_shared_info )
#endif /* SNC_PROCESSOR_BUILD */

/*
 * Macros indicating the semaphore signal flag used for each master
 */
#if (MAIN_PROCESSOR_BUILD)
#define THIS_MASTER_SF          syscpu_sf
#define OTHER_MASTER_SF         snc_sf
#elif (SNC_PROCESSOR_BUILD)
#define THIS_MASTER_SF          snc_sf
#define OTHER_MASTER_SF         syscpu_sf
#endif /* SNC_PROCESSOR_BUILD */

/*
 * SHARED SPACE ENVIRONMENT FUNCTIONS
 **************************************
 */

void app_shared_space_ctrl_init(void)
{
        /* Set application shared space control info. */
        memset(app_shared_info_ptr, 0, sizeof(app_shared_info_t));
        snc_set_shared_space_addr(app_shared_info_ptr, SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_CTRL));
}

void app_shared_space_ctrl_set_ready(void)
{
        app_shared_info_ptr->shared_space_ready = true;
}

bool app_shared_space_ctrl_is_ready(void)
{
#if (MAIN_PROCESSOR_BUILD)
        if (app_shared_info_ptr == NULL) {
                app_shared_info_t *ptr;

                ptr = snc_get_shared_space_addr(SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_CTRL));

                if (ptr != NULL) {
                        app_shared_info_ptr = ptr;
                }
        }

        if (app_shared_info_ptr) {
                return app_shared_info_ptr->shared_space_ready;
        } else {
                return false;
        }
#elif (SNC_PROCESSOR_BUILD)
        return app_shared_info_ptr->shared_space_ready;
#endif /* SNC_PROCESSOR_BUILD */
}

void app_semph_take(void)
{
#if (MAIN_PROCESSOR_BUILD)
        ASSERT_WARNING(app_shared_info_ptr);
#endif
        do {
                app_shared_info_ptr->semph.THIS_MASTER_SF = 1;

                if (app_shared_info_ptr->semph.OTHER_MASTER_SF == 0) {
                        break;
                }

                app_shared_info_ptr->semph.THIS_MASTER_SF = 0;
                while (app_shared_info_ptr->semph.OTHER_MASTER_SF);
        } while (1);
}

void app_semph_give(void)
{
#if (MAIN_PROCESSOR_BUILD)
        ASSERT_WARNING(app_shared_info_ptr);
#endif
        app_shared_info_ptr->semph.THIS_MASTER_SF = 0;
}

/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Template application for SNC
 *
 * Copyright (C) 2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "sys_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "snc.h"
#include "snc_shared_space.h"

/* Task priorities. */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which Template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)

/*
 * Perform any application specific hardware configuration. The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

/*
 * Task functions.
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters);

/*
 * Application SYSCPU-SNC shared data.
 */
__SNC_SHARED app_shared_data_t app_shared_data;

/*
 * System initialization task.
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
        OS_TASK_BEGIN();

        OS_TASK task_h;
        OS_BASE_TYPE status;

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Start Template task. */
        status = OS_TASK_CREATE( "Template",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvTemplateTask,                /* The function that implements the task. */
                        0,                              /* The parameter passed to the task. */
                        0,                              /* The number of bytes to allocate to the
                                                           stack of the task.
                                                           Don't care for Dialog CoRoutines. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Initialize application shared space control. */
        app_shared_space_ctrl_init();

        /* Set application shared space data. */
        memset(&app_shared_data, 0, sizeof(app_shared_data_t));
        snc_set_shared_space_addr(&app_shared_data, SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_DATA));

        /* Indicate that application shared space is ready. */
        app_shared_space_ctrl_set_ready();

        /* The work of the SysInit task is done. */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());

        OS_TASK_END();
}

/**
 * @brief Template main creates a SysInit task, which creates a Template task
 */
int main(void)
{
        OS_TASK xHandle;
        OS_BASE_TYPE status;

        /* Initialize SNC. */
        snc_init();

        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        0,                              /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE,     /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached. */
        for ( ;; );
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters)
{
        OS_TASK_BEGIN();

        static OS_TICK_TIME xNextWakeTime;
        static uint32_t test_counter = 0;

        /* Initialise xNextWakeTime - this only needs to be done once. */
        xNextWakeTime = OS_GET_TICK_COUNT();

        for ( ;; ) {
                /* Place this task in the blocked state until it is time to run again.
                   The block time is specified in ticks. While in the Blocked state this
                   task will not consume any CPU time. */
                xNextWakeTime += mainCOUNTER_FREQUENCY_MS;
                OS_DELAY_UNTIL(xNextWakeTime);
                test_counter++;

                if (test_counter % (1000 / OS_TICKS_2_MS(mainCOUNTER_FREQUENCY_MS)) == 0) {
                        /* Acquire exclusive access on shared data. */
                        app_semph_take();

                        app_shared_data.buffer[0] = OS_TICKS_2_MS(OS_GET_TICK_COUNT());
                        app_shared_data.buffer[1]++;

                        /* Release exclusive access on shared data. */
                        app_semph_give();

                        /* Send notification to SYSCPU. */
                        snc_set_snc2sys_int();
                }
        }

        OS_TASK_END();
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
        /* Init hardware */
        pm_system_init(periph_init);
}

/**
 * @brief Malloc fail hook
 */
OS_APP_MALLOC_FAILED(void)
{
        /* This function will only get called if a call to OS_MALLOC() fails. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
OS_APP_IDLE(void)
{
        /* This function will be called on each iteration of the idle task. */
}

/**
 * @brief Application stack overflow hook
 */
OS_APP_STACK_OVERFLOW(OS_TASK pxTask, char *pcTaskName)
{
        (void) pcTaskName;
        (void) pxTask;

        /* This function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
OS_APP_TICK(void)
{
}

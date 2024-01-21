/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief SYSCPU template application using SNC
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
#include "hw_sys.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "snc.h"
#include "snc_shared_space.h"

/* Task priorities. */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* Task notifications. */
#define TASK_SNC_NOTIF                          ( 1 << 0 )

/* Task handles. */
static OS_TASK task_h;

/*
 * Callback function for SNC2SYS IRQ.
 */
void test_snc_cb(void) {
        OS_TASK_NOTIFY_FROM_ISR(task_h, TASK_SNC_NOTIF, OS_NOTIFY_SET_BITS);
}

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

/*
 * Task functions.
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters);

/*
 * Address of application SYSCPU-SNC shared data.
 */
__RETAINED app_shared_data_t *app_shared_data_ptr;

/*
 * System initialization task.
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
        OS_TASK_BEGIN();

        OS_BASE_TYPE status;

        cm_sys_clk_init(sysclk_RCHS_32);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);

        /* Start Template task. */
        status = OS_TASK_CREATE( "Template",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvTemplateTask,                /* The function that implements the task. */
                        0,                              /* The parameter passed to the task. */
                        1024,                           /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Register callback for SNC2SYS IRQ. */
        snc_register_snc2sys_int(test_snc_cb);

        /* Initialize and start SNC. */
        snc_freeze();
        snc_init();
        snc_start();

        /* Wait for SNC to finish start-up process. */
        while (!snc_is_ready());

        /* Wait for SNC to initialize application shared space. */
        while (!app_shared_space_ctrl_is_ready());

        /* Get the address of application shared data. */
        app_shared_data_ptr = snc_get_shared_space_addr(SNC_SHARED_SPACE_APP(APP_SHARED_SPACE_DATA));

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
 * @brief Template task prints a message every time a notification is received from SNC
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters)
{
        OS_TASK_BEGIN();

        uint32_t notif;

        for ( ;; ) {
                OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

                /* Check whether a notification has been received from SNC. */
                if (notif & TASK_SNC_NOTIF) {
                        /* Acquire exclusive access on shared data. */
                        app_semph_take();

                        printf("@%lu\tSNC->SYSCPU (%lu)\r\n", app_shared_data_ptr->buffer[0],
                                app_shared_data_ptr->buffer[1]);

                        /* Release exclusive access on shared data. */
                        app_semph_give();
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

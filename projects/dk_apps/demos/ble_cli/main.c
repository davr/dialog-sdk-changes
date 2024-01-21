/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Test suite template for the Dialog Black Orca platform
 *
 * Copyright (C) 2016-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include <stdbool.h>
#include "sdk_defs.h"
#include "osal.h"
#include "ble_mgr.h"
#include "hw_gpio.h"
#include "hw_pdc.h"
#include "hw_wkup.h"
#include "console.h"
#include "cli.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"

/* Task priorities */
#define mainBLE_CLI_TASK_PRIORITY       ( OS_TASK_PRIORITY_NORMAL )

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
OS_TASK_FUNCTION(ble_cli_task, params);

static OS_TASK handle = NULL;

/* Wkup handler used by GPIO-WKUP commands */
extern void gpio_wkup_cmd_handler(void);

/**
 * @brief System Initialization and creation of the BLE task
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        /*
         * Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only from a
         * task since they will suspend the task until the XTAL has settled and, maybe, the PLL
         * is locked.
         */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Initialize system watchdog */
        sys_watchdog_init();
#if dg_configUSE_WDOG
        // Register the Idle task first.
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Prepare the hardware to run this demo */
        prvSetupHardware();

        /* Set the desired sleep mode */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);


#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* Initialize cli framework */
        cli_init();

        /* Initialize BLE Manager */
        ble_mgr_init();

        /* Start the ble_cli application task */
        OS_TASK_CREATE("BLE_CLI",                       /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                       ble_cli_task,                    /* The function that implements the task. */
                       NULL,                            /* The parameter passed to the task. */
                       1024 * OS_STACK_WORD_SIZE,       /* The number of bytes to allocate to the
                                                           stack of the task. */
                       mainBLE_CLI_TASK_PRIORITY,       /* The priority assigned to the task. */
                       handle);                         /* The task handle. */
        OS_ASSERT(handle);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}
/*-----------------------------------------------------------*/

/**
 * @brief Basic initialization and creation of the system initialization task.
 */
int main( void )
{
        OS_BASE_TYPE status;

        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",                /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                                system_init,              /* The System Initialization task. */
                                ( void * ) 0,             /* The parameter passed to the task. */
                                1200,                     /* The number of bytes to allocate to the
                                                             stack of the task. */
                                OS_TASK_PRIORITY_HIGHEST, /* The priority assigned to the task. */
                                handle);                  /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks     to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );
}

static void periph_init(void)
{
}

static void wkup_handler()
{

        if (hw_wkup_get_gpio_status(SER1_CTS_PORT) & (1 << SER1_CTS_PIN)) {
                console_wkup_handler();
                hw_wkup_clear_gpio_status(SER1_CTS_PORT, (1 << SER1_CTS_PIN));
        } else {
                /* This handler clears all pins Wkup status - it must be called as the last */
                gpio_wkup_cmd_handler();
        }
}

static void prvSetupHardware( void )
{

        /* Init hardware */
        pm_system_init(periph_init);

        /* Initialize hw_wkup to allow wake up on CTS */
        hw_wkup_init(NULL);
        /* Enable wake up from non debounced GPIO */
        hw_wkup_set_trigger(SER1_CTS_PORT, SER1_CTS_PIN, HW_WKUP_TRIG_LEVEL_LO);
        hw_wkup_register_gpio_p0_interrupt(wkup_handler, 1);
        /* Setup PDC entry to wake up from UART CTS pin */
        {
                uint32 idx = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                        HW_PDC_TRIG_SELECT_P0_GPIO, SER1_CTS_PIN,
                                                        HW_PDC_MASTER_CM33,
                                                        HW_PDC_LUT_ENTRY_EN_XTAL));
                hw_pdc_set_pending(idx);
                hw_pdc_acknowledge(idx);
        }
}

/**
 * @brief Malloc fail hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * It is a hook function that will execute when a call to OS_MALLOC() returns error.
 * OS_MALLOC() is called internally by the kernel whenever a task, queue,
 * timer or semaphore is created. It can be also called by the application.
 * The size of the available heap is defined by OS_TOTAL_HEAP_SIZE in osal.h.
 * The OS_GET_FREE_HEAP_SIZE() API function can be used to query the size of
 * free heap space that remains, although it does not provide information on
 * whether the remaining heap is fragmented.
 */
OS_APP_MALLOC_FAILED( void )
{
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * It will be called on each iteration of the idle task.
 * It is essential that code added to this hook function never attempts
 * to block in any way (for example, call OS_QUEUE_GET() with a block time
 * specified, or call OS_TASK_DELAY()). If the application makes use of the
 * OS_TASK_DELETE() API function (as this demo application does) then it is also
 * important that OS_APP_IDLE() is permitted to return to its calling
 * function, because it is the responsibility of the idle task to clean up
 * memory allocated by the kernel to any task that has since been deleted.
 */
OS_APP_IDLE( void )
{
#if dg_configUSE_WDOG
        sys_watchdog_notify(idle_task_wdog_id);
#endif
}

/**
 * @brief Application stack overflow hook
 *
 * Run-time stack overflow checking is performed only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * This hook function is called if a stack overflow is detected.
 */
OS_APP_STACK_OVERFLOW( OS_TASK pxTask, char *pcTaskName )
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * This hook function is executed each time a tick interrupt occurs.
 */
OS_APP_TICK( void )
{
}

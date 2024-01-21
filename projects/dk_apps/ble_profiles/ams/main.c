/**
 *
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief AMS application
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
/* Standard includes. */
#include <string.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "ad_ble.h"
#include "ad_nvms.h"
#include "ble_mgr.h"
#include "cli.h"
#include "console.h"
#include "hw_gpio.h"
#include "hw_wkup.h"
#include "hw_pdc.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#include "ams_config.h"

/* Both K1 and CTS use P1.6 on DA1468x boards - we need to define CFG_USER_BUTTON_* wrappers. */
#define CFG_USER_BUTTON_MODE    (KEY1_MODE)
#define CFG_USER_BUTTON_FUNC    (KEY1_FUNC)

/* Task priorities */
#define mainBLE_AMS_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The configCHECK_FOR_STACK_OVERFLOW setting in FreeRTOSConifg can be used to
check task stacks for overflows.  It does not however check the stack used by
interrupts.  This demo has a simple addition that will also check the stack used
by interrupts if mainCHECK_INTERRUPT_STACK is set to 1.  Note that this check is
only performed from the tick hook function (which runs in an interrupt context).
It is a good debugging aid - but won't catch interrupt stack problems until the
tick interrupt next executes. */
//#define mainCHECK_INTERRUPT_STACK                     1
#if mainCHECK_INTERRUPT_STACK == 1
const unsigned char ucExpectedInterruptStackValues[] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
#endif

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
OS_TASK_FUNCTION(ams_task, params);

static OS_TASK handle;

void ams_wkup_handler(void);

static void wkup_handler()
{

        if (hw_gpio_get_pin_status(CFG_USER_BUTTON_PORT, CFG_USER_BUTTON_PIN) == false)  {
                ams_wkup_handler();
        }

        hw_wkup_reset_key_interrupt();
}

/**
 * @brief System Initialization and creation of the BLE task
 */
static OS_TASK_FUNCTION(system_init, pvParameters )
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        /* Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only from a
         * task since they will suspend the task until the XTAL16M has settled and, maybe, the PLL
         * is locked.
         */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /*
         * Initialize platform watchdog
         */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        // Register the Idle task first.
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif


        /* Prepare the hardware to run this demo. */
        prvSetupHardware();


#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* Set the desired sleep mode. */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Initialize BLE Manager */
        ble_mgr_init();

        /* Initialize cli framework */
        cli_init();

        /* Start the AMS Profile application task. */
        OS_TASK_CREATE("AMS Profile",                      /* The text name assigned to the task, for
                                                              debug only; not used by the kernel. */
                       ams_task,                           /* The function that implements the task. */
                       NULL,                               /* The parameter passed to the task. */
                       1024,                               /* The number of bytes to allocate to the
                                                              stack of the task. */
                       mainBLE_AMS_TASK_PRIORITY,          /* The priority assigned to the task. */
                       handle);                            /* The task handle. */
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

        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",                /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                                system_init,              /* The System Initialization task. */
                                ( void * ) 0,             /* The parameter passed to the task. */
                                1024,                     /* The number of bytes to allocate to the
                                                             stack of the task. */
                                OS_TASK_PRIORITY_HIGHEST, /* The priority assigned to the task. */
                                handle );                 /* The task handle */
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

/**
 * \brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
        HW_GPIO_SET_PIN_FUNCTION(CFG_USER_BUTTON);
        HW_GPIO_PAD_LATCH_ENABLE(CFG_USER_BUTTON);
        HW_GPIO_PAD_LATCH_DISABLE(CFG_USER_BUTTON);
}

static void init_wakeup(void)
{
        hw_wkup_init(NULL);
        hw_wkup_set_trigger(CFG_USER_BUTTON_PORT, CFG_USER_BUTTON_PIN, HW_WKUP_TRIG_LEVEL_LO_DEB);
        hw_wkup_set_key_debounce_time(10);
        hw_wkup_register_key_interrupt(wkup_handler, 1);
        hw_wkup_enable_key_irq();
}

static void wkup_gpio_p0_interrupt_cb()
{
        /* Clear interrupt if the source is the UART CTS pin */
        if (hw_wkup_get_gpio_status(SER1_CTS_PORT) & (1 << SER1_CTS_PIN)) {
                hw_wkup_clear_gpio_status(SER1_CTS_PORT, (1 << SER1_CTS_PIN));
                console_wkup_handler();
        }
}

static void init_wkup_gpio_p0(void)
{
        /* Enable wake up from non debounced GPIO */
        hw_wkup_set_trigger(SER1_CTS_PORT, SER1_CTS_PIN, HW_WKUP_TRIG_LEVEL_LO);
        /* Register callback for (non-debounced) GPIO port 0 interrupt */
        hw_wkup_register_gpio_p0_interrupt(wkup_gpio_p0_interrupt_cb, 1);

        /* Setup PDC entry to wake up from UART CTS pin */
        {
                uint32 idx = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
                                                        HW_PDC_TRIG_SELECT_P0_GPIO, HW_GPIO_PIN_7,
                                                        HW_PDC_MASTER_CM33,
                                                        HW_PDC_LUT_ENTRY_EN_XTAL));
                hw_pdc_set_pending(idx);
                hw_pdc_acknowledge(idx);
        }
}

static void prvSetupHardware( void )
{
#if mainCHECK_INTERRUPT_STACK == 1
        extern unsigned long _vStackTop[], _pvHeapStart[];
        unsigned long ulInterruptStackSize;
#endif

        /* Init hardware */
        pm_system_init(periph_init);

        init_wakeup();
        /* Initialize wakeup from non-debounced IRQ used in demo */
        init_wkup_gpio_p0();

#if mainCHECK_INTERRUPT_STACK == 1
        /* The size of the stack used by main and interrupts is not defined in
           the linker, but just uses whatever RAM is left.  Calculate the amount of
           RAM available for the main/interrupt/system stack, and check it against
           a reasonable number.  If this assert is hit then it is likely you don't
           have enough stack to start the kernel, or to allow interrupts to nest.
           Note - this is separate to the stacks that are used by tasks.  The stacks
           that are used by tasks are automatically checked if
           configCHECK_FOR_STACK_OVERFLOW is not 0 in FreeRTOSConfig.h - but the stack
           used by interrupts is not.  Reducing the conifgTOTAL_HEAP_SIZE setting will
           increase the stack available to main() and interrupts. */
        ulInterruptStackSize = ( ( unsigned long ) _vStackTop ) - ( ( unsigned long ) _pvHeapStart );
        OS_ASSERT( ulInterruptStackSize > 350UL );

        /* Fill the stack used by main() and interrupts to a known value, so its
           use can be manually checked. */
        memcpy( ( void * ) _pvHeapStart, ucExpectedInterruptStackValues, sizeof( ucExpectedInterruptStackValues ) );
#endif
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
        taskDISABLE_INTERRUPTS();
        for ( ;; );
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

        taskDISABLE_INTERRUPTS();
        for ( ;; );
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
#if mainCHECK_INTERRUPT_STACK == 1
        extern unsigned long _pvHeapStart[];

        OS_ASSERT( memcmp( ( void * ) _pvHeapStart, ucExpectedInterruptStackValues, sizeof( ucExpectedInterruptStackValues ) ) == 0U );
#endif /* mainCHECK_INTERRUPT_STACK */
}

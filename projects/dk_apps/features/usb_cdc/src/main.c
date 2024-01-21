/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief USB CDC
 *
 * Copyright (C) 2016-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include <stdbool.h>

#include "osal.h"
#include "hw_gpio.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#include "sys_usb.h"

#if dg_configUSB_DMA_SUPPORT
#include "hw_dma.h"
#endif

#if dg_configUSE_SYS_CHARGER
#include "sys_charger.h"
#include "custom_charging_profile.h"
#endif

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

static void prvSetupHardware(void);

static OS_TASK_FUNCTION(system_init, pvParameters)
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

        /* Initialize platform watchdog */
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

#if dg_configUSB_DMA_SUPPORT
        sys_usb_conf_t cfg;

        /* Set the desired DMA channels and parameters
         * to be used with USB DATA interface.
         * The USB subsystem will try to use these resources on
         * every plug-in.
         * However, if the DMA resources are not available
         * when the USB plug-in event occurs, then it will
         * automatically fallback to USB operation in interrupt
         * mode to ensure that the USB will be functional.
         */

        cfg.lld.use_dma = true;                    /* Enable the use of DMA with USB */
        cfg.lld.rx_dma_channel = HW_DMA_CHANNEL_0; /* Select the DMA channel for USB RX.
                                                    * Avoid channel 7 which is the only
                                                    * way to write the keys to Crypto Engine
                                                    */
        cfg.lld.tx_dma_channel = HW_DMA_CHANNEL_1; /* Select the DMA channel for USB RX.
                                                    * Avoid channel 7 which is the only
                                                    * way to write the keys to Crypto Engine
                                                    */
        cfg.lld.dma_prio.rx_prio = HW_DMA_PRIO_5;  /* Set the priority of the RX DMA */
        cfg.lld.dma_prio.tx_prio = HW_DMA_PRIO_6;  /* Set the priority of the TX DMA */

        sys_usb_cfg(&cfg);                         /* Apply the configuration.
                                                    * Call this before the sys_usb_init()
                                                    */
#endif /* dg_configUSB_DMA_SUPPORT */

        sys_usb_init();

#if dg_configUSE_SYS_CHARGER
        sys_charger_init(&sys_charger_conf);
#endif

        /* the work of the SysInit task is done */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}


static void periph_init(void)
{
        /* USB data pin configuration */
        hw_gpio_set_pin_function(HW_GPIO_PORT_2, HW_GPIO_PIN_10, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
        hw_gpio_set_pin_function(HW_GPIO_PORT_2, HW_GPIO_PIN_11, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
}


/**
 * @brief BLE FW demo main creates a BLE task
 */
int main(void)
{
        OS_TASK handle;
        OS_BASE_TYPE status;

        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",        /* The text name assigned to the task, for
                                                   debug only; not used by the kernel. */
                        system_init,              /* The System Initialization task. */
                        ( void * ) 0,             /* The parameter passed to the task. */
                        1024,                     /* The number of bytes to allocate to the
                                                   stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST, /* The priority assigned to the task. */
                        handle);                  /* The task handle */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
         line will never be reached.  If the following line does execute, then
         there was insufficient FreeRTOS heap memory available for the idle and/or
         timer tasks to be created.  See the memory management section on the
         FreeRTOS web site for more details. */
        for (;;)
                ;
}

static void prvSetupHardware(void)
{

        /* Init hardware */
        pm_system_init(periph_init);
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
OS_APP_MALLOC_FAILED(void)
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
OS_APP_IDLE(void)
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
OS_APP_STACK_OVERFLOW( OS_TASK pxTask, char *pcTaskName)
{
        (void)pcTaskName;
        (void)pxTask;

        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 *
 * This function will be called only if it is enabled in the configuration of the OS
 * or in the OS abstraction layer header osal.h, by a relevant macro definition.
 * This hook function is executed each time a tick interrupt occurs.
 */
OS_APP_TICK(void)
{
}


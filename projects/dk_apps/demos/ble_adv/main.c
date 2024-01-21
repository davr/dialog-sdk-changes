/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief BLE ADV demo application
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdbool.h>
#include "osal.h"
#include "resmgmt.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_mgr.h"
#include "hw_gpio.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"

/*
 * BLE adv demo advertising data
 */
static const uint8_t adv_data[] = {
        0x10, GAP_DATA_TYPE_LOCAL_NAME,
        'D', 'i', 'a', 'l', 'o', 'g', ' ', 'A', 'D', 'V', ' ', 'D', 'e', 'm', 'o'
};

/* Task priorities */
#define mainBLE_ADV_DEMO_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

#if (dg_configTRACK_OS_HEAP == 1)
/*
 * ConstantsVariables used for Tasks Stack and OS Heap tracking
 * Declared global to avoid IDLE stack Overflows
 */
#define mainMAX_NB_OF_TASKS           10
#define mainMIN_STACK_GUARD_SIZE      8 /* words */
#define mainTOTAL_HEAP_SIZE_GUARD     64 /*bytes */

OS_TASK_STATUS pxTaskStatusArray[mainMAX_NB_OF_TASKS];
#endif /* (dg_configTRACK_OS_HEAP == 1) */

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

static OS_TASK handle = NULL;

/*
 * Perform any application specific hardware configuration. The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/*
 * Task functions.
 */
static OS_TASK_FUNCTION(ble_adv_demo_task, pvParameters);

/**
 * @brief System Initialization and creation of the BLE task
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        /* Use appropriate XTAL for each device */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Initialize platform watchdog */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        /* Register the Idle task first */
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif


        /* Prepare the hardware to run this demo */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* Set the desired sleep mode */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Initialize BLE Manager */
        ble_mgr_init();

        /* Start the BLE adv demo application task */
        OS_TASK_CREATE("BLE ADV Demo",                  /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                       ble_adv_demo_task,               /* The function that implements the task. */
                       NULL,                            /* The parameter passed to the task. */
                       768,                             /* The number of bytes to allocate to the
                                                           stack of the task. */
                       mainBLE_ADV_DEMO_TASK_PRIORITY,  /* The priority assigned to the task. */
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


        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",                /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                                system_init,              /* The System Initialization task. */
                                ( void * ) 0,             /* The parameter passed to the task. */
                                1200,                     /* The number of bytes to allocate to the
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


}


static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        /**
         * Manage behavior upon connection
         */
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        /**
         * Manage behavior upon disconnection
         */

        // Restart advertising
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

static void handle_evt_gap_pair_req(ble_evt_gap_pair_req_t *evt)
{
        ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
}


static OS_TASK_FUNCTION(ble_adv_demo_task, pvParameters)
{
        int8_t wdog_id;

        /* Just remove compiler warnings about the unused parameter */
        ( void ) pvParameters;

        /* Register ble_adv_demo_task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /* Start BLE device as a peripheral */
        ble_peripheral_start();

        /* Set device name */
        ble_gap_device_name_set("Dialog ADV Demo", ATT_PERM_READ);

        /* Set advertising data */
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);

        /* Start advertising */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        for (;;) {
                ble_evt_hdr_t *hdr;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on ble_get_event() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait for a BLE event - this task will block
                 * indefinitely until something is received.
                 */
                hdr = ble_get_event(true);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                if (!hdr) {
                        continue;
                }

                switch (hdr->evt_code) {
                case BLE_EVT_GAP_CONNECTED:
                        handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                        break;
                case BLE_EVT_GAP_DISCONNECTED:
                        handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                        break;
                case BLE_EVT_GAP_PAIR_REQ:
                        handle_evt_gap_pair_req((ble_evt_gap_pair_req_t *) hdr);
                        break;
                default:
                        ble_handle_event_default(hdr);
                        break;
                }

                /* Free event buffer (it's not needed anymore) */
                OS_FREE(hdr);
        }
}

static void prvSetupHardware( void )
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
#if (dg_configTRACK_OS_HEAP == 1)
        OS_BASE_TYPE i = 0;
        OS_BASE_TYPE uxMinimumEverFreeHeapSize;

        // Generate raw status information about each task.
        OS_UBASE_TYPE uxNbOfTaskEntries = OS_GET_TASKS_STATUS(pxTaskStatusArray,
                                                        mainMAX_NB_OF_TASKS, NULL);

        for (i = 0; i < uxNbOfTaskEntries; i++) {
                /* Check Free Stack*/
                OS_BASE_TYPE uxStackHighWaterMark;

                uxStackHighWaterMark = OS_GET_TASK_STACK_WATERMARK(pxTaskStatusArray[i].xHandle);
                OS_ASSERT(uxStackHighWaterMark >= mainMIN_STACK_GUARD_SIZE);
        }

        /* Check Minimum Ever Free Heap against defined guard. */
        uxMinimumEverFreeHeapSize = OS_GET_FREE_HEAP_SIZE();
        OS_ASSERT(uxMinimumEverFreeHeapSize >= mainTOTAL_HEAP_SIZE_GUARD);
#endif /* (dg_configTRACK_OS_HEAP == 1) */

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

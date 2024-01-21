/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief FreeRTOS template application with retarget
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
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
#include "ad_pmu.h"
#include "hw_gpio.h"
#include "hw_led.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"

#include "gdi.h"

#define PIN_RGB_R HW_GPIO_PIN_30
#define PIN_RGB_G HW_GPIO_PIN_29
#define PIN_RGB_B HW_GPIO_PIN_28

#define PORT_RGB HW_GPIO_PORT_1

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

static OS_TASK xHandle;

/*
 * Task functions.
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters);

static OS_TASK_FUNCTION(system_init, pvParameters)
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif
        OS_TASK task_h = NULL;

        sys_clk_t sys_clk_prio[5] = {
                sysclk_PLL160,
                sysclk_XTAL32M,
                sysclk_RCHS_96,
                sysclk_RCHS_32,
                sysclk_RCHS_64,
        };

        cm_sys_clk_set_priority(sys_clk_prio);
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Initialize platform watchdog */
        sys_watchdog_init();



        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);

#if defined CONFIG_RETARGET
      //  retarget_init();
#endif
        cm_sys_clk_request(AD_LCDC_DEFAULT_CLK);



        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "Template",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvTemplateTask,                /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(task_h);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

/**
 * @brief Template main creates a SysInit task, which creates a Template task
 */
int main( void )
{
        OS_BASE_TYPE status;


        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        OS_MINIMAL_TASK_STACK_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);



        /* Start the tasks and timer running. */
        OS_TASK_SCHEDULER_RUN();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );

}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static OS_TASK_FUNCTION(prvTemplateTask, pvParameters)
{



       /* Make layer visible */
       gdi_set_layer_enable(HW_LCDC_LAYER_0, true);

        for (int i = 0 ;; i++ )
        {

                OS_DELAY_MS(10);

                hw_led_pwm_set_duty_cycle_pct_off(HW_LED_ID_LED_1, 8000,0);

                uint16_t* fb = (uint16_t*)gdi_get_frame_buffer_addr(HW_LCDC_LAYER_0);

                for(int ix = 0; ix < 240; ix++)
                        for(int iy = 0; iy < 240; iy++)
                              fb[iy*240+ix] = ((ix^i)<<8) + (iy^i);



       }
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
        /* Initializes the GDI instance, allocate memory and set default background color */
        gdi_init();
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{
        const hw_led_config led_conf = { {5000,1000,1000}, {0,0,0}, 1000};

        const gpio_config gpio_conf[] = {
                HW_GPIO_PINCONFIG(PORT_RGB, PIN_RGB_R, OUTPUT_PUSH_PULL, GPIO, true),
                HW_GPIO_PINCONFIG(PORT_RGB, PIN_RGB_G, OUTPUT_PUSH_PULL, GPIO, true),
                HW_GPIO_PINCONFIG(PORT_RGB, PIN_RGB_B, OUTPUT_PUSH_PULL, GPIO, true),
                HW_GPIO_PINCONFIG_END
        };

        /* Init hardware */
        pm_system_init(periph_init);


        hw_gpio_configure(gpio_conf);

        hw_gpio_configure_pin_power(PORT_RGB, PIN_RGB_R, HW_GPIO_POWER_V33);
        hw_gpio_configure_pin_power(PORT_RGB, PIN_RGB_G, HW_GPIO_POWER_V33);
        hw_gpio_configure_pin_power(PORT_RGB, PIN_RGB_B, HW_GPIO_POWER_V33);


        ad_pmu_rail_config_t vled_rail_config;

        vled_rail_config.enabled_onwakeup = true;
        vled_rail_config.enabled_onsleep = false;
        vled_rail_config.rail_vled.current_onsleep = HW_PMU_VLED_MAX_LOAD_150;
        vled_rail_config.rail_vled.current_onwakeup = HW_PMU_VLED_MAX_LOAD_150;
        vled_rail_config.rail_vled.voltage_common = HW_PMU_VLED_VOLTAGE_4V5;

        ad_pmu_configure_rail(PMU_RAIL_VLED, &vled_rail_config);

        hw_led_on(HW_LED_ALL_LED_MASK);

        hw_led_init(&led_conf);

        hw_led_pwm_on(HW_LED_ALL_LED_MASK);

        hw_led_pwm_set_load_sel(HW_LED_ID_LED_1, 7);
        hw_led_pwm_set_load_sel(HW_LED_ID_LED_2, 2);
        hw_led_pwm_set_load_sel(HW_LED_ID_LED_3, 2);








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

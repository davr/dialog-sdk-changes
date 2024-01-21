/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Test application for verifying Audio Unit
 *
 * Copyright (C) 2020-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_pd.h"
#include "sdk_defs.h"
#include "hw_watchdog.h"
#include "DA7218_driver.h"
#include "ad_pmu.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "audio_task.h"
#include "periph_setup.h"

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

static OS_TASK xHandle;
extern context_audio_demo_t context_audio_demo;
/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/**
 * \brief System Initialization
 *
 */
void periph_init(void)
{
        hw_gpio_pad_latch_enable_all();         // enable pads

        /* To make use of Codec7218 on mb */
        hw_gpio_configure_pin(DA_PWRON_PAD, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_set_pin_function(MCLK_PAD, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO);
        hw_gpio_clk_output_enable(HW_GPIO_CLK_DIVN_OUT);
#if DEMO_PDM_MIC || DEMO_PDM_RECORD_PLAYBACK || DEMO_PDM_MIC_PARALLEL_SRCS
        hw_gpio_configure_pin(PDM_CLK_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PDM_CLK, false);
        hw_gpio_configure_pin(PDM_DATA_PIN, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_PDM_DATA, false);
#endif

#if DEMO_SDADC_MIC || DEMO_SDADC_RECORD_PLAYBACK
        hw_gpio_configure_pin_power(MIC_PWR, HW_GPIO_POWER_V33);
        hw_gpio_configure_pin(MIC_PWR, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true);
        hw_gpio_configure_pin(PGA_INP, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false);
        hw_gpio_configure_pin(PGA_INM, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false);
#endif /* DEVICE_FAMILY */

#if DEMO_PDM_MIC || DEMO_PCM_MIC || DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_RECORD_PLAYBACK || \
        DEMO_PDM_MIC_PARALLEL_SRCS || DEMO_SDADC_MIC || DEMO_SDADC_RECORD_PLAYBACK
        hw_gpio_configure_pin(PCM_CLK_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PCM_CLK, false);
        hw_gpio_configure_pin(PCM_FSC_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PCM_FSC, false);

        hw_gpio_configure_pin(PCM_DO_PIN, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PCM_DO, false);
#endif

#if DEMO_PCM_MIC || DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_MIC_PARALLEL_SRCS
        hw_gpio_configure_pin(PCM_DI_PIN, HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_PCM_DI, false);
#endif

#if DEMO_PDM_RECORD_PLAYBACK || DEMO_PCM_RECORD_PLAYBACK || DEMO_PDM_MIC_PARALLEL_SRCS || \
        DEMO_MEM_TO_MEM || DEMO_SDADC_RECORD_PLAYBACK
        hw_gpio_configure_pin(BTN_PIN, KEY1_MODE, KEY1_FUNC, false);
#endif
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{
        /* Init hardware */
        pm_system_init(periph_init);

}

/**
 * \brief System Initialization
 *
 */
static OS_TASK_FUNCTION(system_init, pvParameters)
{

#ifdef CONFIG_RETARGET
        extern void retarget_init(void);
#endif

#if SYS_CLK_DIV1
#if SYS_CLK_RCHS64
        cm_sys_clk_init(sysclk_RCHS_64);
#elif SYS_CLK_RCHS96
        cm_sys_clk_init(sysclk_RCHS_96);
#elif SYS_CLK_PLL160
        cm_sys_clk_init(sysclk_PLL160);
#endif
#else
        cm_sys_clk_init(sysclk_XTAL32M);
#endif

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#ifdef CONFIG_RETARGET
        retarget_init();
#endif

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
# if SYS_CLK_RCHS64 || SYS_CLK_RCHS96
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_normal);
# else
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);
# endif

        /* Initialize and Start Codec 7218 */
#if !(DEMO_MEM_TO_MEM)
        // Prerequisites to use Codec7218 attached on mb for DA1470x
        // Enable snc power domain to use I2C for Codec7218
        if (!hw_pd_check_snc_status()) {
                hw_pd_power_up_snc();
        }
        /* Power on manually the 1V8 rail */
        const ad_pmu_rail_config_t v18_rail_cfg = {
                .enabled_onwakeup = true,
                .enabled_onsleep = false,
                .rail_1v8.voltage_common = HW_PMU_1V8_VOLTAGE_1V8,
                .rail_1v8.current_onwakeup = HW_PMU_1V8_MAX_LOAD_100,
                .rail_1v8.current_onsleep = HW_PMU_1V8_MAX_LOAD_100,
        };

        ad_pmu_configure_rail(PMU_RAIL_1V8, &v18_rail_cfg);
        /* Initialize Codec7218 */
        DA7218_Init();
#endif
        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "Audio task",           /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                audio_task,                     /* The function that implements the task. */
                NULL,                           /* The parameter passed to the task. */
                3 * configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                /* The number of bytes to allocate to the
                                                   stack of the task. */
                mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                context_audio_demo.audio_task );/* The task handle */
        OS_ASSERT(context_audio_demo.audio_task);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

int main(void)
{
        OS_BASE_TYPE status;

        /* Start the two tasks as described in the comments at the top of this file. */
        status = OS_TASK_CREATE("SysInit",      /* The text name assigned to the task, for
                                                 debug only; not used by the kernel. */
                system_init,                    /* The System demoInitialization task. */
                ( void * ) 0,                   /* The parameter passed to the task. */
                2 * configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                /* The number of bytes to allocate to
                                                   the stack of the task. */
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

        return 1;
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

void printf_settings(sys_audio_device_t *dev, device_direction_t dir)
{
        if (dir == INPUT_DEVICE) {
                printf("\n\r>>> Input device: ");
        } else {
                printf("\n\r>>> Output device: ");
        }

        switch (dev->device_type) {
        case AUDIO_PDM:
                printf("PDM <<<\n\r");
                printf("1. Mode:                     %s\n\r", ((dev->pdm_param.mode == MODE_SLAVE) ? "Slave" : "Master"));
                printf("2. Clock frequency:          %ld Hz\n\r", dev->pdm_param.clk_frequency);

                if (dir == OUTPUT_DEVICE) {
                        printf("3. Channels recorded:        %s\n\r", PRINTF_RECORDED_CHANNELS(dev->pdm_param.channel));
                }

                if (dir == INPUT_DEVICE) {
                        printf("4. In delay:                 %d\n\r", dev->pdm_param.in_delay);
                } else {
                        printf("4. Out delay:                %d\n\r", dev->pdm_param.out_delay);
                }
                break;
        case AUDIO_PCM:
                printf("PCM <<<\n\r");
                printf("1.  Mode:                    %s\n\r", ((dev->pcm_param.mode == MODE_SLAVE) ? "Slave" : "Master"));
                printf("2.  Format:                  %s\n\r", ((dev->pcm_param.format == PCM_MODE) ? "PCM" :
                        (dev->pcm_param.format == I2S_MODE) ? "I2S" :
                                (dev->pcm_param.format == IOM2_MODE)? "IOM2": "TDM"));
                printf("3.  Sample rate:             %ld Hz\n\r", dev->pcm_param.sample_rate);
                printf("4.  Total channel number:    %d\n\r", dev->pcm_param.total_channel_num);
                printf("5.  Channel delay:           %d\n\r", dev->pcm_param.channel_delay);
                printf("6.  Bits depth:              %d\n\r", dev->pcm_param.bit_depth);
                printf("7.  Clk generation:          %s\n\r", (dev->pcm_param.clk_generation == HW_PCM_CLK_GEN_FRACTIONAL)?
                        "HW_PCM_CLK_GEN_FRACTIONAL":"HW_PCM_CLK_GEN_INTEGER_ONLY");
                printf("8.  FSC delay:               %s\n\r", (dev->pcm_param.fsc_delay == HW_PCM_FSC_STARTS_1_CYCLE_BEFORE_MSB_BIT) ?
                        "HW_PCM_FSC_STARTS_1_CYCLE_BEFORE_MSB_BIT": "HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT");
                printf("9.  Inverted FSC polarity:   %s\n\r", (dev->pcm_param.inverted_fsc_polarity == HW_PCM_FSC_POLARITY_NORMAL)?
                        "HW_PCM_FSC_POLARITY_NORMAL":"HW_PCM_FSC_POLARITY_INVERTED");
                printf("10. Inverted_clock polarity: %s\n\r", (dev->pcm_param.inverted_clk_polarity == HW_PCM_CLK_POLARITY_NORMAL)?
                        "HW_PCM_CLK_POLARITY_NORMAL":"HW_PCM_CLK_POLARITY_INVERTED");
                printf("11. Cycles per bit:          %s\n\r", (dev->pcm_param.cycle_per_bit == HW_PCM_ONE_CYCLE_PER_BIT) ?
                        "HW_PCM_ONE_CYCLE_PER_BIT": "HW_PCM_TWO_CYCLE_PER_BIT");
                printf("12. FSC length:              %d\n\r", dev->pcm_param.fsc_length);
                break;
        case AUDIO_MEMORY:
                printf("MEMORY <<<\n\r");
                printf("1. Sample rate:              %ld Hz\n\r", dev->memory_param.sample_rate);
                printf("2. Stereo:                   %s\n\r", dev->memory_param.stereo ? "Yes" : "No");
                printf("3. Bits depth:               %d\n\r", dev->memory_param.bit_depth);
                break;
#  if dg_configUSE_HW_SDADC
        case AUDIO_SDADC:
                printf("SDADC <<<\n\r");
                printf("1. PGA_GAIN:                    %d\n\r", dev->sdadc_param.pga_gain);
                printf("2. PGA_MODE:                    %d\n\r", dev->sdadc_param.pga_mode);
                break;
#  endif /* dg_configUSE_HW_SDADC */
        default :
                break;
        }
}

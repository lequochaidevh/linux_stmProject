/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/

/* kernel include */
#include "../ak/ak.h"
#include "../ak/message.h"
#include "../ak/timer.h"
#include "../ak/fsm.h"

/* driver include */
#include "../driver/led/led.h"
#include "../driver/button/button.h"
#include "../driver/flash/flash.h"
#include "../driver/hs1101/hs1101.h"

/* app include */
#include "app.h"
#include "app_dbg.h"
#include "app_bsp.h"
#include "app_flash.h"
#include "app_non_clear_ram.h"

#include "task_list.h"
#include "task_shell.h"
#include "task_life.h"
#include "task_if.h"
#include "task_rf24_if.h"

/* sys include */
#include "../sys/sys_boot.h"
#include "../sys/sys_irq.h"
#include "../sys/sys_io.h"
#include "../sys/sys_ctrl.h"
#include "../sys/sys_dbg.h"
#include "../sys/sys_arduino.h"

/* common include */
#include "../common/utils.h"
#include "../common/screen_manager.h"

#include "../platform/stm32f4/io_cfg.h"
#include "../platform/stm32f4/stm32f4.h"

const char* app_version = APP_VER;

static boot_app_share_data_t boot_app_share_data;

uint16_t mortor1_steps_speed = 0;
uint16_t mortor2_steps_speed = 0;

static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();

/*****************************************************************************/
/* app main function.
 */
/*****************************************************************************/
int main_app() {
	APP_PRINT("app version: %s\n", app_version);

	/******************************************************************************
	* init active kernel
	*******************************************************************************/
	ENTRY_CRITICAL();
	task_init();
	task_create(app_task_table);
	EXIT_CRITICAL();

	/* life led init */
	led_init(&led_life, led_life_init, led_life_on, led_life_off);

    step_motor1_init();
    step_motor2_init();

    GPIO_SetBits(STEP_MOTOR1_DIR_PORT, STEP_MOTOR1_DIR_PIN);
    GPIO_SetBits(STEP_MOTOR1_EN_PORT, STEP_MOTOR1_EN_PIN);

    GPIO_SetBits(STEP_MOTOR2_DIR_PORT, STEP_MOTOR2_DIR_PIN);
    GPIO_SetBits(STEP_MOTOR2_EN_PORT, STEP_MOTOR2_EN_PIN);

    /* button init */
    button_init(&btn_mode,	15,	BUTTON_MODE_ID,	io_button_mode_init,	io_button_mode_read,	btn_mode_callback);
    button_init(&btn_up,	15,	BUTTON_UP_ID,	io_button_up_init,		io_button_up_read,		btn_up_callback);
    button_init(&btn_down,	15,	BUTTON_DOWN_ID,	io_button_down_init,	io_button_down_read,	btn_down_callback);

    button_enable(&btn_mode);
    button_enable(&btn_up);
    button_enable(&btn_down);

    timer_50us_init();
    timer_50us_disable();

    /* flash io init */
    flash_io_ctrl_init();
    windboneFlash_spi_ctrl_init();

    /* increase start time */
    fatal_log_t app_fatal_log;
    flash_read(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, (uint8_t*)&app_fatal_log, sizeof(fatal_log_t));
    app_fatal_log.restart_times ++;
    flash_erase_sector(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR);
    flash_write(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, (uint8_t*)&app_fatal_log, sizeof(fatal_log_t));

	EXIT_CRITICAL();

	/* start timer for application */
	app_init_state_machine();
	app_start_timer();
	/******************************************************************************
	* app task initial
	*******************************************************************************/
	app_task_init();

	/******************************************************************************
	* run applications
	*******************************************************************************/
	return task_run();
}

/*****************************************************************************/
/* app initial function.
 */
/*****************************************************************************/

/* start software timer for application
 * used for app tasks
 */
void app_start_timer() {
	/* start timer to toggle life led */
    timer_set(AC_TASK_LIFE_ID, AC_LIFE_SYSTEM_CHECK, 1000, TIMER_PERIODIC);
//    timer_set(AC_TASK_DBG_ID, AC_DBG_MORTOR_QUAY_THUAN, 5000, TIMER_ONE_SHOT);
}

/* init state machine for tasks
 * used for app tasks
 */
void app_init_state_machine() {
}

/* send first message to trigger start tasks
 * used for app tasks
 */
void app_task_init() {
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/
/* hardware timer interrupt 10ms
 * used for led, button polling
 */
void sys_irq_timer_10ms() {
    //xprintf("sys_irq_timer_10ms\n");
    button_timer_polling(&btn_mode);
    button_timer_polling(&btn_up);
    button_timer_polling(&btn_down);
}

void sys_irq_step_motor_run() {
    mortor1_steps_speed++;
    mortor2_steps_speed++;


    if (mortor1_steps_speed == STEP_MOTOR_1_SPEED) {
        GPIO_SetBits(STEP_MOTOR1_PULSE_PORT, STEP_MOTOR1_PULSE_PIN);
    }
    else if (mortor1_steps_speed == (STEP_MOTOR_1_SPEED*2)) {
        mortor1_steps_speed = 0;
        GPIO_ResetBits(STEP_MOTOR1_PULSE_PORT, STEP_MOTOR1_PULSE_PIN);
    }
    if (mortor2_steps_speed == STEP_MOTOR_2_SPEED) {
        GPIO_SetBits(STEP_MOTOR2_PULSE_PORT, STEP_MOTOR2_PULSE_PIN);
    }
    else if (mortor2_steps_speed == (STEP_MOTOR_2_SPEED*2)) {
        mortor2_steps_speed = 0;
        GPIO_ResetBits(STEP_MOTOR2_PULSE_PORT, STEP_MOTOR2_PULSE_PIN);
    }
}

/* hardware timer interrupt 50ms
 * used for encode and decode ir
 */
void sys_irq_timer_50us() {
}

/* hardware rtc interrupt alarm
 * used for internal rtc alarm
 */
void sys_irq_timer_hs1101() {
}

/* hardware io interrupt at rev ir pin
 * used for decode ir
 */
void sys_irq_ir_io_rev() {
}

void* app_get_boot_share_data() {
	return (void*)&boot_app_share_data;
}

/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <forward_list>
#include <functional>
#include <iterator>
#include <vector>
#include <deque>
#include <queue>
#include <array>
#include <map>

/* kernel include */
#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

/* driver include */
#include "led.h"
#include "button.h"
#include "flash.h"
#include "hs1101.h"

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
#include "task_uart_if.h"
#include "task_display.h"
#include "task_zigbee.h"



/* sys include */
#include "sys_boot.h"
#include "sys_irq.h"
#include "sys_io.h"
#include "sys_ctrl.h"
#include "sys_dbg.h"
#include "sys_thread.h"

/* arduino include */
#include "SPI.h"
#include "WString.h"
#include "HardwareSerial.h"
#include "ArduinoJson.h"

/* common include */
#include "screen_manager.h"
/*devh menu*/
#include "menu_options_list.h"
#include "menu_display.h"
/* ----------------------- Platform includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#if defined (TASK_MBMASTER_EN)
#include "mbport.h"
#include "mbm.h"
#include "mbtypes.h"
#include "common/mbportlayer.h"
#endif

/* ----------------------- Json includes ------------------------------------*/
//#include "json.hpp"

using namespace std;

#define MBM_SERIAL_PORT           ( 0 )
#define MBM_SERIAL_BAUDRATE       ( 57600 )

#if defined(RELEASE)
const char* app_run_mode = "RELEASE";
#else
static const char* app_run_mode = "DEBUG";
#endif

const app_info_t app_info { \
	APP_MAGIC_NUMBER, \
			APP_VER, \
};

static boot_app_share_data_t boot_app_share_data;

static void app_power_on_reset();
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();

//############################################################################
#if defined ( RTOS_DEV_EN )
static sys_thread_t sys_thread_loop_1;
static sys_thread_t sys_thread_loop_2;
static uint32_t loop_1_stack[256];
static uint32_t loop_2_stack[256];

void loop_1_task() {
	while (1) {
		xprintf("--- loop_1_task ---\n");
	}
}

void loop_2_task() {
	while (1) {
		xprintf("+++ loop_2_task +++\n");
	}
}
#endif
//############################################################################

/*****************************************************************************/
/* app main function.
 */
/*****************************************************************************/
int main_app() {
	//############################################################################
#if defined ( RTOS_DEV_EN )
	sys_thread_create(&sys_thread_loop_1, &loop_1_task, loop_1_stack, sizeof(loop_1_stack));
	sys_thread_create(&sys_thread_loop_2, &loop_2_task, loop_2_stack, sizeof(loop_2_stack));
	EXIT_CRITICAL();
	while(1);
#endif
	//############################################################################

	APP_PRINT("App run mode: %s, App version: %d.%d.%d.%d\n", app_run_mode, app_info.version[0] \
			, app_info.version[1]	\
			, app_info.version[2]	\
            , app_info.version[3]);

	sys_soft_reboot_counter++;

	/******************************************************************************
	* init active kernel
	*******************************************************************************/
	ENTRY_CRITICAL();
	task_init();
	task_create((task_t*)app_task_table);
	task_polling_create((task_polling_t*)app_task_polling_table);
	EXIT_CRITICAL();

	/******************************************************************************
	* init applications
	*******************************************************************************/
	/*********************
	* hardware configure *
	**********************/
	/* init watch dog timer */
	sys_ctrl_independent_watchdog_init();	/* 32s */
	sys_ctrl_soft_watchdog_init(200);		/* 20s */

	SPI.begin();
    //view_scr_test_setup();
	/* adc peripheral configure */
//	io_cfg_adc1();			/* configure adc for thermistor and CT sensor */
//	adc_thermistor_io_cfg();

	/* adc configure for ct sensor */
//	adc_ct_io_cfg();

	/* flash io init */
	flash_io_ctrl_init();

	/*********************
	* software configure *
	**********************/
	/* initial boot object */
	sys_boot_init();

	/* life led init */
	led_init(&led_life, led_life_init, led_life_on, led_life_off);

	ring_buffer_char_init(&ring_buffer_console_rev, buffer_console_rev, BUFFER_CONSOLE_REV_SIZE);

	/* button init */
	button_init(&btn_mode,	10,	BUTTON_MODE_ID,	io_button_mode_init,	io_button_mode_read,	btn_mode_callback);
	button_init(&btn_up,	10,	BUTTON_UP_ID,	io_button_up_init,		io_button_up_read,		btn_up_callback);
	button_init(&btn_down,	10,	BUTTON_DOWN_ID,	io_button_down_init,	io_button_down_read,	btn_down_callback);

	button_enable(&btn_mode);
	button_enable(&btn_up);
	button_enable(&btn_down);

	/* get boot share data */
	flash_read(APP_FLASH_INTTERNAL_SHARE_DATA_SECTOR_1, reinterpret_cast<uint8_t*>(&boot_app_share_data), sizeof(boot_app_share_data_t));
	if (boot_app_share_data.is_power_on_reset == SYS_POWER_ON_RESET) {
		app_power_on_reset();
	}

	/* increase start time */
	fatal_log_t app_fatal_log;
	flash_read(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, reinterpret_cast<uint8_t*>(&app_fatal_log), sizeof(fatal_log_t));
	app_fatal_log.restart_times ++;
	flash_erase_sector(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR);
	flash_write(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, reinterpret_cast<uint8_t*>(&app_fatal_log), sizeof(fatal_log_t));

#if defined (TASK_MBMASTER_EN)
	xMBHandle       xMBMMaster;
	eMBMSerialInit( &xMBMMaster, MB_RTU, MBM_SERIAL_PORT, MBM_SERIAL_BAUDRATE, MB_PAR_NONE );
#endif

#if defined (TASK_ZIGBEE_EN)
	Serial2.begin();
	Serial2.setTimeout(100);
#endif

	EXIT_CRITICAL();

#if defined (TASK_ZIGBEE_EN)
	APP_PRINT("start_coordinator(0)\n");
	if (zigbee_network.start_coordinator(0) == 0) {
		APP_PRINT("OK\n");
	}
	else {
		APP_PRINT("NG\n");
	}
#endif

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

	sys_ctrl_shell_sw_to_nonblock();

	return task_run();
}

/*****************************************************************************/
/* polling task can be pushed right here
 * when all ak message queue empty, task_polling_xxx() will be called.
 */
/*****************************************************************************/
void task_polling_zigbee() {
#if defined(TASK_ZIGBEE_EN)
	zigbee_network.update();
#endif
}

void task_polling_console() {
	volatile uint8_t c = 0;

	while (ring_buffer_char_is_empty(&ring_buffer_console_rev) == false) {

		ENTRY_CRITICAL();
		c = ring_buffer_char_get(&ring_buffer_console_rev);
		EXIT_CRITICAL();

#if defined (IF_LINK_UART_EN)
		if (plink_hal_rev_byte(c) == LINK_HAL_IGNORED) {
#endif
			if (shell.index < SHELL_BUFFER_LENGHT - 1) {

				if (c == '\r' || c == '\n') { /* linefeed */

					xputc('\r');
					xputc('\n');

					shell.data[shell.index] = c;
					shell.data[shell.index + 1] = 0;
					task_post_common_msg(AC_TASK_SHELL_ID, AC_SHELL_LOGIN_CMD, (uint8_t*)&shell.data[0], shell.index + 2);

					shell.index = 0;
				}
				else {

					xputc(c);

					if (c == 8 && shell.index) { /* backspace */
						shell.index--;
					}
					else {
						shell.data[shell.index++] = c;
					}
				}
			}
			else {
				LOGIN_PRINT("\nerror: cmd too long, cmd size: %d, try again !\n", SHELL_BUFFER_LENGHT);
				shell.index = 0;
			}
#if defined (IF_LINK_UART_EN)
		}
#endif
	}
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
	timer_set(AC_TASK_LIFE_ID, AC_LIFE_SYSTEM_CHECK, AC_LIFE_TASK_TIMER_LED_LIFE_INTERVAL, TIMER_PERIODIC);
	timer_set(AC_TASK_FW_ID, FW_CHECKING_REQ, FW_UPDATE_REQ_INTERVAL, TIMER_ONE_SHOT);
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
    //SCREEN_CTOR(&scr_mng_app, scr_startup_handle, &scr_startup);

    //SCREEN_CTOR(&scr_mng_appsetup, scr_setup_handle, &scr_setupidle);
    SCREEN_CTOR(&scr_mng_app_menu, scr_startup_handle_wait, &scr_startup_wait);

	task_post_pure_msg(AC_TASK_RF24_IF_ID, AC_RF24_IF_INIT_NETWORK);
	task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_INITIAL);
	task_post_pure_msg(AC_TASK_UART_IF_ID, AC_UART_IF_INIT);
	task_post_pure_msg(AC_TASK_AIRCOND_SCENE_ID, AC_AIRCOND_SCENE_INIT);
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
 */
void sys_irq_timer_10ms() {
	button_timer_polling(&btn_mode);
	button_timer_polling(&btn_up);
	button_timer_polling(&btn_down);
}

/* hardware timer interrupt 50us
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

void sys_irq_usb_recv(uint8_t* data, uint32_t len) {
	(void)data;
	(void)len;
}

/* init non-clear RAM objects
 */
void app_power_on_reset() {
    sys_soft_reboot_counter = 0;
}

void* app_get_boot_share_data() {
	return static_cast<void*>(&boot_app_share_data);
}

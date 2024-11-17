#include "../driver/button/button.h"

#include "../sys/sys_dbg.h"

#include "app.h"
#include "app_bsp.h"
#include "app_dbg.h"

#include "io_cfg.h"

#include "task_list.h"

button_t btn_mode;
button_t btn_up;
button_t btn_down;

uint16_t STEP_MOTOR_1_SPEED = 10;
uint16_t STEP_MOTOR_2_SPEED = 10;
uint16_t STEP_MOTOR_3_SPEED = 10;

void btn_mode_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
        xprintf("[btn_mode_callback] BUTTON_SW_STATE_PRESSED\n");
        timer_50us_disable();
        GPIO_ResetBits(STEP_MOTOR1_EN_PORT, STEP_MOTOR1_EN_PIN);
    }
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
        xprintf("[btn_mode_callback] BUTTON_SW_STATE_LONG_PRESSED\n");
        timer_50us_enable();
        GPIO_ResetBits(STEP_MOTOR1_EN_PORT, STEP_MOTOR1_EN_PIN);
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
        xprintf("[btn_mode_callback] BUTTON_SW_STATE_RELEASED\n");
		task_post_pure_msg(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_ON_LOGO);
	}
		break;

	default:
		break;
	}
}

void btn_up_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
        xprintf("[btn_up_callback] BUTTON_SW_STATE_PRESSED\n");
        GPIO_ResetBits(STEP_MOTOR1_DIR_PORT, STEP_MOTOR1_DIR_PIN);
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
        xprintf("[btn_up_callback] BUTTON_SW_STATE_LONG_PRESSED\n");
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
        xprintf("[btn_up_callback] BUTTON_SW_STATE_RELEASED\n");
	}
		break;

	default:
		break;
	}
}

void btn_down_callback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
        xprintf("[btn_down_callback] BUTTON_SW_STATE_PRESSED\n");
        GPIO_SetBits(STEP_MOTOR1_DIR_PORT, STEP_MOTOR1_DIR_PIN);
	}
		break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
        xprintf("[btn_down_callback] BUTTON_SW_STATE_LONG_PRESSED\n");
	}
		break;

	case BUTTON_SW_STATE_RELEASED: {
        xprintf("[btn_down_callback] BUTTON_SW_STATE_RELEASED\n");
	}
		break;

	default:
		break;
	}
}

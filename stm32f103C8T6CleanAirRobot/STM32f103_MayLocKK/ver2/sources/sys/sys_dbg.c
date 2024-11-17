/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   05/09/2016
 ******************************************************************************
**/
#include "sys_dbg.h"
#include "sys_ctrl.h"

#include "../driver/eeprom/eeprom.h"
#include "../driver/flash/flash.h"

#include "../common/utils.h"

#if defined(STM32L_PLATFORM)
#include "../common/xprintf.h"
#include "../platform/stm32l/io_cfg.h"
#include "../platform/stm32l/sys_cfg.h"
#elif defined(STM32F10X_PLATFORM)
#include "../common/xprintf.h"
#include "../platform/stm32f10x/io_cfg.h"
#include "../platform/stm32f10x/sys_cfg.h"
#else
#error Please choose platform for kernel.
#endif

void sys_dbg_fatal(const int8_t* s, uint8_t c) {
	unsigned char rev_c = 0;

#if defined(TIVA_PLATFORM)
	UARTprintf("%s\t%x\n", s, c);
#endif
#if defined(STM32L_PLATFORM) || defined(STM32F10X_PLATFORM)
	xprintf("%s\t%x\n", s, c);
#endif

	sys_ctrl_delay_us(1000);

#if defined(RELEASE)
	sys_ctrl_reset();
#endif

	/* Disable all interrupt */
	//DISABLE_INTERRUPTS();

	while(1) {
		/* reset watchdog */
		sys_ctrl_independent_watchdog_reset();
		sys_ctrl_soft_watchdog_reset();

		/* FATAL debug option */
		rev_c = sys_ctrl_shell_get_char();
		if (rev_c) {
			switch (rev_c) {
			case 'r':
				sys_ctrl_reset();
				break;

			default:
				break;
			}
		}

		/* led notify FATAL */
		led_life_on();
		sys_ctrl_delay_us(200000);
		led_life_off();
		sys_ctrl_delay_us(200000);
	}
}

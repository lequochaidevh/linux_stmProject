/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   05/09/2016
 ******************************************************************************
**/
#include <stdint.h>

#include "stm32f4.h"

#include "../../sys/sys_dbg.h"

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "core_cmFunc.h"

#if defined(STM32F40XX)
static int nest_entry_critical_counter = 0;

void enable_interrupts() {
	nest_entry_critical_counter--;
	__enable_irq();
}

void disable_interrupts() {
	__disable_irq();
	nest_entry_critical_counter++;
}

void entry_critical() {
	if (nest_entry_critical_counter == 0) {
		__disable_irq();
	}
	nest_entry_critical_counter++;
}

void exit_critical() {
	nest_entry_critical_counter--;
	if (nest_entry_critical_counter == 0) {
		nest_entry_critical_counter = 0;	/* prevent call exit_critical many times */
		__enable_irq();
	}
	else if (nest_entry_critical_counter < 0) {
		FATAL("ITR", 0x01);
	}
}

int get_nest_entry_critical_counter() {
	return nest_entry_critical_counter;
}
#endif

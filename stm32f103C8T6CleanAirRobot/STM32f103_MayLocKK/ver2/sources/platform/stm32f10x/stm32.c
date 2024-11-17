#include <stdint.h>

#include "stm32.h"
#include "stm32f10x_conf.h"

void enable_interrupts() {
    __enable_irq();
}

void disable_interrupts() {
    __disable_irq();
}

static uint8_t nest_entry_critical_counter = 0;

void entry_critical() {
    if (nest_entry_critical_counter == 0) {
        disable_interrupts();
    }
    nest_entry_critical_counter++;
}

void exit_critical() {
    if (nest_entry_critical_counter <= 1) {
        enable_interrupts();
    }
    nest_entry_critical_counter--;
}

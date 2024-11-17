#ifndef __DELAY_H
#define __DELAY_H
#if defined (__cplusplus)
extern "C" {
#endif

#include "../sys/sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

//SysTick STM32F10x
//delay_us,delay_ms

////////////////////////////////////////////////////////////////////////////////// 
	 
extern void delay_init(void);
extern void delay_ms(u16 nms);
extern void delay_us(u32 nus);


#ifdef __cplusplus
}
#endif

#endif





























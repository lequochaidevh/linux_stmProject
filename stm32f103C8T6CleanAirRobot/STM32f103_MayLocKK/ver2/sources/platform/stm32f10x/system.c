/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   05/09/2016
 ******************************************************************************
**/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "sys_cfg.h"
#include "system.h"
#include "stm32.h"

#include "system_stm32f10x.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "core_cm3.h"
#include "io_cfg.h"

#include "../../sys/sys_dbg.h"
#include "../../sys/sys_ctrl.h"
#include "../../sys/sys_irq.h"

#include "../../app/app.h"



////////////////////////////////////////DevH
#include <io_cfg.h>
#include <sys_cfg.h>
#include <xprintf.h>
////////////////////////////////////////DevH
/*****************************************************************************/
/* linker variable                                                           */
/*****************************************************************************/
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t _estack;

extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();
extern void _init();

/*****************************************************************************/
/* static function prototype                                                 */
/*****************************************************************************/
/*****************************/
/* system interrupt function */
/*****************************/
void default_handler();
void reset_handler();

/*****************************/
/* user interrupt function   */
/*****************************/
void exti_line1_irq();
void exti_line15_irq();
void timer6_irq();
void timer4_irq();
void timer7_irq();
void timer3_irq();
void uart2_irq(void);		/* UART interface cpu interrupt */
void uart3_irq(void);		/*UART RS485 interrupt*/
/* cortex-M processor fault exceptions */
void nmi_handler()          __attribute__ ((weak));
void hard_fault_handler()   __attribute__ ((weak));
void mem_manage_handler()   __attribute__ ((weak));
void bus_fault_handler()    __attribute__ ((weak));
void usage_fault_handler()  __attribute__ ((weak));

/* cortex-M processor non-fault exceptions */
void svc_handler()          __attribute__ ((weak, alias("default_handler")));
void dg_monitor_handler()   __attribute__ ((weak, alias("default_handler")));
void pendsv_handler()       __attribute__ ((weak, alias("default_handler")));
void systick_handler();

/* external interrupts */
static void shell_handler();

/*****************************************************************************/
/* system variable                                                           */
/*****************************************************************************/
system_info_t system_info;

/*****************************************************************************/
/* interrupt vector table                                                    */
/*****************************************************************************/
__attribute__((section(".isr_vector")))
void (* const isr_vector[])() = {
		((void (*)())(uint32_t)&_estack)	,	//	The initial stack pointer
        reset_handler						,	//	The reset handler
		nmi_handler							,	//	The NMI handler
		hard_fault_handler					,	//	The hard fault handler
		mem_manage_handler					,	//	The MPU fault handler
		bus_fault_handler					,	//	The bus fault handler
		usage_fault_handler					,	//	The usage fault handler
		0									,	//	Reserved
		0									,	//	Reserved
		0									,	//	Reserved
		0									,	//	Reserved
		svc_handler							,	//	SVCall handler
		dg_monitor_handler					,	//	Debug monitor handler
		0									,	//	Reserved
		pendsv_handler						,	//	The PendSV handler
		systick_handler						,	//	The SysTick handler

		//External Interrupts
		default_handler						,	//	Window Watchdog
		default_handler						,	//	PVD through EXTI Line detect
		default_handler						,	//	Tamper
		default_handler						,	//	RTC
		default_handler						,	//	Flash
		default_handler						,	//	RCC
		default_handler						,	//	EXTI Line 0
		exti_line1_irq						,	//	EXTI Line 1
		default_handler						,	//	EXTI Line 2
		default_handler						,	//	EXTI Line 3
		default_handler						,	//	EXTI Line 4
		default_handler						,	//	DMA1 Channel 1
		default_handler						,	//	DMA1 Channel 2
		default_handler						,	//	DMA1 Channel 3
		default_handler						,	//	DMA1 Channel 4
		default_handler						,	//	DMA1 Channel 5
		default_handler						,	//	DMA1 Channel 6
		default_handler						,	//	DMA1 Channel 7
		default_handler						,	//	ADC1_2
		default_handler						,	//	USB High Priority or CAN1 TX
		default_handler						,	//	USB Low  Priority or CAN1 RX0
		default_handler						,	//	CAN1 RX1
		default_handler						,	//	CAN1 SCE
		default_handler						,	//	EXTI Line 9..5
		default_handler						,	//	TIM1 Break
		default_handler						,	//	TIM1 Update
		default_handler						,	//	TIM1 Trigger and Commutation
		default_handler						,	//	TIM1 Capture Compare
		default_handler						,	//	TIM2
        timer3_irq                           ,	//	TIM3
		default_handler						,	//	TIM4
		default_handler						,	//	I2C1 Event
		default_handler						,	//	I2C1 Error
		default_handler						,	//	I2C2 Event
		default_handler						,	//	I2C2 Error
		default_handler						,	//	SPI1
		default_handler						,	//	SPI2
        shell_handler  						,	//	USART1      shell_handler
        uart2_irq                           ,	//	USART2
        uart3_irq                           ,	//	USART3
		default_handler						,	//	EXTI Line 15..10
		default_handler						,	//	RTC Alarm through EXTI Line
		default_handler						,	//	USB Wakeup from suspend
		};

void __attribute__((naked))
sys_ctrl_delay(volatile uint32_t count)
{
	__asm("    subs    r0, #1\n"
	"    bne     sys_ctrl_delay\n"
	"    bx      lr");
}

static uint32_t millis_current  = 0;

uint32_t sys_ctrl_millis() {
	volatile uint32_t ret;
	ENTRY_CRITICAL();
	ret = millis_current;
	EXIT_CRITICAL();
	return ret;
}

void _init() {
	/* dummy */
}

/*****************************************************************************/
/* static function defination                                                */
/*****************************************************************************/
void default_handler() {
}
GPIO_InitTypeDef GPIO_InitStructure;
void reset_handler() {
	uint32_t *pSrc	= &_ldata;
	uint32_t *pDest	= &_data;
	volatile unsigned i, cnt;

	/* JTAG-DP Disabled and SW-DP Enabled */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	/* init system */
	SystemInit();

	/* copy init data from FLASH to SRAM */
	while(pDest < &_edata) {
		*pDest++ = *pSrc++;
	}

	/* zero bss */
	for (pDest = &_bss; pDest < &_ebss; pDest++) {
		*pDest = 0UL;
	}

	sys_cfg_clock();
    sys_cfg_tick();     /* system tick 1ms */
    sys_cfg_console();  /* system console */

	/* invoke all static constructors */
	cnt = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < cnt; i++)
		__preinit_array_start[i]();

	_init();

	cnt = __init_array_end - __init_array_start;
	for (i = 0; i < cnt; i++)
		__init_array_start[i]();

	/* wait configuration stable */
	sys_ctrl_delay(100);  /* wait 300 cycles clock */

	/* update system information */
	sys_cfg_update_info();

	/* entry app function */
	main_app();
}

/***************************************/
/* cortex-M processor fault exceptions */
/***************************************/
void nmi_handler() {
}

void hard_fault_handler() {
}

void mem_manage_handler() {
}

void bus_fault_handler() {
}

void usage_fault_handler() {
}

/*******************************************/
/* cortex-M processor non-fault exceptions */
/*******************************************/
void systick_handler() {
	static uint32_t div_counter = 0;

	/* increasing millis counter */
	millis_current++;

	if (div_counter == 10) {
		div_counter = 0;
	}

	switch(div_counter) {
	case 0:
		break;

	case 1:
		sys_irq_timer_10ms();
		break;

	default:
		break;
	}
	div_counter++;
}

/************************/
/* external interrupts  */
/************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH
        uint8_t i=0;
        uint8_t state_printf = 0,tim3_print = 0, index_c = 0, flag_read = 0, int_datafeedback = 0;
        uint8_t roomset;
        uint8_t c[255];
        //uint8_t flag_reset = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH
void shell_handler() {

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH

        c[i] = (char)USART_ReceiveData(USART1);
        xprintf("%c",c[i]);

        if(c[i]==13)
        {
            if (c[i-1]=='s')
            {
                xprintf("UART1! ");
                led_life_off();
                xprintf("Led13on!");
                for(int j=0;j<i; j++)
                {
                    xprintf(" ");
                }

            }
            else if(c[i-1]=='r')
            {

                xprintf("UART1! ");
                led_life_on();
                xprintf("Led13off!");
                for(int j=0;j<i; j++)
                {
                    xprintf(" ");
                }
            }
            i=0;
            xprintf("\n");
         }
        else {
            i++;
        }
        /*if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
        {
            USART_SendData(USART1,c[i]);
        }*/
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH
    }
}

void exti_line1_irq() {
	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void exti_line15_irq() {
}

void timer6_irq() {
}

void timer7_irq() {
}

void timer4_irq() {
}
void timer3_irq() {

    if (TIM_GetFlagStatus(TIM3,TIM_IT_Update) != RESET)
    {
        TIM_ClearFlag(TIM3,TIM_IT_Update);
        tim3_print  ++;
        if(tim3_print == 1)
        {
            flag_read = 1;
            led_life_on();
        }
        else if (tim3_print == 31)
        {
            flag_read = 2;
            led_life_off();
        }
        else if (tim3_print == 61)
        {
            flag_read = 3;
             led_life_on();
        }
        else if (tim3_print == 91)
        {
            flag_read = 4;
            led_life_off();
        }
        else if (tim3_print == 121)
        {
            flag_read = 5;
            led_life_on();
        }
        else if (tim3_print == 151)
        {
            flag_read = 6;
            led_life_off();
        }
        else if (tim3_print == 161)
        {
            flag_read = 0;
            //flag_reset ++;
            tim3_print = 0;
            tim_reset = 0;
            //led_life_off();
        }
        }
        else{}
        /*if(flag_reset == 3){
            flag_reset = 0;
            reset_handler();
        }*/
    }



void uart3_irq(void) {
}
void uart2_irq(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        c[i] = USART_ReceiveData(USART2);
        if(c[i]=='\n')
        {

            c[i] = '\0';
            if(c[0] == 'f')
            {
                if (c[1] == '1')
                {
                    PWM_3(200/100*40);
                    int_datafeedback = 4;
                }
                else if(c[1] == '2')
                {
                    PWM_3(200/100*70);
                    int_datafeedback = 5;
                }
                else if(c[1] == '3')
                {
                    PWM_3(200/100*100);
                    int_datafeedback = 6;
                }
            }

            else if(c[0] == 'c')
            {
                if (c[1] == '1')
                {
                    roomset = 1;
                    int_datafeedback = 1;
                }
                else if(c[2] == '2')
                {
                    roomset = 2;
                    int_datafeedback = 2;
                }
                else if(c[1] == '3')
                {
                    roomset = 3;
                    int_datafeedback = 3;
                }
            }

            i=0;
        }
        else {
            i++;
        }

        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

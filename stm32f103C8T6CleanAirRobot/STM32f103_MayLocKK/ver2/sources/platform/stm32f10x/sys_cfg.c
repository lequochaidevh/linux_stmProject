/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   05/09/2016
 ******************************************************************************
**/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "sys_cfg.h"
#include "system.h"

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

#include "../../common/xprintf.h"
#include "../../sys/sys_dbg.h"
#include "../../sys/sys_def.h"
#include "../../sys/sys_ctrl.h"

/* Private define */
static uint32_t delay_coeficient = 0;
static void xputchar(uint8_t c);
static void xputchar_2(uint8_t c);

#define IPCPU_SOP_CHAR	0xEF
#define SOP_STATE		0x00
#define LEN_STATE		0x01
#define DATA_STATE		0x02
#define FCS_STATE		0x03
static uint8_t rx_frame_state = SOP_STATE;
#define IFCPU_DATA_SIZE		256
#define RX_BUFFER_SIZE		256
typedef struct {
    uint8_t frame_sop;
    uint32_t len;
    uint8_t data_index;
    uint8_t data[IFCPU_DATA_SIZE];
    uint8_t frame_fcs;
} if_cpu_serial_frame_t;

if_cpu_serial_frame_t if_cpu_serial_frame;

uint8_t tx_buffer[255];
uint8_t buffer_read[255];
/******************************************************************************
* system configure function
*******************************************************************************/
void sys_cfg_clock() {
	/* Enable the HSI oscillator */
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	SystemCoreClockUpdate();
}

void sys_cfg_tick() {
	SysTick_Config(SystemCoreClock / 1000); /* tick 1ms */
}

void sys_cfg_console() {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(USARTx_CLK, ENABLE);

	/* Configure USART Tx and Rx as alternate function push-pull */
	//GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
	GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

	/* USARTx configuration */
	USART_InitStructure.USART_BaudRate = SYS_CONSOLE_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);

	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(USARTx,USART_IT_RXNE);
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USARTx, ENABLE);

    xfunc_out = xputchar;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH

void Usart2_Init()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);

    /* Enable USART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure USART Tx and Rx as alternate function push-pull */
    //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

    /* USARTx configuration */
    USART_InitStructure.USART_BaudRate = SYS_CONSOLE_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    /* NVIC configuration */
    /* Configure the Priority Group to 2 bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);


}

uint8_t if_cpu_serial_external_device_calcfcs(uint8_t len, uint8_t *data_ptr) {
    uint8_t xor_result;
    xor_result = len;

    for (int i = 0; i < len; i++, data_ptr++) {
        xor_result = xor_result ^ *data_ptr;
    }

    return xor_result;
}

void app_interface_ui(uint8_t cmd, uint8_t * data, uint8_t len) {

    uint8_t buffer[200];    uint8_t temp[200];
    len++;
    buffer[0] = 0xEF;
    buffer[1] = len;
    buffer[2] = cmd;

    temp[0] = cmd;
    memcpy(&temp[1], &data[0], len-1);
    memcpy(&buffer[3], &data[0], len-1);

    buffer[len + 2]   =   if_cpu_serial_external_device_calcfcs(len, temp);

    for (uint8_t i = 0; i < (len + 3); i++) {
        USART_SendData(USART2, buffer[i]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}

void rx_frame_parser(uint8_t* data, uint8_t len) {
    uint8_t ch; uint8_t temp_char[255];
    int rx_remain;
    while(len) {
        ch = *data++;
        len--;
        switch (rx_frame_state) {
        case SOP_STATE: {
            if (IPCPU_SOP_CHAR == ch) {
                rx_frame_state = LEN_STATE;
                }
        }
            break;
        case LEN_STATE: {
            if (ch > IFCPU_DATA_SIZE) {
                rx_frame_state = SOP_STATE;
                return;
            }
            else {
                if_cpu_serial_frame.len = ch;
                if_cpu_serial_frame.data_index = 0;
                rx_frame_state = DATA_STATE;
                }
        }
            break;
        case DATA_STATE: {
            if_cpu_serial_frame.data[if_cpu_serial_frame.data_index++] = ch;
            rx_remain = if_cpu_serial_frame.len - if_cpu_serial_frame.data_index;
            if (len >= rx_remain) {
                memcpy((uint8_t*)(if_cpu_serial_frame.data + if_cpu_serial_frame.data_index), data, rx_remain);
                if_cpu_serial_frame.data_index += rx_remain;
                len -= rx_remain;
                data += rx_remain;
            }
            else {
                memcpy((uint8_t*)(if_cpu_serial_frame.data + if_cpu_serial_frame.data_index), data, len);
                if_cpu_serial_frame.data_index += len;
                len = 0;
            }
            if (if_cpu_serial_frame.data_index == if_cpu_serial_frame.len) {
                rx_frame_state = FCS_STATE;

            }
        }
            break;
        case FCS_STATE: {
            rx_frame_state = SOP_STATE;
            if_cpu_serial_frame.frame_fcs = ch;
            if (if_cpu_serial_frame.frame_fcs
                    == if_cpu_serial_external_device_calcfcs(if_cpu_serial_frame.len, if_cpu_serial_frame.data)) {
                memcpy((uint8_t*)&temp_char[0], (uint8_t*)&if_cpu_serial_frame.data[0], if_cpu_serial_frame.len);
                temp_char[if_cpu_serial_frame.len-1] = '\0';
                xprintf("%s\n",temp_char);
                /*switch (if_cpu_serial_frame.data[0]) {
                case SL_SENSOR_TEMP: {

                    memcpy((uint8_t*)&object_data, (uint8_t*)&if_cpu_serial_frame.data[1], if_cpu_serial_frame.len);

                    qDebug("object_data.cmd     = %d", object_data.cmd);
                    qDebug("object_data.x     = %d", object_data.x);
                    qDebug("object_data.y     = %d", object_data.y);
                    qDebug("object_data.point = %d", object_data.point);
                }
                    break;


                case SL_SENSOR_HUM: {
                    qDebug("SL_SENSOR_HUM");
                    memcpy((uint8_t*)&object_b, (uint8_t*)&if_cpu_serial_frame.data[1], if_cpu_serial_frame.len);

                    qDebug("object_data.cmd     = %d", object_b.cmd);
                    qDebug("object_data.temp     = %d", object_b.temp);
                    qDebug("object_data.humi     = %d", object_b.humi);
                }
                    break;

                }*/
            }
            else {
                /* TODO: handle checksum incorrect */
            }
        }
            break;
        default:
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH

void sys_cfg_update_info() {
	extern uint32_t isr_vector;
	extern uint32_t _end_flash;
	extern uint32_t _start_ram;
	extern uint32_t _end_ram;
	extern uint32_t _data;
	extern uint32_t _edata;
	extern uint32_t _bss;
	extern uint32_t _ebss;
	extern uint32_t __heap_start__;
	extern uint32_t __heap_end__;

	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);

	system_info.cpu_clock = RCC_Clocks.HCLK_Frequency;
	system_info.tick      = 1;
	system_info.console_baudrate = SYS_CONSOLE_BAUDRATE;
	system_info.flash_used = (uint32_t)&_end_flash - (uint32_t)&isr_vector + ((uint32_t)&_edata - (uint32_t)&_data);
	system_info.ram_used = (uint32_t)&_end_ram - (uint32_t)&_start_ram;
	system_info.data_used = (uint32_t)&_edata - (uint32_t)&_data;
	system_info.stack_used = (uint32_t)&_ebss - (uint32_t)&_bss;
	system_info.heap_size = (uint32_t)&__heap_end__ - (uint32_t)&__heap_start__;

	delay_coeficient = system_info.cpu_clock /1000000;

	/* system banner */
	SYS_PRINT("system information:\n");
	SYS_PRINT("\tFLASH used:\t%d bytes\n", system_info.flash_used);
	SYS_PRINT("\tSRAM used:\t%d bytes\n", system_info.ram_used);
	SYS_PRINT("\t\tdata used:\t%d bytes\n", system_info.data_used);
	SYS_PRINT("\t\tstack used:\t%d bytes\n", system_info.stack_used);
	SYS_PRINT("\t\theap size:\t%d bytes\n", system_info.heap_size);
	SYS_PRINT("\n");
	SYS_PRINT("\tcpu clock:\t%d Hz\n", system_info.cpu_clock);
	SYS_PRINT("\ttick:\t%d ms\n", system_info.tick);
	SYS_PRINT("\tconsole baudrate:\t%d bps\n", system_info.console_baudrate);
	SYS_PRINT("\n\n");
}

/******************************************************************************
* system utilities function
*******************************************************************************/
void xputchar(uint8_t c) {
    USART_SendData(USARTx, (uint8_t)c);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

uint8_t sys_ctrl_shell_get_char() {
	uint8_t c = 0;
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) == SET) {
        c = (uint8_t)USART_ReceiveData(USARTx);
	}
	return c;
}

void sys_ctrl_reset() {
	NVIC_SystemReset();
}

void sys_ctrl_delay_us(volatile uint32_t count) {
	volatile uint32_t delay_value = 0;
	delay_value = count*delay_coeficient / 8;
	while(delay_value--);
}

void sys_ctrl_delay_ms(volatile uint32_t count) {
#if 0
	volatile uint32_t current_time = 0;
	volatile uint32_t current;
	volatile int32_t start = sys_ctrl_millis();

	while(current_time < count) {
		current = sys_ctrl_millis();

		if (current < start) {
			current_time += ((uint32_t)0xFFFFFFFF - start) + current;
		}
		else {
			current_time += current - start;
		}

		start = current;
	}
#else
	volatile uint32_t delay = 1000 * count;
	sys_ctrl_delay_us(delay);
#endif
}

void sys_ctrl_independent_watchdog_init() {
//		/* enable PCLK1 for watchdog timer */
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

//		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//		IWDG_SetPrescaler(IWDG_Prescaler_256);		/* IWDG counter clock: 37KHz(LSI) / 256 = 0.144 KHz */
//		IWDG_SetReload(0xFFF);						/* Set counter reload, T = (1/IWDG counter clock) * Reload_counter  = 30s  */
//		IWDG_ReloadCounter();
//		IWDG_Enable();
}

void sys_ctrl_independent_watchdog_reset() {
//		ENTRY_CRITICAL();
//		IWDG_ReloadCounter();
//		EXIT_CRITICAL();
}

//static uint32_t sys_ctrl_soft_counter = 0;
//static uint32_t sys_ctrl_soft_time_out;

void sys_ctrl_soft_watchdog_init(uint32_t time_out) {
	//	TIM_TimeBaseInitTypeDef  timer_100us;
	//	NVIC_InitTypeDef NVIC_InitStructure;
	//	sys_ctrl_soft_time_out = time_out;

	//	/* timer 10ms to polling receive IR signal */
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	//	timer_100us.TIM_Period = 799;		/* HCLK=8Mhz and 3196 respective HCLK=32Mhz;*/
	//	timer_100us.TIM_Prescaler = 1000;
	//	timer_100us.TIM_ClockDivision = 0;
	//	timer_100us.TIM_CounterMode = TIM_CounterMode_Down;
	//	TIM_TimeBaseInit(TIM7, &timer_100us);

	//	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//	NVIC_Init(&NVIC_InitStructure);

	//	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	//	TIM_Cmd(TIM7, ENABLE);
}

void sys_ctrl_soft_watchdog_reset() {
	//	ENTRY_CRITICAL();
	//	sys_ctrl_soft_counter = 0;
	//	EXIT_CRITICAL();
}

void sys_ctrl_soft_watchdog_enable() {
	//	ENTRY_CRITICAL();
	//	TIM_Cmd(TIM7, ENABLE);
	//	EXIT_CRITICAL();
}

void sys_ctrl_soft_watchdog_disable() {
	//	ENTRY_CRITICAL();
	//	TIM_Cmd(TIM7, DISABLE);
	//	EXIT_CRITICAL();
}

void sys_ctrl_soft_watchdog_increase_counter() {
	//	sys_ctrl_soft_counter++;
	//	if (sys_ctrl_soft_counter >= sys_ctrl_soft_time_out) {
	//		FATAL("SWDG", 0x01);
	//	}
}

void sys_ctrl_get_firmware_info(firmware_header_t* header) {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _data;
	extern uint32_t _edata;

	uint32_t check_sum = 0;
	uint32_t len_of_flash = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data);

	for (uint32_t index = (uint32_t)&(_start_flash); index <= ((uint32_t)&(_start_flash) + len_of_flash); index += sizeof(uint32_t)) {
		check_sum += *((uint32_t*)index);
	}

	header->checksum = (check_sum & 0xFFFF);
	header->bin_len = len_of_flash;
}

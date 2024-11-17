/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   05/09/2016
 * @Update:
 * @AnhHH: Add io function for sth11 sensor.
 ******************************************************************************
**/
#ifndef __IO_CFG_H__
#define __IO_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "../driver/rtc/rtc.h"
#include "../driver/eeprom/eeprom.h"
#include "../platform/stm32f10x/io_cfg.h"

/*
 * define pin for arduino pinMode/digitalWrite/digitalRead
 * NOTE: define value MUST be deferrent
 */

//#define ndebug 1


/*****************************************************************************
 *RS485 pin Arduino define
******************************************************************************/
#define Arduino_RS485_DRIECTION_PIN		11
#define Arduino_RS485_RELAY1_PIN		0
#define Arduino_RS485_RELAY2_PIN		1
#define Arduino_RS485_RELAY3_PIN		2
#define Arduino_RS485_RELAY4_PIN		3
#define Arduino_RS485_RELAY5_PIN		4
#define Arduino_RS485_RELAY6_PIN		5

/******************************************************************************
 *Pin map button
*******************************************************************************/
#define BUTTON_MODE_IO_PIN				(GPIO_Pin_3)
#define BUTTON_MODE_IO_PORT				(GPIOB)
#define BUTTON_MODE_IO_CLOCK			(RCC_APB2Periph_GPIOB)

#define BUTTON_UP_IO_PIN				(GPIO_Pin_4)
#define BUTTON_UP_IO_PORT				(GPIOB)
#define BUTTON_UP_IO_CLOCK				(RCC_APB2Periph_GPIOB)

#define BUTTON_DOWN_IO_PIN				(GPIO_Pin_12)
#define BUTTON_DOWN_IO_PORT				(GPIOC)
#define BUTTON_DOWN_IO_CLOCK			(RCC_APB2Periph_GPIOC)

/*****************************************************************************
 *Pin map LINE
******************************************************************************/
#define LINE_IO_PIN					(GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6)
#define LINE_IO_PORT				(GPIOB)
#define LINE_IO_CLOCK				(RCC_APB2Periph_GPIOB)

/*****************************************************************************
 *Pin map led life
******************************************************************************/
#define LED_LIFE_IO_PIN					(GPIO_Pin_13)
#define LED_LIFE_IO_PORT				(GPIOC)
#define LED_LIFE_IO_CLOCK				(RCC_APB2Periph_GPIOC)

/*****************************************************************************
 *Pin map Flash W2508
******************************************************************************/
#define FLASH_CE_IO_PIN					(GPIO_Pin_10)
#define FLASH_CE_IO_PORT				(GPIOB)
#define FLASH_CE_IO_CLOCK				(RCC_APB2Periph_GPIOB)

/****************************************************************************
 * RS485 defines
*****************************************************************************/
#define USART_RS485							(USART3)
#define USART_RS485_CLK						(RCC_APB1Periph_USART3)
#define USART_RS485_IRQn					(USART3_IRQn)

#define USART_RS485_TX_PIN					(GPIO_Pin_10)
#define USART_RS485_TX_GPIO_PORT			(GPIOB)
#define USART_RS485_TX_GPIO_CLK				(RCC_APB2Periph_GPIOB)
#define USART_RS485_TX_SOURCE				(GPIO_PinSource10)

#define USART_RS485_RX_PIN					(GPIO_Pin_11)
#define USART_RS485_RX_GPIO_PORT			(GPIOB)
#define USART_RS485_RX_GPIO_CLK				(RCC_APB2Periph_GPIOB)
#define USART_RS485_RX_SOURCE				(GPIO_PinSource11)

/*RS485 dir IO*/
#define RS485_DIR_IO_PIN					(GPIO_Pin_12)
#define RS485_DIR_IO_PORT					(GPIOB)
#define RS485_DIR_IO_CLOCK					(RCC_APB2Periph_GPIOB)

/*****************************************************************************
 *Pin map relay
******************************************************************************/
#define RELAY_1_IO_PIN					(GPIO_Pin_12)
#define RELAY_1_IO_PORT					(GPIOB)
#define RELAY_1_IO_CLOCK				(RCC_APB2Periph_GPIOB)

#define RELAY_2_IO_PIN					(GPIO_Pin_13)
#define RELAY_2_IO_PORT					(GPIOB)
#define RELAY_2_IO_CLOCK				(RCC_APB2Periph_GPIOB)

#define RELAY_3_IO_PIN					(GPIO_Pin_14)
#define RELAY_3_IO_PORT					(GPIOB)
#define RELAY_3_IO_CLOCK				(RCC_APB2Periph_GPIOB)

#define RELAY_4_IO_PIN					(GPIO_Pin_15)
#define RELAY_4_IO_PORT					(GPIOB)
#define RELAY_4_IO_CLOCK				(RCC_APB2Periph_GPIOB)

#define RELAY_5_IO_PIN					(GPIO_Pin_8)
#define RELAY_5_IO_PORT					(GPIOA)
#define RELAY_5_IO_CLOCK				(RCC_APB2Periph_GPIOA)

/****************************************************************************
 *Pin map CT - sensor
*****************************************************************************/
#define CT_THER_ADC_CLOCK			(RCC_APB2Periph_ADC1)

#define CT_ADC_PORT					(GPIOA)
#define CT_ADC_IO_CLOCK				(RCC_APB2Periph_GPIOA)

#define CT1_ADC_PIN					(GPIO_Pin_6)
#define CT2_ADC_PIN					(GPIO_Pin_3)

#define CT1_ADC_CHANEL				(ADC_Channel_6)
#define CT2_ADC_CHANEL				(ADC_Channel_3)

/****************************************************************************
 *74hc4052 io control pin
*****************************************************************************/
#define IO_74HC4052_S0_PIN			(GPIO_Pin_5)
#define IO_74HC4052_S0_PIN_PORT		(GPIOA)
#define IO_74HC4052_S0_PIN_CLOCK	(RCC_APB2Periph_GPIOA)

#define IO_74HC4052_S1_PIN			(GPIO_Pin_4)
#define IO_74HC4052_S1_PIN_PORT		(GPIOA)
#define IO_74HC4052_S1_PIN_CLOCK	(RCC_APB2Periph_GPIOA)

#define IO_74hc4052_nY0_nZ				(0)
#define IO_74hc4052_nY1_nZ				(1)
#define IO_74hc4052_nY2_nZ				(2)
#define IO_74hc4052_nY3_nZ				(3)

/******************************************************************************
* led status function
*******************************************************************************/
extern void led_life_init();
extern void led_life_on();
extern void led_life_off();

/******************************************************************************
* flash IO function
*******************************************************************************/
extern void flash_io_ctrl_init();
extern void flash_cs_low();
extern void flash_cs_high();
extern uint8_t flash_transfer(uint8_t);

/******************************************************************************
* hc595 IO function
*******************************************************************************/
extern void io_hc595_init();
extern void io_hc595_put_data1( uint8_t);
extern void io_hc595_put_data2( uint8_t);

/*****************************************************************************
 *io uart for RS485
******************************************************************************/
extern void io_uart_RS485_cfg(uint32_t baudrate );
extern void io_RS485_dir_pin_input();
extern void io_RS485_dir_pin_output();
extern void io_uart_RS485_mode_set(uint8_t mode);
extern void io_RS485_dir_pin_set();
extern void io_RS485_dir_pin_reset();
extern void io_uart_rs485_transfer(uint8_t byte);
extern uint8_t io_uart_RS485_get_byte();
extern void io_RS485_relay_pin_output();
extern void io_RS485_relay5_pin_set();
extern void io_RS485_relay5_pin_reset();
extern void io_RS485_relay4_pin_set();
extern void io_RS485_relay4_pin_reset();
extern void io_RS485_relay3_pin_set();
extern void io_RS485_relay3_pin_reset();
extern void io_RS485_relay2_pin_set();
extern void io_RS485_relay2_pin_reset();
extern void io_RS485_relay1_pin_set();
extern void io_RS485_relay1_pin_reset();

/*****************************************************************************
 *adc ct-sensor config
******************************************************************************/
extern void io_adc_start_cfg(void);
extern uint16_t adc_ct_io_read(uint8_t chanel);

extern void io_adc_ct_cfg();
void IO_74HC4052_S0_PIN_OUTPUT();
void IO_74HC4052_S1_PIN_OUTPUT();

extern void IO_74HC4052_S0_PIN_SET();
extern void IO_74HC4052_S1_PIN_SET();
extern void IO_74HC4052_S0_PIN_RESET();
extern void IO_74HC4052_S1_PIN_RESET();

void IO_74HC4052_PIN_TO_READ(uint8_t pin);


extern void led_test();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////DevH
extern void GPIO_line_init();
extern void IO_LED_control();
extern void TIM3_Int_Init(uint16_t arr,uint16_t psc);
extern void PWM_Init();
extern void PWM_1(uint8_t Power);
extern void PWM_2(uint8_t Power);
extern void PWM_3(uint8_t Power);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH


#ifdef __cplusplus
}
#endif


#endif //__IO_CFG_H__

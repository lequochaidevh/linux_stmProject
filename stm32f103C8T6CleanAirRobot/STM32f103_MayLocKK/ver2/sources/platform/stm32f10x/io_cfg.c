/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   05/09/2016
 * @Update:
 * @AnhHH: Add io function for sth11 sensor.
 ******************************************************************************
**/
#include <stdint.h>
#include <stdbool.h>

#include "io_cfg.h"
#include "stm32.h"
#include "stm32f10x.h"

#include "arduino/Arduino.h"

#include "../sys/sys_dbg.h"

#include "../common/utils.h"
#include "../app/app_dbg.h"
#include "stm32f10x_tim.h"

/******************************************************************************
* led status function
*******************************************************************************/
void led_life_init() {
	GPIO_InitTypeDef        GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_LIFE_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_LIFE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_LIFE_IO_PORT, &GPIO_InitStructure);
}

void led_life_on() {
	GPIO_SetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

void led_life_off() {
	GPIO_ResetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

void led_test() {
    GPIO_ResetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH
/*void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler =   psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = 28;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}*/
void GPIO_line_init()
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LINE_IO_CLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LINE_IO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LINE_IO_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}
void IO_LED_control()
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler =   psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

void PWM_Init()
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef PWM_TimeBaseStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler =   71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter =   0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM4, ENABLE);

    PWM_TimeBaseStructure.TIM_OCMode        =   TIM_OCMode_PWM1;
    PWM_TimeBaseStructure.TIM_OCPolarity    =   TIM_OCPolarity_High;
    PWM_TimeBaseStructure.TIM_OutputState   =   TIM_OutputState_Enable;
    PWM_TimeBaseStructure.TIM_Pulse         =   0;
    TIM_OC3Init(TIM4,&PWM_TimeBaseStructure);

    PWM_TimeBaseStructure.TIM_OCMode        =   TIM_OCMode_PWM1;
    PWM_TimeBaseStructure.TIM_OCPolarity    =   TIM_OCPolarity_High;
    PWM_TimeBaseStructure.TIM_OutputState   =   TIM_OutputState_Enable;
    PWM_TimeBaseStructure.TIM_Pulse         =   0;
    TIM_OC4Init(TIM4,&PWM_TimeBaseStructure);

    PWM_TimeBaseStructure.TIM_OCMode        =   TIM_OCMode_PWM1;
    PWM_TimeBaseStructure.TIM_OCPolarity    =   TIM_OCPolarity_High;
    PWM_TimeBaseStructure.TIM_OutputState   =   TIM_OutputState_Enable;
    PWM_TimeBaseStructure.TIM_Pulse         =   0;
    TIM_OC2Init(TIM4,&PWM_TimeBaseStructure);
}

void PWM_1(uint8_t Power)
{
    /*TIM_OCInitTypeDef PWM_TimeBaseStructure;
    PWM_TimeBaseStructure.TIM_Pulse         =   Power*5;
    TIM_OC3Init(TIM4,&PWM_TimeBaseStructure);*/
    TIM_SetCompare3(TIM4,Power*5);

}
void PWM_2(uint8_t Power)
{
    //z/TIM_Period*100% với TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    /*TIM_OCInitTypeDef PWM_TimeBaseStructure;

    PWM_TimeBaseStructure.TIM_Pulse         =   Power*5;
    TIM_OC4Init(TIM4,&PWM_TimeBaseStructure);*/
    TIM_SetCompare4(TIM4, Power*5);
}
void PWM_3(uint8_t Power)
{
    /*TIM_OCInitTypeDef PWM_TimeBaseStructure;

    PWM_TimeBaseStructure.TIM_Pulse         =   Power*5;
    TIM_OC2Init(TIM4,&PWM_TimeBaseStructure);*/
    TIM_SetCompare2(TIM4, Power*5);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DevH
/******************************************************************************
* flash IO config
*******************************************************************************/
void flash_io_ctrl_init() {
	GPIO_InitTypeDef        GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(FLASH_CE_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = FLASH_CE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_CE_IO_PORT, &GPIO_InitStructure);
}

void flash_cs_low() {
	GPIO_ResetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

void flash_cs_high() {
	GPIO_SetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

uint8_t flash_transfer(uint8_t data) {
	unsigned long rxtxData = data;

	/* waiting send idle then send data */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, (uint8_t)rxtxData);

	/* waiting conplete rev data */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	rxtxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)rxtxData;
}

/*****************************************************************************
 *io uart for RS485
******************************************************************************/
void io_uart_RS485_cfg(uint32_t baudrate ) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(USART_RS485_TX_GPIO_CLK, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(USART_RS485_CLK, ENABLE);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= USART_RS485_TX_PIN;
	GPIO_Init(USART_RS485_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin		= USART_RS485_RX_PIN;
	//GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RS485_RX_GPIO_PORT, &GPIO_InitStructure);

	/* USART3 configuration */
	USART_DeInit(USART_RS485);
	USART_InitStructure.USART_BaudRate		= baudrate;
	USART_InitStructure.USART_WordLength	= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;
	USART_InitStructure.USART_Parity		= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_RS485, &USART_InitStructure);

	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART_RS485_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(USART_RS485, USART_IT_RXNE);
	USART_ITConfig(USART_RS485, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART_RS485, ENABLE);
}

/******************************************************************************
* RS485 dir pin mode select
*******************************************************************************/
void io_RS485_dir_pin_input() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RS485_DIR_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RS485_DIR_IO_PIN;
	GPIO_Init(RS485_DIR_IO_PORT, &GPIO_InitStructure);
}

void io_RS485_dir_pin_output() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RS485_DIR_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RS485_DIR_IO_PIN;

	GPIO_Init(RS485_DIR_IO_PORT, &GPIO_InitStructure);
}

void io_uart_RS485_mode_set(uint8_t mode) {
	if(mode == 0) { // output
		GPIO_SetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // set dir pin -> enable TX function
	} else if(mode == 1) { // input
		GPIO_ResetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // reset dir pin -> enable RX function
	}
}

void io_RS485_dir_pin_set() {
	GPIO_SetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_dir_pin_reset() {
	GPIO_ResetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN); // reset dir pin -> enable RX function
}

void io_uart_rs485_transfer(uint8_t byte) {
	USART_SendData(USART_RS485, byte);
	while (USART_GetFlagStatus(USART_RS485, USART_FLAG_TXE) == RESET);
}

uint8_t io_uart_RS485_get_byte() {
	volatile uint8_t c = 0;
	if(USART_GetITStatus(USART_RS485, USART_IT_RXNE) == SET) {
		USART_ClearITPendingBit(USART_RS485, USART_IT_RXNE);
		c = (uint8_t)USART_ReceiveData(USART_RS485);
		APP_DBG("%02X:",c);
	}
	return c;
}

void io_RS485_relay_pin_output() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RELAY_1_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RELAY_1_IO_PIN;
	GPIO_Init(RELAY_1_IO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RELAY_2_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RELAY_2_IO_PIN;
	GPIO_Init(RELAY_2_IO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RELAY_3_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RELAY_3_IO_PIN;
	GPIO_Init(RELAY_3_IO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RELAY_4_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RELAY_4_IO_PIN;
	GPIO_Init(RELAY_4_IO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RELAY_5_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= RELAY_5_IO_PIN;
	GPIO_Init(RELAY_5_IO_PORT, &GPIO_InitStructure);

}

void io_RS485_relay1_pin_set() {
	GPIO_SetBits(RELAY_1_IO_PORT, RELAY_1_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay1_pin_reset() {
	GPIO_ResetBits(RELAY_1_IO_PORT, RELAY_1_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay2_pin_set() {
	GPIO_SetBits(RELAY_2_IO_PORT, RELAY_2_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay2_pin_reset() {
	GPIO_ResetBits(RELAY_2_IO_PORT, RELAY_2_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay3_pin_set() {
	GPIO_SetBits(RELAY_3_IO_PORT, RELAY_3_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay3_pin_reset() {
	GPIO_ResetBits(RELAY_3_IO_PORT, RELAY_3_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay4_pin_set() {
	GPIO_SetBits(RELAY_4_IO_PORT, RELAY_4_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay4_pin_reset() {
	GPIO_ResetBits(RELAY_4_IO_PORT, RELAY_4_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay5_pin_set() {
	GPIO_SetBits(RELAY_5_IO_PORT, RELAY_5_IO_PIN); // reset dir pin -> enable RX function
}

void io_RS485_relay5_pin_reset() {
	GPIO_ResetBits(RELAY_5_IO_PORT, RELAY_5_IO_PIN); // reset dir pin -> enable RX function
}

/* Proceduce to send with RS485
*		io_RS485_dir_pin_set();
*		io_uart_rs485_transfer('a');
*		sys_ctrl_delay_us(1000);
*		io_RS485_dir_pin_reset();
*/

/******************************************************************************
* adc function
* + CT sensor
* + themistor sensor
* Note: MUST be enable internal clock for adc module.
*******************************************************************************/
void io_adc_start_cfg(void) {
	ADC_InitTypeDef ADC_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* Enable ADC1 clock so that we can talk to it */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

void io_adc_ct_cfg() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(CT_ADC_IO_CLOCK , ENABLE);

	GPIO_InitStructure.GPIO_Pin = CT1_ADC_PIN | CT2_ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(CT_ADC_PORT, &GPIO_InitStructure);
}

uint16_t adc_ct_io_read(uint8_t chanel) {
	ADC_RegularChannelConfig(ADC1, chanel, 1, ADC_SampleTime_13Cycles5);

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	return ADC_GetConversionValue(ADC1);
}

void IO_74HC4052_S0_PIN_OUTPUT() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(IO_74HC4052_S0_PIN_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= IO_74HC4052_S0_PIN;

	GPIO_Init(IO_74HC4052_S0_PIN_PORT, &GPIO_InitStructure);
}

void IO_74HC4052_S1_PIN_OUTPUT() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(IO_74HC4052_S1_PIN_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		= IO_74HC4052_S1_PIN;

	GPIO_Init(IO_74HC4052_S1_PIN_PORT, &GPIO_InitStructure);
}

void IO_74HC4052_S0_PIN_SET() {
	GPIO_SetBits(IO_74HC4052_S0_PIN_PORT, IO_74HC4052_S0_PIN); // reset dir pin -> enable RX function
}

void IO_74HC4052_S1_PIN_SET() {
	GPIO_SetBits(IO_74HC4052_S1_PIN_PORT, IO_74HC4052_S1_PIN); // reset dir pin -> enable RX function
}

void IO_74HC4052_S0_PIN_RESET() {
	GPIO_ResetBits(IO_74HC4052_S0_PIN_PORT, IO_74HC4052_S0_PIN); // reset dir pin -> enable RX function
}

void IO_74HC4052_S1_PIN_RESET() {
	GPIO_ResetBits(IO_74HC4052_S1_PIN_PORT, IO_74HC4052_S1_PIN); // reset dir pin -> enable RX function
}

void IO_74HC4052_PIN_TO_READ(uint8_t pin) {
	switch(pin) {
		case IO_74hc4052_nY0_nZ:
			IO_74HC4052_S0_PIN_RESET();
			IO_74HC4052_S1_PIN_RESET();
			break;
		case IO_74hc4052_nY1_nZ:
			IO_74HC4052_S0_PIN_SET();
			IO_74HC4052_S1_PIN_RESET();
			break;
		case IO_74hc4052_nY2_nZ:
			IO_74HC4052_S0_PIN_RESET();
			IO_74HC4052_S1_PIN_SET();
			break;
		case IO_74hc4052_nY3_nZ:
			IO_74HC4052_S0_PIN_SET();
			IO_74HC4052_S1_PIN_SET();
			break;

		default:
		break;
	}
}



